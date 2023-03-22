# coding=utf-8
# testlight.py
# Tests brightness and color temperature functionality of LIFX bulb
from lifxlan import LifxLAN
import serial

def main():
    num_lights = None
    # instantiate LifxLAN client
    print("Discovering lights...")
    lifx = LifxLAN(verbose=True)

    # get devices
    devices = lifx.get_lights()
    bulb = devices[0]

    # bluetooth
    bt = serial.Serial(port='/dev/cu.DSDTECHHC-05', baudrate=9600, timeout=1, stopbits=1)

        
    while True:
        command = input("type 'b' to change brightness, 'k' to change temperature: ")
        if (command == 'b'):
            brightness = input("enter brightness [0-65535]: ")
            if (int(brightness) < 0) or (int(brightness) > 65535):
                print("outside of range, try again")
            bulb.set_brightness(brightness, 500)
        elif (command == 'k'):
            kelvin = input("enter temperature [2500-9000]: ")
            if (int(kelvin) < 2500) or (int(kelvin) > 9000):
                print("outside of range, try again")
            bulb.set_colortemp(kelvin, 500)
        else:
            print("invalid command, try again")

        # brightness in range [0-65535], duration in ms, rapid = 1 ->
        # bulb.set_brightness() # (brightness, [duration], [rapid])
        # bulb.set_colortemp(kelvin, [duration], [rapid]) # kelvin in range [2500-9000]

if __name__ == "__main__":
    main()