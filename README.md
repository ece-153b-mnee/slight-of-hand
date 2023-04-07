# [sLight of Hand](https://sites.google.com/view/slight-of-hand/)

sLight of Hand is a smart desk lamp project that utilizes the STM32L476RG microcontroller. We leverage the board's Real-Time-Clock (RTC) in order to adjust the color temperature (K) of our LIFX light bulb. The user can also easily adjust the brightness of the lamp by intuitively hovering their hand over the HC-SR04 Ultrasonic Sensor, where the brightness can be lowered by lowering your hand -- and increasing brightness by rising your hand. An ILI9341 SPI LCD shows the current time and brightness level of the light.

## Installing dependencies

Our lightbulb uses the [LIFX LAN Protocol](https://lan.developer.lifx.com/). We use the [**lifxlan**](https://github.com/mclarkk/lifxlan) Python 3 module to locally control our light bulb over LAN. We also use the [pySerial](https://pyserial.readthedocs.io/en/latest/) library to receive Bluetooth communication from the device.

To install the modules:

`pip install lifxlan`

`pip install pyserial`

The **lifxlan** utilizes a feature in *bitstring* that is only available in older versions (however the setup.py of the module uses the latest version). To fix this, uninstall the most recent version of bitstring.

`pip3 uninstall bitstring`

then install version 3.1.9 of bitstring

`pip3 install bitstring==3.1.9`
