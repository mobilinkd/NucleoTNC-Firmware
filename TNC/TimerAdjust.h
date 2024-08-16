// Copyright 2024 Mobilinkd LLC.

#pragma once

#include "stm32l4xx_hal.h"

#include <cstdint>
#include <cstdlib>

namespace mobilinkd {

/**
 * Given the Period and Count, along with the actual clock's error within
 * that period, adjust the timer period as needed to best match the nominal
 * clock.
 * 
 * @note This will only adjust the period up or down by one. This limits
 * the pull range of this adjustment.
 * 
 * @example for a nominal clock frequency of 48MHz and an actual clock
 * frequency of 48005120 (MSI PLL @ 48MHz), a timer period of 1000 and
 * a timer count of 192 (e.g. 192 samples), we round the ActualClock
 * for this period * count to 192016 (within 250ppb), giving an error
 * of 16. The timer period will be intermittently increased to 1001 to
 * keep the timer accurate to the expected clock, increasing jitter to
 * do so.
 */
template <uint32_t Period, uint32_t Count, int32_t Error>
struct TimerAdjust
{
    enum Adjustment {None, Up, Down};
    static constexpr int32_t ActualClock = Period * Count + Error;

    TIM_HandleTypeDef* htim_;
    int32_t ticks_ = 0;
    uint32_t current_period_ = Period;
    Adjustment adjustment_ = Adjustment::None;
    

    TimerAdjust(TIM_HandleTypeDef* htim)
    : htim_(htim)
    {
        if (Error > 0) {
            adjustment_ = Adjustment::Up;
        } else if (Error < 0) {
            adjustment_ = Adjustment::Down;
        }
    }

    void reset()
    {
        ticks_ = 0;
        current_period_ = Period;
        __HAL_TIM_SET_AUTORELOAD(htim_, current_period_ - 1);
    }

    void operator()()
    {
        ticks_ += Count * current_period_;
        switch (adjustment_) {
        case Adjustment::Up:
            if (ticks_ < ActualClock) {
                current_period_ = Period + 1;
            } else {
                current_period_ = Period;
            }
            __HAL_TIM_SET_AUTORELOAD(htim_, current_period_ - 1);
            break;
        case Adjustment::Down:
            if (ticks_ > ActualClock) {
                current_period_ = Period - 1;
            } else {
                current_period_ = Period;
            }
            __HAL_TIM_SET_AUTORELOAD(htim_, current_period_ - 1);
            break;
        case Adjustment::None:
            break;
        }
        ticks_ -= ActualClock;
    }
};

} // mobilinkd
