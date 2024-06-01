#include <string>
#include <stdexcept>

#include "gorinir/embedded/serialport/Hardware.hpp"
#include "gorinir/embedded/serialport/Application.hpp"
#include "main.h"
#include "gorinir_embedded_serialport.h"
#include "usbd_cdc_if.h"

extern "C" void gorinir_embedded_serialport_usb_transfer(uint8_t* buffer, uint32_t* length) {
    if (gorinir::embedded::serialport::Application::getInstance().getHal()) {
        std::function<void(uint8_t*, uint32_t*)> usbTransferFunction = {};
        usbTransferFunction = gorinir::embedded::serialport::Application::getInstance().getHal()->getUsbTransferFunction();
        if (usbTransferFunction) {
            usbTransferFunction(buffer, length);
        }
    }
}

namespace gorinir::embedded::serialport {

    int Hardware::mainInit() {
        if (!usbTransferFunction) {
            usbTransferFunction = [](uint8_t* buffer, uint32_t* length) {
                uint32_t size = *length;
                for (uint32_t i = 0; i < size; i++) {
                    if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                        buffer[i] -= ('a' - 'A');
                    }
                }
                CDC_Transmit_HS(buffer, size);
            };
        }
        return HAL_Main_Init();
    }

    void Hardware::delay(uint32_t microSeconds) {
        HAL_Delay(microSeconds);
    }

    void Hardware::setLedState(uint16_t led, bool state) {
        if (led == 1u) {
            if (state) {
                HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
            }
        } else {
            throw std::runtime_error("Unsupported led: " + std::to_string(led));
        }
    }

    void Hardware::setUsbTransferFunction(const std::function<void(uint8_t* buffer, uint32_t* length)>& value) {
        usbTransferFunction = value;
    }

    std::function<void(uint8_t* buffer, uint32_t* length)> Hardware::getUsbTransferFunction() {
        return usbTransferFunction;
    }

    Hardware::~Hardware() noexcept = default;

}
