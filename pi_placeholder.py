import serial
import time

serdev = 'COM3'
s = serial.Serial(serdev)

while(True):
  for i in range(101):
    st = str(i) + "\n"
    s.write(st.encode("ascii"))
    print(st.encode("ascii"))
    time.sleep(1)