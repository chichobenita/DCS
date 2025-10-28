import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, simpledialog
import serial
import serial.tools.list_ports
import threading

ser = None

def get_available_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

def connect():
    global ser
    selected_port = port_var.get()
    if not selected_port:
        messagebox.showwarning("Warning", "Please select a COM port.")
        return
    try:
        ser = serial.Serial(selected_port, baudrate=9600, timeout=1)
        connect_button.config(state='disabled')
        port_menu.config(state='disabled')
        start_rx_loop()
        log_area.insert(tk.END, f"Connected to {selected_port}\n")
    except serial.SerialException as e:
        messagebox.showerror("Connection Error", f"Failed to open port: {e}")

def send_command(cmd):
    if ser and ser.is_open:
        try:
            ser.write(bytes(cmd + '\n', 'ascii'))
        except Exception as e:
            log_area.insert(tk.END, f"Send error: {e}\n")
            log_area.see(tk.END)

def button_click(n):
    if n == 3:
        x_value = simpledialog.askstring("Input", "Enter delay time X (ms):")
        if x_value and x_value.isdigit():

            send_command("@3"+str(x_value)+"#")
        else:
            messagebox.showerror("Invalid Input", "Please enter a valid number.")
    else:
        send_command("@"+str(n)+"#")

def start_rx_loop():
    def rx():
        while True:
            if ser and ser.in_waiting:
                try:
                    data = ser.readline().decode('ascii').strip()
                    if data:
                        log_area.insert(tk.END, f"Response: {data}\n")
                        log_area.see(tk.END)
                except Exception as e:
                    log_area.insert(tk.END, f"Receive error: {e}\n")
                    log_area.see(tk.END)
    threading.Thread(target=rx, daemon=True).start()

# GUI
root = tk.Tk()
root.title("MSP430 UART Control Panel")

# Port selection
top_frame = tk.Frame(root)
top_frame.pack(pady=5)

tk.Label(top_frame, text="Select COM Port:").pack(side=tk.LEFT, padx=5)
port_var = tk.StringVar()
port_menu = ttk.Combobox(top_frame, textvariable=port_var, values=get_available_ports(), width=10)
port_menu.pack(side=tk.LEFT, padx=5)

connect_button = tk.Button(top_frame, text="Connect", command=connect)
connect_button.pack(side=tk.LEFT, padx=5)

# Buttons 1–8
button_frame = tk.Frame(root)
button_frame.pack(pady=10)
for i in range(1, 9):
    b = tk.Button(button_frame, text=str(i), width=10, command=lambda n=i: button_click(n))
    b.grid(row=(i-1)//4, column=(i-1)%4, padx=5, pady=5)

# Log area
log_area = scrolledtext.ScrolledText(root, width=50, height=10)
log_area.pack(pady=10)

root.mainloop()
