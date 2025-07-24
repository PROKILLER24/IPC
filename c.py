import websocket
import threading
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

data_points = []

def on_message(ws, message):
    print("[C] Received:", message)
    try:
        value = float(message.strip())
        data_points.append(value)
        if len(data_points) > 500:
            data_points.pop(0)
    except Exception as e:
        print("Error parsing:", e)

def on_open(ws):
    print("[C] Connected to WebSocket")

def run_ws():
    ws = websocket.WebSocketApp("ws://localhost:9002",
                                on_message=on_message,
                                on_open=on_open)
    ws.run_forever()

if __name__ == "__main__":
    threading.Thread(target=run_ws, daemon=True).start()

    plt.style.use('ggplot')
    fig, ax = plt.subplots()
    line, = ax.plot([], [], 'o-')

    def update(frame):
        line.set_data(range(len(data_points)), data_points)
        ax.relim()
        ax.autoscale_view()
        return line,

    ani = FuncAnimation(fig, update, interval=200)
    plt.title("Live Left RPM Data")
    plt.xlabel("Samples")
    plt.ylabel("Left RPM")
    plt.tight_layout()
    plt.show()
