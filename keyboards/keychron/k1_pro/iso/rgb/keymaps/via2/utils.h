#pragma once

#include "color.h"

#ifdef CONSOLE_ENABLE
#include "print.h"
#    define DEBUG_LOG(message, ...) uprintf(message, ##__VA_ARGS__)
#else
#    define DEBUG_LOG(message, ...) ((void)0)
#endif


HSV getHSVForLayer(uint8_t layerId);
void printBinary(uint16_t m);
