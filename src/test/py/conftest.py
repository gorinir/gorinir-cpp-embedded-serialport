def pytest_addoption(parser):
    parser.addoption('--com-port', action='store', default=None, help='str value for com port (example: COM1)')
