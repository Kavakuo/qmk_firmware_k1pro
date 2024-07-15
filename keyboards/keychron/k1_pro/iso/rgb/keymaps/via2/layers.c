
#include "layers.h"
#include "action_layer.h"
#include "rgb_matrix.h"
#include "utils.h"



layer_state_t layer_state_set_user(layer_state_t state) {
    return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    DEBUG_LOG("default_layer_state_set_user: %08hX(%u)\n", state, get_highest_layer(state));
    HSV hsv = getHSVForLayer(get_highest_layer(state));
    rgblight_sethsv(hsv.h, hsv.s, hsv.v);
    return state;
}

