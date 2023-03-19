import serial
import time
# import serial.tools.list_ports


serialPort = serial.Serial(port='/dev/cu.DSDTECHHC-05', baudrate=9600, timeout=1, stopbits=1)


# ports = serial.tools.list_ports.comports()

# for port, desc, hwid in sorted(ports):
#         print("{}: {} [{}]".format(port, desc, hwid))


while 1:
    data = serialPort.readline().decode('utf-8')
    try:
        measurement = int(data)
        print(measurement)
    except:
        pass
    # print(data)
    # time.sleep(1)
    
