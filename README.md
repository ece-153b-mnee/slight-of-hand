# sLight of Hand

## Installing dependencies

Our lightbulb uses the [LIFX LAN Protocol](https://lan.developer.lifx.com/). We use the [**lifxlan**](https://github.com/mclarkk/lifxlan) Python 3 module to locally control our light bulb over LAN. We also use the [pySerial](https://pyserial.readthedocs.io/en/latest/) library to receive Bluetooth communication from the device.

To install the modules:

`pip install lifxlan`

`pip install pyserial`

The **lifxlan** utilizes a feature in *bitstring* that is only available in older versions (however the setup.py of the module uses the latest version). To fix this, uninstall the most recent version of bitstring.

`pip3 uninstall bitstring`

then install version 3.1.9 of bitstring

`pip3 install bitstring==3.1.9`
