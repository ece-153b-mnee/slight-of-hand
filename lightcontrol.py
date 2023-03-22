# coding=utf-8
# lightcontrol.py
# Python control script for sLight of Hand
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

    # color temp initialization
    # color temp ranges from [2500, 9000] Kelvin
    #   sunrise -> sunset = 12 hours -> ~541K change per hour
    timeSet = False
    decrementTemp = False
    temp = 5000
    while not timeSet:
        # get initial time for temperature
        hourData = bt.readline().decode('utf-8')
        print(hourData)
        try:
            hour = int(hourData)
            if hour in range(7, 19):
                # day range -> increment (7am-7pm)
                temp = 9000 - ((hour - 7) * 541)
                decrementTemp = True
            else:
                # night range -> decrement
                if (hour in range(19, 25)):
                    temp = 2500 + ((hour - 19) * 541)
                elif (hour in range(0, 7)):
                    # 5 hours past 
                    temp = 2500 + ((5 + hour) * 541)
                decrementTemp = True

            bulb.set_colortemp(temp, 500)
            timeSet = True
        except:
            pass
        
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
            if "t" in data:
                # timer alarm -> change color temperature -> 9K every minute
                if decrementTemp:
                    temp -= 9
                else:
                    temp += 9
                if temp > 9000:
                    temp = 9000
                    decrementTemp = True
                elif temp < 2500:
                    temp = 2500
                    decrementTemp = False

                bulb.set_colortemp(temp, 500)

if __name__ == "__main__":
    main()