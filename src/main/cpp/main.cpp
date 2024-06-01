#include "gorinir/embedded/serialport/Application.hpp"

int main() {
    return gorinir::embedded::serialport::Application::getInstance().run();
}
