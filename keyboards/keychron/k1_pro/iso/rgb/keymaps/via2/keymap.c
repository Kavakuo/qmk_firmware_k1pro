/* Copyright 2023 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "action_layer.h"
#include "os_detection.h"
#include "process_key_override.h"
#include "quantum_keycodes.h"
#include "rgb_matrix.h"
#include QMK_KEYBOARD_H


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

static bool printDebug = false;

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
    COMMON_FN,
    NUMPAD,
    MOUSE
};

#ifdef CONSOLE_ENABLE
#include "print.h"
#    define DEBUG_LOG(message, ...) uprintf(message, ##__VA_ARGS__)
#else
#    define DEBUG_LOG(message, ...) ((void)0)
#endif


const key_override_t mac_square_bracket_open = ko_make_with_layers(MOD_MASK_ALT, KC_LEFT_BRACKET, ROPT(KC_5), 1);
const key_override_t mac_square_bracket_close = ko_make_with_layers(MOD_MASK_ALT, KC_RIGHT_BRACKET, ROPT(KC_6), 1);
const key_override_t mac_curly_bracket_open = ko_make_with_layers(MOD_BIT_RALT | MOD_BIT_RSHIFT, KC_LEFT_BRACKET, ROPT(KC_8), 1);
const key_override_t mac_curly_bracket_close = ko_make_with_layers(MOD_BIT_RALT | MOD_BIT_RSHIFT, KC_RIGHT_BRACKET, ROPT(KC_9), 1);

const key_override_t win_square_bracket_open = ko_make_with_layers(MOD_BIT_RALT, KC_LEFT_BRACKET, RALT(KC_8), 4);
const key_override_t win_square_bracket_close = ko_make_with_layers(MOD_BIT_RALT, KC_RIGHT_BRACKET, RALT(KC_9), 4);
const key_override_t win_curly_bracket_open = ko_make_with_layers(MOD_BIT_RALT | MOD_MASK_SHIFT, KC_LEFT_BRACKET, RALT(KC_7), 4);
const key_override_t win_curly_bracket_close = ko_make_with_layers(MOD_BIT_RALT | MOD_MASK_SHIFT, KC_RIGHT_BRACKET, RALT(KC_0), 4);


// Curly brackets (alt+shift) must come before sqaure brackets (only alt)
const key_override_t **key_overrides = (const key_override_t *[]){
    &mac_curly_bracket_open,
    &mac_curly_bracket_close,
    &mac_square_bracket_open,
    &mac_square_bracket_close,
    &win_curly_bracket_open,
    &win_curly_bracket_close,
    &win_square_bracket_open,
    &win_square_bracket_close,
	NULL
};


void printBinary(uint16_t m) {
    printf(BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
}


void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
#ifdef CONSOLE_ENABLE
    debug_enable = true;
#endif
    // debug_matrix=true;
    // debug_keyboard=true;
    // debug_mouse=true;
}

enum customKeys {
    // Momentarily turn off highest layer
    CKC_MOF_HL = NEW_SAFE_RANGE,
    CKC_MOF_ALL,
    CKC_MOFN,
    CKC_HARD_RESET,
    CKC_SOFT_RESET
    CKC_SOFT_RESET,
    CKC_DEBUG
};

static bool readyForReset = false;

void notify_usb_device_state_change_user(enum usb_device_state usb_device_state) {
    switch (usb_device_state) {
        case USB_DEVICE_STATE_INIT:
            rgblight_sethsv_noeeprom(HSV_RED);
            break;
        case USB_DEVICE_STATE_NO_INIT:
            rgblight_sethsv_noeeprom(HSV_PURPLE);
            break;
        case USB_DEVICE_STATE_CONFIGURED:
            rgblight_sethsv_noeeprom(HSV_GREEN);
            readyForReset = true;
            break;
        case USB_DEVICE_STATE_SUSPEND:
            rgblight_sethsv_noeeprom(HSV_ORANGE);
            break;
    }
}


void suspend_power_down_user() {
    // Reset Keyboard
    // In some hotunplug scenarios only power down is triggered,
    // but the USB_DEVICE_STATE stays configured. Because of this, OS detection is not retriggered then.
    // Therefore, a soft reset needs to be performed.
    if (readyForReset) {
        DEBUG_LOG("Power down\n");
        readyForReset = false;
        soft_reset_keyboard();
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint8_t cached_highest_layer = 0;
    static layer_state_t cached_layer_state = 0;

    switch (keycode) {
        case CKC_MOF_HL:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_MOF_HL pressed\n");
                uint8_t highest_layer = get_highest_layer(layer_state);
                if (highest_layer > 0) {
                    cached_highest_layer = highest_layer;
                    layer_off(highest_layer);
                }
            } else {
                DEBUG_LOG("CKC_MOF_HL released\n");
                layer_on(cached_highest_layer);
            }
            return false; // Skip all further processing of this key
        case CKC_MOF_ALL:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_MOF_ALL pressed\n");
                cached_layer_state = layer_state;
                layer_clear();
            } else {
                DEBUG_LOG("CKC_MOF_ALL released\n");
                layer_state_set(cached_layer_state);
            }
            return false;
        case CKC_MOFN:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_MOFN pressed\n");
                layer_on(COMMON_FN);
                layer_on(get_highest_layer(default_layer_state)+1);
            } else {
                DEBUG_LOG("CKC_MOFN released\n");
                layer_off(COMMON_FN);
                layer_off(get_highest_layer(default_layer_state)+1);
            }
            return false;
        case CKC_HARD_RESET:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_HARD_RESET pressed\n");
                reset_keyboard();
            }
            return false;
        case CKC_SOFT_RESET:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_SOFT_RESET pressed\n");
                soft_reset_keyboard();
            }
            return false;
        case CKC_DEBUG:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_DEBUG pressed\n");
                printDebug = !printDebug;
            }
        default:
            return true; // Process all other keycodes normally
    }
}

HSV getHSVForLayer(uint8_t layerId) {
    switch (layerId) {
        case MAC_BASE:
            return (HSV){HSV_WHITE};
        case WIN_BASE:
            return (HSV){150, 247, 0xff};
        case MAC_FN:
        case WIN_FN:
        case COMMON_FN:
            return (HSV){HSV_YELLOW};
        case NUMPAD:
            return (HSV){HSV_GREEN};
        case MOUSE:
            return (HSV){HSV_PURPLE};
        default:
            return (HSV){HSV_RED};
    }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    DEBUG_LOG("default_layer_state_set_user: %08hX(%u)\n", state, get_highest_layer(state));
    HSV hsv = getHSVForLayer(get_highest_layer(state));
    rgblight_sethsv(hsv.h, hsv.s, hsv.v);
    return state;
}

bool process_detected_host_os_user(os_variant_t detected_os) {
    switch (detected_os) {
        case OS_MACOS:
        case OS_IOS:
            DEBUG_LOG("OS Detection Mac/iOS\n");
            default_layer_set(1UL << MAC_BASE);
            break;
        case OS_WINDOWS:
        case OS_LINUX:
            DEBUG_LOG("OS Detection Windows/Linux\n");
            default_layer_set(1UL << WIN_BASE);
            break;
        case OS_UNSURE:
            rgblight_sethsv(HSV_PURPLE);
            break;
    }

    return true;
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (get_highest_layer(layer_state) > 0) {
        // This function is called very often when a custom layer is set.
        // Avoid heavy calculations.
        //DEBUG_LOG("get_highest_layer %08hX(%u)\n", layer, layer);

        uint8_t layer = get_highest_layer(layer_state);
        for (uint8_t layeri = 0; layeri <= layer; layeri++) {
            if (!layer_state_is(layeri) || layeri == get_highest_layer(default_layer_state)) {
                continue;
            }

            for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
                for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                    uint8_t index = g_led_config.matrix_co[row][col];

                    if (index >= led_min && index < led_max && index != NO_LED && keymap_key_to_keycode(layeri, (keypos_t){col, row}) > KC_TRNS) {
                        HSV hsv = getHSVForLayer(layeri);
                        RGB rgb = hsv_to_rgb(hsv);
                        rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
                    }
                }
            }
        }
    }
    return false;
}

#ifdef CONSOLE_ENABLE
static uint16_t key_timer;
void matrix_scan_user() {
    if (!key_timer) {
        key_timer = timer_read();
    }
    

    if (printDebug && timer_elapsed(key_timer) > 500) {
        printf("        layer_state "); printBinary(layer_state);
        printf("default_layer_state "); printBinary(default_layer_state);

        printf("highest layer_state %2u\n", get_highest_layer(layer_state));
        printf("hst def_layer_state %2u\n", get_highest_layer(default_layer_state));
        printf("----\n");
        key_timer = timer_read();
    }
}
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_tkl_iso(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_SIRI,  RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,            KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD, KC_ROPTN, MO(MAC_FN),KC_RCTL, KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_tkl_iso(
        _______,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______,  RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_tkl_iso(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_CTANA, RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,            KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_NUHS,  KC_ENT,
        KC_LSFT,  KC_NUBS,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RGUI, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_tkl_iso(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  RGB_TOG,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),

    [4] = LAYOUT_tkl_iso(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),
    
    [5] = LAYOUT_tkl_iso(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),
    
    [6] = LAYOUT_tkl_iso(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),
    
    [7] = LAYOUT_tkl_iso(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______),
};
