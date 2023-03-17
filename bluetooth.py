import serial

serialPort = serial.Serial(port='/dev/cu.DSDTECHHC-05', baudrate=9600, timeout=1, stopbits=1)

while 1:
    data = serialPort.readline().decode('utf-8')
    try:
        measurement = int(data)
        print(measurement)
    except:
        print("")
    # print(data)
    
