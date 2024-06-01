#include <cstdlib>
#include <exception>

#include "gorinir/embedded/serialport/Application.hpp"
#include "gorinir/embedded/serialport/Hardware.hpp"

namespace gorinir::embedded::serialport {

    Application& Application::getInstance() {
        static Application instance;
        return instance;
    }

    Application& Application::setRunning(bool value) {
        running = value;
        return *this;
    }

    Application& Application::setHal(const std::shared_ptr<IHardware> value) {
        hal = value;
        return *this;
    }

    std::shared_ptr<IHardware> Application::getHal() {
        return hal;
    }

    int Application::run() {
        try {
            bool halDeleteRequired = false;

            if (hal == nullptr) {
                hal = std::shared_ptr<Hardware>(new Hardware());
                halDeleteRequired = true;
            }

            int result = hal->mainInit();

            if (result != EXIT_SUCCESS) {
                if (halDeleteRequired) {
                    hal.reset();
                    hal = nullptr;
                }
                return result;
            }

            unsigned int shortMax = 4;
            unsigned int shortIndex = 0;
            do {
                if (shortIndex + 1 == shortMax) {
                    hal->delay(3000u);
                } else {
                    hal->setLedState(1, true);
                    hal->delay(500u);
                    hal->setLedState(1, false);
                    hal->delay(500u);
                }
                shortIndex = (shortIndex + 1) % shortMax;
            } while (running);

            if (halDeleteRequired) {
                hal.reset();
                hal = nullptr;
            }

            return result;
        } catch (const std::exception& e) {
            return EXIT_FAILURE;
        } catch (...) {
            return EXIT_FAILURE;
        }
    }

    Application::Application() = default;

    Application::~Application() = default;

}
