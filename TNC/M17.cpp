// Copyright 2020 Rob Riggs <rob@mobilinkd.com>
// All rights reserved.

#include "M17.h"

namespace mobilinkd { namespace m17 {

const std::array<int16_t, FILTER_TAP_NUM> rrc_taps = {
    -163, -130, -62, 29, 129, 215, 267, 269, 216, 114, -19, -158, -269, -321,
    -293, -176, 16, 253, 485, 654, 698, 573, 253, -249, -886, -1569, -2176,
    -2569, -2606, -2166, -1163, 427, 2569, 5155, 8014, 10930, 13661, 15966,
    17634, 18510, 18510, 17634, 15966, 13661, 10930, 8014, 5155, 2569, 427,
    -1163, -2166, -2606, -2569, -2176, -1569, -886, -249, 253, 573, 698, 654,
    485, 253, 16, -176, -293, -321, -269, -158, -19, 114, 216, 269, 267, 215,
    129, 29, -62, -130, -163};


}} // mobilinkd::m17