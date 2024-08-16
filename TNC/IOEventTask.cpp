// Copyright 2017-2021 Rob Riggs <rob@mobilinkd.com>
// All rights reserved.

#include "AudioLevel.hpp"
#include "Log.h"
#include "IOEventTask.h"
#include "PortInterface.h"
#include "PortInterface.hpp"
#include "main.h"
#include "AudioInput.hpp"
#include "HdlcFrame.hpp"
#include "Kiss.hpp"
#include "KissHardware.hpp"
#include "ModulatorTask.hpp"
#include "Modulator.hpp"
#include "LEDIndicator.h"

#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

extern osMessageQId hdlcOutputQueueHandle;
extern osThreadId modulatorTaskHandle;
extern osThreadId audioInputTaskHandle;

extern IWDG_HandleTypeDef hiwdg;

static PTT getPttStyle(const mobilinkd::tnc::kiss::Hardware& hardware)
{
    return hardware.options & KISS_OPTION_PTT_SIMPLEX ? PTT::SIMPLEX : PTT::MULTIPLEX;
}

void startIOEventTask(void const*)
{
    using namespace mobilinkd::tnc;

    indicate_on();
    initSerial();
    openSerial();

    print_startup_banner();

    auto& hardware = kiss::settings();
    if (!hardware.load() or !hardware.crc_ok())
    {
        hardware.init();
        hardware.store();
    }

    hardware.debug();

    audio::init_log_volume();
    audio::setAudioOutputLevel();
    audio::setAudioInputLevels();
    setPtt(getPttStyle(hardware));

    osMessagePut(audioInputQueueHandle, mobilinkd::tnc::audio::DEMODULATOR,
        osWaitForever);

    // The modulator task cannot start until after EEPROM settings are
    // loaded or initialized.
    osThreadResume(modulatorTaskHandle);
    osThreadResume(audioInputTaskHandle);

    indicate_waiting_to_connect();

    /* Infinite loop */
    for (;;)
    {
        osEvent evt = osMessageGet(ioEventQueueHandle, 100);
        if (hdlc::ioFramePool().size() != 0 && !getModulator().get_ptt()->state()) {
            // If the IO event loop is inactive or the frame pool is empty for
            // too long, the TNC is essentially non-functional. When  transmitting,
            // the modulator is responsible for updating the watchdog.
            HAL_IWDG_Refresh(&hiwdg); // Refresh IWDG in IO loop (primary refresh).
        }

        if (evt.status != osEventMessage)
            continue;

        uint32_t cmd = evt.value.v;
        if (cmd < FLASH_BASE) // Assumes FLASH_BASE < SRAM_BASE.
        {
            // uint16_t arg = cmd & 0xFFFF;
            // cmd &= 0x07FF0000;
            switch (cmd) {
            case CMD_USER_BUTTON_DOWN:
                INFO("Button Down");
                osMessagePut(audioInputQueueHandle,
                    mobilinkd::tnc::audio::AUTO_ADJUST_INPUT_LEVEL,
                    osWaitForever);
                osMessagePut(audioInputQueueHandle,
                    mobilinkd::tnc::audio::DEMODULATOR, osWaitForever);
                break;
            case CMD_USER_BUTTON_UP:
                TNC_DEBUG("Button Up");
                break;
            case CMD_SET_PTT_SIMPLEX:
                getModulator().set_ptt(&simplexPtt);
                break;
            case CMD_SET_PTT_MULTIPLEX:
                getModulator().set_ptt(&multiplexPtt);
                break;
            case CMD_RESTORE_SYSCLK:
                // Not appicable to NucleoTNC
                break;
            default:
                WARN("unknown command = %04x", static_cast<unsigned int>(cmd));
                break;
            }
            continue;
        }

        using hdlc::IoFrame;

        auto frame = static_cast<IoFrame*>(evt.value.p);

        if (frame->source() & IoFrame::RF_DATA)
        {
            TNC_DEBUG("RF frame");
            frame->source(frame->source() & 0x70);
            if (!ioport->write(frame, frame->size() + 100))
            {
                ERROR("Timed out sending frame");
                // The frame has been passed to the write() call.  It owns it now.
                // Do not release here.
            }
        }
        else
        {
            TNC_DEBUG("Serial frame");
            if ((frame->type() & 0x0F) == IoFrame::DATA)
            {
                kiss::getAFSKTestTone().stop();
                if (osMessagePut(hdlcOutputQueueHandle,
                    reinterpret_cast<uint32_t>(frame),
                    osWaitForever) != osOK)
                {
                    ERROR("Failed to write frame to TX queue");
                    hdlc::release(frame);
                }
            }
            else
            {
                kiss::handle_frame(frame->type(), frame);
            }
        }
    }
}

namespace mobilinkd {
namespace tnc {

void print_startup_banner()
{
#ifdef KISS_LOGGING
    uint32_t* uid = (uint32_t*) UID_BASE;  // STM32L4xx (same for 476 and 432)

    INFO("%s version %s", mobilinkd::tnc::kiss::HARDWARE_VERSION,
        mobilinkd::tnc::kiss::FIRMWARE_VERSION);
    INFO("CPU core clock: %luHz", SystemCoreClock);
    INFO("    Device UID: %08lX %08lX %08lX", uid[0], uid[1], uid[2]);

    uint8_t* version_ptr = (uint8_t*) 0x1FFF6FF2;

    int version = *version_ptr;

    INFO("Bootloader version: 0x%02X", version);
#endif
}

}
} // mobilinkd::tnc
