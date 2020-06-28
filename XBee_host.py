import serial
import time

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

while(1):
    s.write("/myled1/write 1\r".encode())
    print("1st")
    #line = s.readline()
    #print(line)
    #print("1st")
    time.sleep(1)
    s.write("/myled1/write 0\r".encode())
    print("2nd")
    #line = s.readline()
    #print(line)
    #print("2nd")
    time.sleep(1)
s.close()