# coding=utf-8
import sys
from lifxlan import LifxLAN
import serial

def main():
    num_lights = None
    # instantiate LifxLAN client, num_lights may be None (unknown).
    # In fact, you don't need to provide LifxLAN with the number of bulbs at all.
    # lifx = LifxLAN() works just as well. Knowing the number of bulbs in advance 
    # simply makes initial bulb discovery faster.
    print("Discovering lights...")
    lifx = LifxLAN(num_lights, verbose=True)

    # get devices
    devices = lifx.get_lights()
    bulb = devices[0]

    # bluetooth
    bt = serial.Serial(port='/dev/cu.DSDTECHHC-05', baudrate=9600, timeout=1, stopbits=1)

    while True:
        # polling to check for bluetooth signal
        data = bt.readline().decode('utf-8')
        try:
            measurement = int(data)
            if measurement in range(0,3):
                # low -> 1-2 inches
                bulb.set_brightness(1000, 500)
            elif measurement in range(3, 6):
                # low-medium -> 3-5 inches
                bulb.set_brightness(10000, 500)
            elif measurement in range(6, 9):
                # high-medium -> 6-8 inches
                bulb.set_brightness(30000, 500)
            elif measurement in range(9, 11):
                # high -> 9-10 inches
                bulb.set_brightness(50000, 500)
        except:
            pass
        # command = input("type 'b' to change brightness, 'k' to change temperature: ")
        # if (command == 'b'):
        #     brightness = input("enter brightness [0-65535]: ")
        #     if (int(brightness) < 0) or (int(brightness) > 65535):
        #         print("outside of range, try again")
        #     bulb.set_brightness(brightness, 500)
        # elif (command == 'k'):
        #     kelvin = input("enter temperature [2500-9000]: ")
        #     if (int(kelvin) < 2500) or (int(kelvin) > 9000):
        #         print("outside of range, try again")
        #     bulb.set_colortemp(kelvin, 500)
        # else:
        #     print("invalid command, try again")

        # brightness in range [0-65535], duration in ms, rapid = 1 ->
        # bulb.set_brightness() # (brightness, [duration], [rapid])
        # bulb.set_colortemp(kelvin, [duration], [rapid]) # kelvin in range [2500-9000]
    labels = []
    for device in devices:
        labels.append(device.get_label())
    print("Found Bulbs:")
    for label in labels:
        print("  " + str(label))
if __name__ == "__main__":
    main()