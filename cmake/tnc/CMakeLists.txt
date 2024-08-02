cmake_minimum_required(VERSION 3.22)

project(tnc)
add_library(tnc)

# Enable CMake support for ASM and C languages
enable_language(CXX ASM)

target_compile_definitions(tnc PUBLIC 
    USE_HAL_DRIVER 
    STM32L4P5xx
    ARM_MATH_CM4
    BOOST_DISABLE_ASSERTS
    NUCLEOTNC
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:KISS_LOGGING>
)

target_include_directories(tnc PUBLIC
    ../../Inc
    ../../Drivers/STM32L4xx_HAL_Driver/Inc
    ../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
    ../../Middlewares/Third_Party/FreeRTOS/Source/include
    ../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    ../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ../../Drivers/CMSIS/Device/ST/STM32L4xx/Include
    ../../Drivers/CMSIS/Include
    ../../Middlewares/ST/ARM/DSP/Inc
    ../../TNC
    ../../blaze
)

target_sources(tnc PRIVATE
    ../../TNC/Afsk1200Demodulator.cpp
    ../../TNC/AfskDemodulator.cpp
    ../../TNC/AFSKModulator.cpp
    ../../TNC/AFSKTestTone.cpp
    ../../TNC/AudioInput.cpp
    ../../TNC/AudioLevel.cpp
    ../../TNC/Correlator.cpp
    ../../TNC/DCD.cpp
    ../../TNC/Demodulator.cpp
    ../../TNC/Digipeater.cpp
    ../../TNC/FilterCoefficients.cpp
    ../../TNC/FirFilter.cpp
    ../../TNC/Fsk9600Demodulator.cpp
    ../../TNC/Fsk9600Modulator.cpp
    ../../TNC/Goertzel.cpp
    ../../TNC/Golay24.cpp
    ../../TNC/HdlcDecoder.cpp
    ../../TNC/HdlcFrame.cpp
    ../../TNC/IOEventTask.cpp
    ../../TNC/Kiss.cpp
    ../../TNC/KissHardware.cpp
    ../../TNC/KissTask.cpp
    ../../TNC/LEDIndicator.cpp
    ../../TNC/Log.cpp
    ../../TNC/M17.cpp
    ../../TNC/M17Demodulator.cpp
    ../../TNC/M17Encoder.cpp
    ../../TNC/M17Modulator.cpp
    ../../TNC/ModulatorTask.cpp
    ../../TNC/NullPort.cpp
    ../../TNC/PortInterface.cpp
    ../../TNC/SerialPort.cpp
)

target_link_directories(tnc PUBLIC
    ../../Middlewares/ST/ARM/DSP/Lib
)

target_link_libraries(tnc PRIVATE
    arm_cortexM4lf_math
)

target_compile_options(tnc PRIVATE -fpermissive -fdata-sections -ffunction-sections -fpermissive -fsigned-char -fsingle-precision-constant -fno-inline-functions -ffast-math -fstack-usage --param=large-stack-frame=128 --param=large-stack-frame-growth=50 -fstrict-aliasing --specs=nano.specs -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit --specs=nosys.specs)

# Validate that TNC code is compatible with C++ standard
if(CMAKE_CXX_STANDARD LESS 20)
    message(ERROR "Generated code requires C++20 or higher")
endif()


