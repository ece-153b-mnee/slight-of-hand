# sLight of Hand

## Installing dependencies

Our lightbulb uses the [LIFX LAN Protocol](https://lan.developer.lifx.com/). We use the **lifxlan** Python 3 module to locally control our light bulb over LAN.

To install the module:

`pip install lifxlan`

The **lifxlan** utilizes a feature in *bitstring* that is only available in older versions (however the setup.py of the module uses the latest version). To fix this, uninstall the most recent version of bitstrings

`pip3 uninstall bitstrings`

then install version 3.1.9 of bitstrings

`pip3 install bitstring==3.1.9`