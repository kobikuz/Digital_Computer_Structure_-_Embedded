import serial as ser
import time
import ctypes

pingPongNum = 10


# in this example we sends a character and ping pong the input character "pingPongNum" times before the port closes
def resetPort():
    # Define constants
    GENERIC_READ = 0x80000000
    GENERIC_WRITE = 0x40000000
    OPEN_EXISTING = 3
    FILE_FLAG_OVERLAPPED = 0x40000000
    PURGE_RXABORT = 0x0002
    PURGE_TXABORT = 0x0008
    PURGE_RXCLEAR = 0x0004
    PURGE_TXCLEAR = 0x0008

    # Create a handle to the port
    port_handle = ctypes.windll.kernel32.CreateFileW(
        r"\\.\COM8",
        GENERIC_READ | GENERIC_WRITE,
        0,
        None,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        None
    )

    # Purge the port's buffer
    ctypes.windll.kernel32.PurgeComm(port_handle, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR)

    # Close the port handle
    ctypes.windll.kernel32.CloseHandle(port_handle)


def printMenu():
    print("LAB 4: \n"
          "1. Blink RGB LED, color by color with delay of X[ms]\n"
          "2. Count up onto LCD screen with delay of X[ms]\n"
          "3. Circular tone series via Buzzer with delay of X[ms]\n"
          "4. Get delay time X[ms]:\n"
          "5. LDR 3-digit value [v] onto LCD\n"
          "6. Clear LCD screen\n"
          "7. Show menu\n"
          "8. Sleep\n"
          "9. Get strings from PC side and print them onto LCD\n")


def sendDelay(delay):
    # sending 4 to mcu

    # calc delay
    delay = int(delay)
    delayBytes = ['0', '0', '0', '0']
    delayBytes[0] = bytes(str(delay // 1000), 'ascii')
    delayBytes[1] = bytes(str((delay // 100) % 10), 'ascii')
    delayBytes[2] = bytes(str((delay // 10) % 10), 'ascii')
    delayBytes[3] = bytes(str(delay % 10), 'ascii')
    return delayBytes

    # send delay

    pass


def main():
    while (1):
        times = 0
        # print("\n\ninit send\n\n")
        # # connection init
        # s = ser.Serial('COM12', baudrate=9600, bytesize=ser.EIGHTBITS,
        #                parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
        #                timeout=1)  # timeout of 1 sec where the read and write operations are blocking,
        #
        # # after the timeout the program continues
        # enableTX = False
        #
        # # clear buffers
        # s.reset_input_buffer()
        # s.reset_output_buffer()

        while (1):

            # connection init
            s = ser.Serial('COM8', baudrate=9600, bytesize=ser.EIGHTBITS,
                           parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                           timeout=1)  # timeout of 1 sec where the read and write operations are blocking,

            # after the timeout the program continues
            enableTX = False

            # clear buffers
            s.reset_input_buffer()
            s.reset_output_buffer()
            printMenu()
            inChar = input("Enter Selection: \n")
            if inChar == '1' or inChar == '2' or inChar == '3' or inChar == '5' or inChar == '6' or inChar == '8':
                bytesChar = bytes(inChar, 'ascii')
                s.write(bytesChar)
                while s.out_waiting > 0:
                    pass
                # clean the Buffers?

            elif inChar == '4':
                delay = input("Enter the wanted delay: \n")
                delaBytes = sendDelay(delay)
                s.write(bytes(inChar, 'ascii'))
                while s.out_waiting > 0:  # in or out waiting?
                    pass
                s.write(delaBytes[0])
                while s.out_waiting > 0:
                    pass
                s.write(delaBytes[1])
                while s.out_waiting > 0:
                    pass
                s.write(delaBytes[2])
                while s.out_waiting > 0:
                    pass
                s.write(delaBytes[3])
                while s.out_waiting > 0:
                    pass
                s.write(bytes('x', 'ascii'))
                while s.out_waiting > 0:
                    pass
                # clean the Buffers?



            elif inChar == '9':
                sentence = input("Enter a wanted message: \n")
                char_sentence = list(sentence)
                s.write(bytes(inChar, 'ascii'))
                while s.out_waiting > 0:
                    pass
                i = 0
                #while i < len(char_sentence):
                s.write(bytes(char_sentence[0], 'ascii'))
                while s.out_waiting > 0:
                    pass
                s.write(bytes(char_sentence[1], 'ascii'))
                while s.out_waiting > 0:
                  pass
                s.write(bytes(char_sentence[2], 'ascii'))
                while s.out_waiting > 0:
                    pass
                   # i += 1



            elif inChar == '7':
                pass

            else:
                print("Invalid selection.\n")
                pass
            s.close()
    # while (1):
    #     while (s.in_waiting > 0):  # while the input buffer isn't empty
    #         char = s.read(size=1)  # read 1 char from the input buffer
    #         print(char.decode("ascii"))
    #         if (s.in_waiting == 0):
    #             enableTX = True  # enable transmission to echo the received data
    #     while (s.out_waiting > 0 or enableTX):
    #         s.write(char)
    #         count = count + 1
    #         if (s.out_waiting == 0):
    #             enableTX = False
    #     if count == pingPongNum + 1:
    #         s.close()
    #         break


if __name__ == '__main__':
    main()
