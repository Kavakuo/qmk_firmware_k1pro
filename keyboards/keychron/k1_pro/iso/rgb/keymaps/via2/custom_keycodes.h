

#pragma once

#include "k1_pro.h"

extern bool printDebug;

enum customKeys {
    // Momentarily turn off highest layer
    CKC_MOF_HL = NEW_SAFE_RANGE,
    CKC_MOF_ALL,
    CKC_TF_ALL,
    CKC_MOFN,
    CKC_HARD_RESET,
    CKC_SOFT_RESET,
    CKC_DEBUG
};
