import logging
import pytest
from serial import Serial


@pytest.fixture(scope='session')
def com_port(pytestconfig):
    return pytestconfig.getoption('--com-port')


class TestSerialport:
    """
    TestSerialport class.
    """
    __logger = logging.getLogger('.'.join([__module__, __qualname__]))

    def test_1(self, com_port):
        try:
            self.__logger.info(f"com_port: '{com_port}'")

            assert com_port is not None, 'use "python" cmd option "--com-port" to specify com port (example: pytest --com-port=COM1)'

            device = Serial(com_port)
            self.__logger.info(f"device initialized: {device is not None}")

            assert device is not None

            data = device.read_all().decode()
            length = len(data)
            self.__logger.info(f"received data length: {length}")

            assert length == 0

            data = 'hello world'
            length = device.write(data.encode())
            self.__logger.info(f"sent data: '{data}'")
            self.__logger.info(f"sent data length: {length}")

            assert length == 11

            data = device.read_all().decode()
            self.__logger.info(f"received: '{data}'")

            assert data == 'HELLO WORLD'

            data = device.read_all().decode()
            length = len(data)
            self.__logger.info(f"received data length: {length}")

            assert length == 0

            device.close()
        except Exception as e:
            self.__logging.error(e, exc_info=True)
            raise e
