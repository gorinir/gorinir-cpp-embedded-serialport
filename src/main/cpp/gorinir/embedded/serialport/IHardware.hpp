#pragma once

#include <cstdint>
#include <functional>

namespace gorinir::embedded::serialport {

    class IHardware {

        public:

            virtual int mainInit() = 0;

            virtual void delay(uint32_t microSeconds) = 0;

            virtual void setLedState(uint16_t led, bool state) = 0;

            virtual void setUsbTransferFunction(const std::function<void(uint8_t* buffer, uint32_t* length)>& value) = 0;
            virtual std::function<void(uint8_t* buffer, uint32_t* length)> getUsbTransferFunction() = 0;

            virtual ~IHardware() noexcept = default;

    };

}
