# coding=utf-8
import sys

from lifxlan import LifxLAN

def main():
    num_lights = None
    if len(sys.argv) != 2:
        print("\nDiscovery will go much faster if you provide the number of lights on your LAN:")
        print("  python {} <number of lights on LAN>\n".format(sys.argv[0]))
    else:
        num_lights = int(sys.argv[1])

    # instantiate LifxLAN client, num_lights may be None (unknown).
    # In fact, you don't need to provide LifxLAN with the number of bulbs at all.
    # lifx = LifxLAN() works just as well. Knowing the number of bulbs in advance 
    # simply makes initial bulb discovery faster.
    print("Discovering lights...")
    lifx = LifxLAN(num_lights, verbose=True)

    # get devices
    devices = lifx.get_lights()
    bulb = devices[0]

    while True:
        # polling to check for bluetooth signal

        # brightness in range [0-65535], duration in ms, rapid = 1 ->
        bulb.set_brightness() # (brightness, [duration], [rapid])
        bulb.set_colortemp(kelvin, [duration], [rapid]) # kelvin in range [2500-9000]
    labels = []
    for device in devices:
        labels.append(device.get_label())
    print("Found Bulbs:")
    for label in labels:
        print("  " + str(label))

if __name__ == "__main__":
    main()