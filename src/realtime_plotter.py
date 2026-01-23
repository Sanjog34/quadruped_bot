import serial
import time
import math
import threading
import matplotlib.pyplot as plt

PORT = "/dev/ttyUSB0"
BAUD = 115200
TRIGGER_CHAR = "a"   # character sent to ESP

L1 = 8   # thigh length
L2 = 10.25   # shank length

START_TOKEN = "start"
END_TOKEN = "end"


# ---------------- SERIAL READER ----------------
def serial_reader(ser, data_queue):
    recording = False

    while True:
        try:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue

            print("ESP →", line)

            # ---- START ----
            if line.lower() == START_TOKEN:
                recording = True
                continue

            # ---- END ----
            if line.lower() == END_TOKEN:
                recording = False

                # 🔥 send trigger again after END
                print("END received → requesting next burst")
                ser.write((TRIGGER_CHAR + "\n").encode())

                continue

            # ---- DATA ----
            if recording:
                parts = line.split(",")
                if len(parts) == 4:
                    try:
                        hip = float(parts[0])
                        knee = float(parts[1])
                        length = float(parts[2])
                        height = float(parts[3])

                        # plot ONE tuple at a time
                        data_queue.append((hip, knee))

                    except ValueError:
                        pass

        except serial.SerialException:
            break



# ---------------- REALTIME PLOT ----------------
def realtime_plot(data_queue):
    plt.ion()
    fig, ax = plt.subplots()

    ax.set_aspect("equal")
    ax.set_xlim(-8, 8)
    ax.set_ylim(-10, 0)
    ax.grid(True)

    while True:
        if data_queue:
            hip_deg, knee_deg = data_queue.pop(0)

            hip = math.radians(hip_deg)
            knee = math.radians(knee_deg)

            # Origin
            x0, y0 = 0, 0

            # Hip joint
            x1 = -L1 * math.cos(hip)
            y1 = -L1 * math.sin(hip)

            # Knee joint
            total_angle = hip + knee
            x2 = x1 - L2 * math.cos(total_angle)
            y2 = y1 - L2 * math.sin(total_angle)

            ax.cla()
            ax.set_aspect("equal")
            ax.set_xlim(-8, 8)
            ax.set_ylim(-10,0)
            ax.grid(True)

            ax.plot([x0, x1], [y0, y1], "o-", linewidth=3)
            ax.plot([x1, x2], [y1, y2], "o-", linewidth=3)

            ax.set_title(f"Hip={hip_deg:.1f}°, Knee={knee_deg:.1f}°")

            plt.pause(0.001)

        else:
            time.sleep(0.001)


# ---------------- MAIN ----------------
def main():
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)
    print("Serial connected")

    data_queue = []

    # Start serial reader thread
    t = threading.Thread(target=serial_reader, args=(ser, data_queue), daemon=True)
    t.start()

    # Trigger ESP
    print("Sending trigger to ESP")
    ser.write((TRIGGER_CHAR + "\n").encode())

    # Start plotting
    realtime_plot(data_queue)


if __name__ == "__main__":
    main()
