// Copyright 2020 Rob Riggs <rob@mobilinkd.com>
// All rights reserved.

#include "AFSKModulator.hpp"
#include "AudioLevel.hpp"

namespace mobilinkd { namespace tnc {

void AFSKModulator::init(const kiss::Hardware& hw)
{
    audio::setAudioOutputLevel();
    set_twist(hw.tx_twist);

    // Configure 72MHz clock for 26.4ksps.
    SysClock72();
    __HAL_TIM_SET_AUTORELOAD(&htim7, 2726);
    __HAL_TIM_SET_PRESCALER(&htim7, 0);
    mobilinkd::dacTimerAdjust = dacTimerAdjust; 

    DAC_ChannelConfTypeDef sConfig;

    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
        CxxErrorHandler();
    }

    if (HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048) != HAL_OK)
        CxxErrorHandler();
    if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
        CxxErrorHandler();
}

}} // mobilinkd::tnc
