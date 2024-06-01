#include <string>
#include <stdexcept>

#include "gorinir/embedded/serialport/Hardware.hpp"

namespace gorinir::embedded::serialport {

    int Hardware::mainInit() {
        throw std::runtime_error("Not implemented!");
    }

    void Hardware::delay(uint32_t microSeconds) {
        throw std::runtime_error("Not implemented!");
    }

    void Hardware::setLedState(uint16_t led, bool state) {
        throw std::runtime_error("Not implemented!");
    }

    void Hardware::setUsbTransferFunction(const std::function<void(uint8_t* buffer, uint32_t* length)>& value) {
        throw std::runtime_error("Not implemented!");
    }

    std::function<void(uint8_t* buffer, uint32_t* length)> Hardware::getUsbTransferFunction() {
        throw std::runtime_error("Not implemented!");
    }

    Hardware::~IHardware() noexcept = default;

}
