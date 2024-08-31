import tkinter as tk
from PIL import Image, ImageTk
import threading
import serial as ser
import time
import ctypes
from tkinter import filedialog


pingPongNum = 10


def send_serial_data(arr):
    for i in range(len(arr)):
        s.write(bytes(arr[i], 'ascii'))
        while s.out_waiting > 0:
            pass

def read_and_separate_text(file_path):
    with open(file_path, 'r') as file:
        content = file.read()

    # Split content by spaces, newlines, and commas
    separated_text = content.replace(',', ' ').split()

    return separated_text


def manual_dictionary(list_of_words, known_words):
    result = []
    i = 0
    while i < len(list_of_words):
        word = list_of_words[i]
        if word in known_words:
            result.append(known_words[word]) #opcode

            # Special handling for "07" (stepper_scan)
            if known_words[word] == ascii(7):
                if i + 1 < len(list_of_words):  # Ensure there is a next element
                    try:
                        result.append(ascii(int(int(list_of_words[i + 1]) / 100)))
                        result.append(ascii(int(int(list_of_words[i + 1])% 100 / 10)))
                        result.append(ascii(int(int(list_of_words[i + 1]) % 10)))
                    except ValueError:
                        pass  # Handle cases where the next word is not a number
                if i + 2 < len(list_of_words):  # Ensure there is a third element
                    try:
                        result.append(ascii(int(int(list_of_words[i + 2]) / 100)))
                        result.append(ascii(int(int(list_of_words[i + 2])% 100 / 10)))
                        result.append(ascii(int(int(list_of_words[i + 2]) % 10)))
                    except ValueError:
                        pass  # Handle cases where the third word is not a number
                i += 2  # Skip the next two words
           #     result.append('y')  # we will use this to know theres a new command after this

            # For other commands, just add the next word if it exists
            elif known_words[word] not in [ascii(5), ascii(8)]:  # These don't need an additional word
                if i + 1 < len(list_of_words):
                    try:
                        result.append(ascii(int(int(list_of_words[i + 1]) / 100)))
                        result.append(ascii(int(int(list_of_words[i + 1])% 100 / 10)))
                        result.append(ascii(int(int(list_of_words[i + 1]) % 10)))
                    except ValueError:
                        pass  # Handle cases where the next word is not a number
                i += 1  # Skip the next word
                #result.append('y')  # we will use this to know theres a new command after this

          #  else:
           #     result.append('y')  # we will use this to know theres a new command after this
        i += 1  # Move to the next word
    result.append('z')  # we will use this to know it ended

    return result
known_words = {
    "inc_lcd": ascii(1),
    "dec_lcd": ascii(2),
    "rra_lcd": ascii(3),
    "set_delay": ascii(4),
    "clear_lcd": ascii(5),
    "stepper_deg": ascii(6),
    "stepper_scan": ascii(7),
    "sleep": ascii(8)
}

# Test the function with your file path
file_path = r'C:\Users\Kobi\sifrariim\final_lab\GUI\GUI\Script1.txt'

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


def calc_location(arr):
    if len(arr) == 9:
        X_num = int(arr[1]) * 1000 + int(arr[2]) * 100 + int(arr[3]) * 10 + int(arr[4])
        Y_num = int(arr[5]) * 1000 + int(arr[6]) * 100 + int(arr[7]) * 10 + int(arr[8])
        arr_gui = [int(arr[0]), X_num, Y_num]
       #print(arr_gui)
        return arr_gui
    return [2, 512, 512]

class DrawingApp:
    def __init__(self, root):
        self.user_input= None
        self.root = root
        self.root.title("DCS Final Project")
        self.iniside4 = False
        self.sent_Scripts = False
        self.threads_running = True

        # Load images for different states
        self.load_images()

        # Create a canvas for drawing
        self.canvas = tk.Canvas(root, bg="white", width=1400, height=700)
        self.canvas.pack()

        # Display the welcome message
        self.display_welcome_message()
        self.valid_bit = False

        #new
        self.script_files = []
        self.create_script_buttons()
        threading.Thread(target=self.read_serial_data, daemon=True).start()
        threading.Thread(target=self.read_angle, daemon=True).start()
        self.script_paths = None

        # Variables to track the pencil's position and state
        self.last_x = None
        self.last_y = None
        self.drawing_state = 2 # 0: Draw, 1: Erase, 2: Do nothing
        self.keep_drawing = False
        self.pencil_dot = None  # Store the dot reference
        self.last_key = None
        self.joystick_neutral = True
        self.already_drawed = False

        # Bind keypress events to the canvas
        self.root.bind("<Key>", self.handle_keypress)


    def load_images(self):
        try:
            # Load images for draw, erase, and do nothing states
            self.draw_image = Image.open("draw_icon.png")
            self.erase_image = Image.open("erase_icon.png")
            self.nothing_image = Image.open("nothing_icon.png")

            # Resize images if needed
            self.draw_image = self.draw_image.resize((20, 20))
            self.erase_image = self.erase_image.resize((20, 20))
            self.nothing_image = self.nothing_image.resize((20, 20))

            # Convert images to PhotoImage
            self.draw_photo = ImageTk.PhotoImage(self.draw_image)
            self.erase_photo = ImageTk.PhotoImage(self.erase_image)
            self.nothing_photo = ImageTk.PhotoImage(self.nothing_image)

        except Exception as e:
            print(f"Error loading images: {e}")
            self.draw_photo = None
            self.erase_photo = None
            self.nothing_photo = None

    def display_welcome_message(self):
        # Display the welcome text
        welcome_text = "Welcome to Or's and Kobi's\nDCS Final Project"
        self.canvas.create_text(700, 50, text=welcome_text, font=("Arial", 16), fill="black", justify="center")

        # Display the menu
        menu_text = "1. Stepper Engine Control System\n2. Drawing Mode \n3. Calibrate your Stepper Engine\n4. Script Mode"
        self.canvas.create_text(700, 150, text=menu_text, font=("Arial", 14), fill="black", justify="left")

    def destroy_all_buttons(self):
        for widget in self.root.winfo_children():
            if isinstance(widget, tk.Button):
                widget.destroy()

    def handle_keypress(self, event):
        self.destroy_all_buttons()
        arr = []
        # Save the key pressed by the user
        self.user_input = event.char
        self.last_key = self.user_input

        # Stop the real-time drawing loop if it's running
        self.keep_drawing = False
        if self.user_input== '1' or self.user_input == '2' or self.user_input =='3' or self.user_input== '4':
            bytesChar = bytes(self.user_input, 'ascii')
            s.write(bytesChar)
            while s.out_waiting > 0:
                pass
            s.flush()

        if self.user_input == '1':
            self.iniside4 = False
            self.sent_Scripts = False
            self.canvas.delete("all")  # Clear the canvas
            self.canvas.create_text(700, 15, text="Manually controlling the Stepper Engine:", font=("Arial", 16), fill="black")
        elif self.user_input == '2' or self.last_key == '2':
            self.iniside4 = False
            self.sent_Scripts = False
            self.canvas.delete("all")  # Clear the canvas
            self.canvas.create_text(700, 15, text="Drawing Mode:", font=("Arial", 16), fill="black")
            self.last_y = 350
            self.last_x = 700
            # Start the serial reading thread
            self.threads_running = True
            self.serial_thread = threading.Thread(target=self.read_serial_data)
            self.serial_thread.start()

        elif self.user_input == '3':
            self.iniside4 = False
            self.sent_Scripts = False
            self.canvas.delete("all")  # Clear the canvas
            self.canvas.create_text(650, 15, text="Calibration started...", font=("Arial", 16), fill="black")

        elif self.user_input == '4':
            self.threads_running = True
            self.iniside4 = True
            self.sent_Scripts = False
            self.canvas.delete("all")  # Clear the canvas
            self.canvas.create_text(650, 15, text="Script Mode", font=("Arial", 16), fill="black")
            self.create_script_buttons()
           # self.send_all_script_data() #new line
            #prepare files:
        # separated_text1 = read_and_separate_text(file_path)
         #   processed_text1 = manual_dictionary(separated_text1, known_words)

            #send data
           # send_serial_data(processed_text1)

        elif self.user_input == ' ':
            s.write(bytes('~', 'ascii'))
            while s.out_waiting > 0:
                pass
            self.sent_Scripts = True
            self.threads_running = False
            self.iniside4 = False
            self.sent_Scripts = False
            self.canvas.delete("all")  # Clear the canvas
            self.display_welcome_message()


        elif self.user_input == '\r' :
            if  self.iniside4 == True and self.sent_Scripts == True:

                self.canvas.delete("all")
                self.canvas.create_text(650, 50, text="Press Enter for Next command\nPress Space to Return to Main menu", font=("Arial", 16), fill="black")

                self.angle_thread = threading.Thread(target=self.read_angle)
                self.angle_thread.start()

                s.write(bytes('q', 'ascii'))
                while s.out_waiting > 0:
                    pass
                self.sent_Scripts = True



            else:
                error_text1 = "No File Uploaded\n Press Space To return to Main menu"
                self.canvas.create_text(650, 50, text=error_text1, font=("Arial", 16), fill="black", justify="center")


        #    if self.user_input != 2:  # Check if user_input is not empty
        # main_final_lab.embeded(self.user_input)

    def create_script_buttons(self):
        # Create buttons to upload scripts
        self.button1 = tk.Button(self.root, text="Upload Script 1", command=lambda: self.load_script(1))
        self.button1.place(x=600, y=100)

        self.button2 = tk.Button(self.root, text="Upload Script 2", command=lambda: self.load_script(2))
        self.button2.place(x=600, y=150)

        self.button3 = tk.Button(self.root, text="Upload Script 3", command=lambda: self.load_script(3))
        self.button3.place(x=600, y=200)

        # Create a button to process and send the scripts
        self.process_button = tk.Button(self.root, text="Process and Send", command=self.send_all_script_data, state=tk.DISABLED)
        self.process_button.place(x=600, y=250)


    def load_buttons(self):
        """Create buttons to load script files."""
        button_frame = tk.Frame(self.root)
        button_frame.pack(side=tk.TOP, pady=10)

        self.script_paths = []

        for i in range(1, 4):
            button = tk.Button(button_frame, text=f"Load Script {i}", command=lambda i=i: self.load_script(i))
            button.pack(side=tk.LEFT, padx=5)

    def load_script(self, script_number):
        file_path = tk.filedialog.askopenfilename(title=f"Select Script {script_number}")
        if file_path:
            self.script_files.append(file_path)
            # Enable the "Process and Send" button after any file is loaded
            self.process_button.config(state=tk.NORMAL)

    def send_all_script_data(self):
        self.canvas.delete("all")  # Clear the canvas
        self.canvas.create_text(650, 15, text="Processing Scripts...", font=("Arial", 16), fill="black")

        for file_path in self.script_files:
            separated_text = read_and_separate_text(file_path)
            processed_text = manual_dictionary(separated_text, known_words)
            send_serial_data(processed_text)
        self.canvas.delete("all")  # Clear the canvas
        self.canvas.create_text(650, 50, text="Scripts Sent Successfully!\nPress Enter to Start the Execution", font=("Arial", 16), fill="black")
        self.sent_Scripts = True


    def read_angle(self):
        while  s.in_waiting > 0 and self.threads_running == True:
            checkbit = s.read(size=1)  # read 1 byte from the input buffer
            if checkbit == b'b':
                time.sleep(0.01)
                while(self.iniside4 == True):
                    angle = []
                    while  s.in_waiting > 0:

                        digitByte = s.read(size=1)  # read 1 byte from the input buffer
                        angle.append(digitByte.decode("ascii"))
                        time.sleep(0.001)  # delay for accurate read/write operations on both ends
                        if s.in_waiting == 0:
                            if(len(angle)==3):
                                actual_angle = (int(angle[0])*100 + int(angle[1])*10 + int(angle[2]))*360/510
                                print(actual_angle)
                                self.canvas.delete("all")
                                self.canvas.create_text(700, 50,
                                                        text=f"Angle: {actual_angle:.2f}",
                                                        font=("Arial", 16), fill="black")

                            angle = []
                            # Update GUI using root.after() for thread safety






    def read_serial_data(self):
        arr = []
        s.write(bytes('x', 'ascii'))

        while s.out_waiting > 0 and self.threads_running==True:
            pass
        s.flush()  # clean
        time.sleep(0.001)
        while self.user_input == '2' and s.in_waiting > 0 and self.threads_running==True:
            self.valid_bit = False
            charByte = s.read(size=1)  # read 1 byte from the input buffer
            arr.append(charByte.decode("ascii"))
            time.sleep(0.001)  # delay for accurate read/write operations on both ends

            if s.in_waiting == 0:
                print(arr)
                arr_gui = calc_location(arr)
                print(arr_gui)
                # Update GUI using root.after() for thread safety
                self.valid_bit=True
                self.root.after(0, self.start_real_time_drawing, arr_gui)
                arr = []
                s.write(bytes('x', 'ascii'))
                while s.out_waiting > 0:
                    pass
                s.flush()  # clean
                time.sleep(0.001)

    def start_real_time_drawing(self, arr):
        # Enable the real-time drawing loop
        if self.valid_bit ==  True:
            self.keep_drawing = True
            self.already_drawed = False
            self.update_drawing(arr)

    def update_drawing(self, arr):
        if self.keep_drawing:
            # Example: Replace this with actual data from the external system
            delta = 2
            self.drawing_state = arr[0]  # 0: Draw
            x = arr[1]
            y = arr[2]
            if( self.last_x is None or self.last_y is None):
                self.last_y = 350
                self.last_x = 700

            if (800< x <= 1023 and 800<y<=1023 and 50 < self.last_x and 50 <self.last_y ): #1
                x_new = self.last_x - delta
                y_new = self.last_y - delta
            elif (250 <= x <= 800 and 800< y <=1023 and 50 < self.last_y): #2
                x_new = self.last_x
                y_new = self.last_y - delta
            elif (0 <= x < 250 and 800 <y<=1023 and self.last_x <1200 and 50 <self.last_y  ): #3
                x_new = self.last_x + delta
                y_new = self.last_y - delta
            elif (800 < x <= 1023 and 250 <= y <= 800 and  50<self.last_x ): #4
                x_new = self.last_x - delta
                y_new = self.last_y
            elif 250 <= x <= 800 and 250 <= y <= 800:  # 5
                self.joystick_neutral = True
                x_new = self.last_x
                y_new = self.last_y
            elif (0 <= x < 250 and 250 <= y <= 800 and self.last_x <1200): #6
                x_new = self.last_x + delta
                y_new = self.last_y
            elif (800 < x <= 1023 and 0 <= y < 250 and 50<self.last_x and self.last_y < 600 ): #7
                x_new = self.last_x - delta
                y_new = self.last_y + delta
            elif (250 <= x <= 800 and 0 <= y < 250 and self.last_y < 600): #8
                x_new = self.last_x
                y_new = self.last_y + delta
            elif (0 <= x < 250 and 0 <= y < 250 and self.last_y < 600 and self.last_x < 1200): #9
                x_new = self.last_x + delta
                y_new = self.last_y + delta
            else:
                x_new = self.last_x
                y_new = self.last_y
            self.joystick_neutral = False


            self.update_canvas( x_new, y_new)

            # Schedule the next update
            #self.root.after(50, self.update_drawing, arr)  # Update every 500 milliseconds changed

    def update_canvas(self, x, y):


        if (self.valid_bit==True and self.already_drawed==False):
            if self.drawing_state == 0:  # Draw
               # if self.last_x is not None and self.last_y is not None:
                    # Draw a line from the last position to the current position
                    self.canvas.create_line(self.last_x, self.last_y, x, y, fill="black", width=2)
                # Update the last position to the current position
                    self.last_x = x
                    self.last_y = y

            elif self.drawing_state == 1:  # Erase
                #if self.last_x is not None and self.last_y is not None:
                    # Draw a line with white color to simulate erasing
                    self.canvas.create_line(self.last_x, self.last_y, x, y, fill="white", width=10)
                    # Update the last position to the current position
                    self.last_x = x
                    self.last_y = y

            elif self.drawing_state == 2:  # Do nothing
                # Reset last_x and last_y to avoid drawing lines
                    self.last_x = x
                    self.last_y = y

            self.already_drawed = True
            # Update the dot (pencil icon) based on the state
            self.update_pencil_icon()

    def update_pencil_icon(self):
        # Remove the previous dot if it exists
        if hasattr(self, 'pencil_dot') and self.pencil_dot is not None:
            self.canvas.delete(self.pencil_dot)

        # Select the appropriate image for the state
        if self.drawing_state == 0:
            image = self.draw_photo
        elif self.drawing_state == 1:
            image = self.erase_photo
        elif self.drawing_state == 2:
            image = self.nothing_photo

        # Create a new dot at the current position
        self.pencil_dot = self.canvas.create_image(self.last_x,self.last_y - 15 , image=image, anchor=tk.NW)

# Main loop
if __name__ == "__main__":

    s = ser.Serial('COM8', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   timeout=1)  # timeout of 1 sec where the read and write operations are blocking,

    # after the timeout the program continues
    enableTX = False

    s.reset_input_buffer()
    s.reset_output_buffer()
    # printMenu()

    root = tk.Tk()
    app = DrawingApp(root)
    app.destroy_all_buttons()
    root.mainloop()
