
#include "process_key_override.h"
#include "quantum_keycodes.h"


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
