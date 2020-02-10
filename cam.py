import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 38400, timeout=0.2)  # open serial port

# define our commands
RESET_COMMAND = bytearray.fromhex("56 00 26 00")
DEFAULT_COMPRESSION_COMMAND = bytearray.fromhex("56 00 31 05 01 01 12 04 36")
RES_640x480_COMMAND = bytearray.fromhex("56 00 31 05 04 01 00 19 00")
RES_320x240_COMMAND = bytearray.fromhex("56 00 31 05 04 01 00 19 11")
CAPTURE_IMAGE_COMMAND = bytearray.fromhex("56 00 36 01 00")
READ_DATALEN_COMMAND = bytearray.fromhex("56 00 34 01 00")

# 56 00 32 0C 00 0D 00 00 XX XX 00 00 YY YY 00 0A
READ_DATA_COMMAND = bytearray.fromhex("56 00 32 0C 00 0A 00 00 00 00 00 00 FF FF 10 00")

STOP_CAPTURE_COMMAND = bytearray.fromhex("56 00 36 01 03")

SET_115200_BAUD_COMMAND = bytearray.fromhex("56 00 24 03 01 0D A6")




# write a reset to the camera
ser.write(RESET_COMMAND)
recv = ser.read(1000)
print("received after reset:")
print(''.join('{:02x} '.format(x) for x in recv))


# set 115200 baud rate 
ser.write(SET_115200_BAUD_COMMAND)
recv = ser.read(1000)
print("received after set baud:")
print(''.join('{:02x} '.format(x) for x in recv))

# close the old serial port and open a new one at the new speed
ser.close()
ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1.0)  # open serial port

# set 640x480 resolution
ser.write(RES_320x240_COMMAND)
recv = ser.read(1000)
print("received after set res:")
print(''.join('{:02x} '.format(x) for x in recv))


# set default compression
specific_compression_command = DEFAULT_COMPRESSION_COMMAND
specific_compression_command[-1] = 150
ser.write(DEFAULT_COMPRESSION_COMMAND)
recv = ser.read(1000)
print("received after set compression:")
print(''.join('{:02x} '.format(x) for x in recv))




frameNum = 0

# read forever
while 1:
    # set default compression
    ser.write(CAPTURE_IMAGE_COMMAND)
    recv = ser.read(1000)

    # set get length of JPEG data
    ser.write(READ_DATALEN_COMMAND)
    recv = ser.read(1000)

    # set to read correct amount of data
    try:
        high_len = recv[-2]
        low_len = recv[-1]
    except:
        print("didnt receive data")

    specific_read_command = READ_DATA_COMMAND
    specific_read_command[-4] = high_len
    specific_read_command[-3] = low_len

    # read all the data, starting at address 0x0000
    ser.write(specific_read_command)
    time.sleep(0.2)
    recvBuf = []
    # loop until all data is read - note these timings may have to be changed
    while 1:
        time.sleep(0.1)
        if ser.inWaiting():
            for d in ser.read(1000):
                recvBuf.append(d)
        else:
            break

    
    print("received image, size: ", len(recvBuf)-10)

    # write image to a file
    with open("test_" + str(frameNum) + ".jpg", 'wb') as f:
        f.write(bytearray(recvBuf[5:-5]))
        frameNum += 1
    
    # stop the capture
    ser.write(STOP_CAPTURE_COMMAND)

    # clear the serial buffer
    recv = ser.read(100)
