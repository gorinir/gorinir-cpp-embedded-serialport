# gorinir-cpp-embedded-serialport

- Uses board: NUCLEO-H7A3ZI-Q with MCU: STM32H7A3ZI
- Requires: `cmake`, `ninja`, `STM32CubeMX`, `openocd`

##### How To Generate Vendor Developed Code

```
cmake -DUSE_CUBE=ON --preset windows.ninja.gcc-arm.debug.static
```

##### How To Build

```
cmake --preset windows.ninja.gcc-arm.debug.static
cmake --build --preset windows.ninja.gcc-arm.debug.static --target clean
cmake --build --preset windows.ninja.gcc-arm.debug.static --target gorinir-cpp-embedded-serialport-app
```

##### How To Upload

```
cmake --preset windows.ninja.gcc-arm.debug.static
cmake --build --preset windows.ninja.gcc-arm.debug.static --target clean
cmake --build --preset windows.ninja.gcc-arm.debug.static --target gorinir-cpp-embedded-serialport-app
cmake --build --preset windows.ninja.gcc-arm.debug.static --target cmake-flash-write
```

##### Tests with PyTest

Device VID: `0x0483 (1155)`
Device PID: `0x5470 (22336)`
File: `src/main/c/exqudens/embedded/serial/USB_DEVICE/App/usbd_desc.c`

```
py -m venv build/py-env
./build/py-env/Scripts/pip install -r src/test/resources/requirements.txt
./build/py-env/Scripts/python -m serial.tools.list_ports
./build/py-env/Scripts/pytest --collect-only
./build/py-env/Scripts/pytest --com-port=COM1 src/test/py/test_serial.py::TestSerial::test_1
```
