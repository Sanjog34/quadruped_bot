import serial
import time
import csv
import os
import threading

PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200

CSV_FILE = "leg_data.csv"
START_TOKEN = "start"
END_TOKEN = "end"


def wait_for_port():
    print(f"Waiting for {PORT} ...")
    while not os.path.exists(PORT):
        time.sleep(1)
    print(f"{PORT} connected")


def csv_is_empty(filename):
    return not os.path.exists(filename) or os.path.getsize(filename) == 0


def serial_reader(ser):
    """
    Reads data coming FROM ESP
    Handles start/end logging and CSV writing
    """
    recording = False
    csv_file = None
    csv_writer = None

    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue

            print(f"ESP → {line}")

            # ---- START ----
            if line.lower() == START_TOKEN:
                print("START received → Recording")

                recording = True

                if csv_file:
                    csv_file.close()

                file_empty = csv_is_empty(CSV_FILE)
                csv_file = open(CSV_FILE, "a", newline="")
                csv_writer = csv.writer(csv_file)

                if file_empty:
                    csv_writer.writerow([
                        "hip_angle",
                        "knee_angle",
                        "length",
                        "height_from_ground"
                    ])
                continue

            # ---- END ----
            if line.lower() == END_TOKEN:
                print("END received → Stop recording")

                recording = False
                if csv_file:
                    csv_file.close()
                    csv_file = None
                    csv_writer = None
                continue

            # ---- DATA ----
            if recording:
                parts = line.split(",")
                if len(parts) == 4:
                    try:
                        csv_writer.writerow([
                            float(parts[0]),
                            float(parts[1]),
                            float(parts[2]),
                            float(parts[3])
                        ])
                        csv_file.flush()
                    except ValueError:
                        pass

        except serial.SerialException:
            print("Serial read error")
            break


def serial_writer(ser):
    """
    Sends user input (chat/commands) TO ESP
    """
    while True:
        try:
            msg = input("You → ")
            ser.write((msg + "\n").encode())
        except serial.SerialException:
            print("Serial write error")
            break
        except KeyboardInterrupt:
            print("\nExiting...")
            os._exit(0)


def main():
    while True:
        try:
            wait_for_port()
            ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
            print("Serial opened")

            # Start reader & writer threads
            t_read = threading.Thread(target=serial_reader, args=(ser,), daemon=True)
            t_write = threading.Thread(target=serial_writer, args=(ser,), daemon=True)

            t_read.start()
            t_write.start()

            # Keep main alive
            while True:
                time.sleep(1)

        except serial.SerialException:
            print("USB disconnected. Reconnecting...")
            time.sleep(1)


if __name__ == "__main__":
    main()
