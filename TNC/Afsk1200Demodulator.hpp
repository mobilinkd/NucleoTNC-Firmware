// Copyright 2020-2021 Rob Riggs <rob@mobilinkd.com>
// All rights reserved.

#pragma once

#include "Demodulator.hpp"
#include "AfskDemodulator.hpp"
#include "FirFilter.hpp"
#include "FilterCoefficients.hpp"
#include "KissHardware.hpp"
#include "HdlcFrame.hpp"
#include "TimerAdjust.h"

namespace mobilinkd { namespace tnc {

struct Afsk1200Demodulator : IDemodulator
{
    /*
     * Generated with Scipy Filter, 152 coefficients, 1100-2300Hz bandpass,
     * Hann window, starting and ending 0 value coefficients removed.
     *
     * np.array(
     *  firwin2(152,
     *      [
     *          0.0,
     *          1000.0/(sample_rate/2),
     *          1100.0/(sample_rate/2),
     *          2350.0/(sample_rate/2),
     *          2500.0/(sample_rate/2),
     *          1.0
     *      ],
     *      [0,0,1,1,0,0],
     *      antisymmetric = False,
     *      window='hann') * 32768,
     *  dtype=int)[10:-10]
     */
    static constexpr size_t FILTER_TAP_NUM = 132;
    static constexpr uint32_t ADC_BLOCK_SIZE = afsk1200::ADC_BUFFER_SIZE;
    static_assert(audio::ADC_BUFFER_SIZE >= ADC_BLOCK_SIZE);

    static constexpr uint32_t SAMPLE_RATE = 26400;
    static constexpr uint16_t VREF = 16383;

    using audio_filter_t = Q15FirFilter<ADC_BLOCK_SIZE, FILTER_TAP_NUM>;

    static const q15_t bpf_coeffs[FILTER_TAP_NUM];

    static afsk1200::emphasis_filter_type filter_1;
    static afsk1200::emphasis_filter_type filter_2;
    static afsk1200::emphasis_filter_type filter_3;

    static afsk1200::Demodulator demod1;
    static afsk1200::Demodulator demod2;
    static afsk1200::Demodulator demod3;

    audio_filter_t demod_filter;
    uint16_t last_fcs{0};
    uint32_t last_counter{0};
    uint32_t counter{0};
    bool locked_{false};
    TimerAdjust<2727, 26400, 12320> adcTimerAdjust{&htim6};

    virtual ~Afsk1200Demodulator() {}
    size_t get_adc_exponent() const override { return 2; }

    void start() override
    {
        SysClock72();

        // rx_twist is 6dB for discriminator input and 0db for de-emphasized input.
        auto twist = kiss::settings().rx_twist;

        filter_1.init(filter::fir::AfskFilters[twist + 3]->taps);
        filter_2.init(filter::fir::AfskFilters[twist + 6]->taps);
        filter_3.init(filter::fir::AfskFilters[twist + 9]->taps);

        last_fcs = 0;
        last_counter = 0;
        counter = 0;

        demod_filter.init(bpf_coeffs);
        passall(kiss::settings().options & KISS_OPTION_PASSALL);

        ADC_ChannelConfTypeDef sConfig;

        sConfig.Channel = AUDIO_IN;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SingleDiff = ADC_SINGLE_ENDED;
        sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
        sConfig.OffsetNumber = ADC_OFFSET_NONE;
        sConfig.Offset = 0;
        if (HAL_ADC_ConfigChannel(&DEMODULATOR_ADC_HANDLE, &sConfig) != HAL_OK)
            CxxErrorHandler();
        mobilinkd::adcTimerAdjust = adcTimerAdjust;
        startADC(2727, ADC_BLOCK_SIZE);
    }

    void stop() override
    {
        stopADC();
        locked_ = false;
    }

    hdlc::IoFrame* operator()(const q15_t* samples) override;

    float readTwist() override;

    uint32_t readBatteryLevel() override;

    bool locked() const override
    {
        return locked_;
    }

    size_t size() const override
    {
        return ADC_BLOCK_SIZE;
    }

    void passall(bool enabled) override
    {
        demod1.hdlc_decoder_.setPassall(enabled);
        demod2.hdlc_decoder_.setPassall(enabled);
        demod3.hdlc_decoder_.setPassall(enabled);
    }
};

}} // mobilinkd::tnc
