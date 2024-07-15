
#include "keycodes.h"
#include "utils.h"
#include "layers.h"

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
        case CKC_TF_ALL:
            if (record->event.pressed) {
                DEBUG_LOG("CKC_TF_ALL pressed\n");
                layer_clear();
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
