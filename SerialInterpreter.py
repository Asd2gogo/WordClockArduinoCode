import serial
import contextlib
import grap

ser = serial.Serial()
ser.baudrate= 115200
ser.port = 'COM3'
ser.open()

while 1:
    line = ser.readline()
    if len(line) != 0:
        print(line)