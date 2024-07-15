

#include "color.h"
#include "layers.h"
#include "utils.h"

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

void printBinary(uint16_t m) {
    DEBUG_LOG(BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
}
