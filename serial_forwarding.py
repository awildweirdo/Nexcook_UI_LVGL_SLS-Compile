import serial


ser_in = serial.Serial('COM6', baudrate=115200, timeout=1)


ser_out = serial.Serial('COM9', baudrate=115200, timeout=1)

print("Forwarding data from COM6 → COM9... Press Ctrl+C to stop.")

try:
    while True:
        if ser_in.in_waiting > 0:  
            data = ser_in.read(ser_in.in_waiting)  
            print(f"Received: {data}")  
            ser_out.write(data)  
except KeyboardInterrupt:
    print("Stopped by user.")
finally:
    ser_in.close()
    ser_out.close()
