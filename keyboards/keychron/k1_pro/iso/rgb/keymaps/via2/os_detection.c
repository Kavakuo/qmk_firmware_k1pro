

#include "os_detection.h"
#include "action_layer.h"
#include "rgb_matrix.h"
#include "utils.h"
#include "layers.h"

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
