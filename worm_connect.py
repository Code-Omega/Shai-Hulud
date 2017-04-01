# import serial
# import time
# # # Serial port parameters
# serial_speed = 9600
# serial_port = '/dev/tty.HC-06-DevB' # bluetooth shield hc-06
# if __name__ == '__main__':
# print "conecting to serial port ..."
# 	ser = serial.Serial(serial_port, serial_speed, timeout=1)
# print "sending message to turn on PIN 13 ..."
# 	ser.write('1')
# print "recieving message from arduino ..."
# 	data = ser.readline()
# if (data != ""):
# print "arduino says: %s" % data
# else:
# print "arduino doesnt respond"
# 	time.sleep(4)
# print "finish program and close connection!"
# #*********************************************************#

# import serial
# import time 

#initialization 
serial_speed = 9600 # adjust later
serial_port = '/dev/tty.HC-06-DevB' # should have this when connect to hc06

if __name__ == '__main__':
		ser = serial.Serial(serial_port, serial_speed, timeout=1) # start connection
		ser.write('1')
		while(1):
			data = ser.readline()
			if (data != ""):
				print "arduino says: %s" % data









