import customtkinter as ctk
import tkinter as tk
from tkinter import ttk
import sys
import threading
import time
import queue
from collections import deque
import re

ctk.set_appearance_mode("light")
ctk.set_default_color_theme("blue")

class IoTMonitor:
    def __init__(self, root):
        self.root = root
        root.title("IoT OS Dashboard")
        root.geometry("1100x750")
        
        self.msg_queue = queue.Queue()
        self.history = deque(maxlen=120)
        self.last_value = None
        self.status = 'ACTIVE'
        self.power_level = 100
        self.animation_step = 0

        self.tabview = ctk.CTkTabview(root, width=1080, height=680)
        self.tabview.pack(padx=10, pady=10, fill="both", expand=True)
        self.tabview.add("Dashboard")
        self.tabview.add("Logs")

        self.setup_dashboard()
        self.setup_logs()

        self.running = True
        self.thread = threading.Thread(target=self.read_stream)
        self.thread.daemon = True
        self.thread.start()

        self.root.after(100, self.process_queue)

    def setup_dashboard(self):
        tab = self.tabview.tab("Dashboard")

        header = ctk.CTkFrame(tab)
        header.pack(fill='x', padx=10, pady=10)
        ctk.CTkLabel(header, text="IoT Sensor Monitor", font=ctk.CTkFont(size=32, weight="bold")).pack(side='left', padx=20, pady=10)
        self.status_label = ctk.CTkLabel(header, text="ACTIVE", font=ctk.CTkFont(size=16, weight="bold"), fg_color="#2ecc71", text_color="black")
        self.status_label.pack(side='right', padx=20, pady=10)

        content = ctk.CTkFrame(tab)
        content.pack(fill='both', expand=True, padx=10, pady=10)

        left = ctk.CTkFrame(content)
        left.pack(side='left', fill='y', padx=10, pady=10)

        ctk.CTkLabel(left, text="Current Temperature", font=ctk.CTkFont(size=18)).pack(pady=10)
        self.val_label = ctk.CTkLabel(left, text="WAITING...", font=ctk.CTkFont(size=72, weight="bold"), text_color="#2ecc71")
        self.val_label.pack(pady=20)

        stats_frame = ctk.CTkFrame(left)
        stats_frame.pack(pady=10)
        self.min_label = ctk.CTkLabel(stats_frame, text="Min: -", font=ctk.CTkFont(size=16))
        self.min_label.pack(pady=5)
        self.avg_label = ctk.CTkLabel(stats_frame, text="Avg: -", font=ctk.CTkFont(size=16))
        self.avg_label.pack(pady=5)
        self.max_label = ctk.CTkLabel(stats_frame, text="Max: -", font=ctk.CTkFont(size=16))
        self.max_label.pack(pady=5)

        ctk.CTkLabel(left, text="Power Efficiency", font=ctk.CTkFont(size=16)).pack(pady=10)
        self.power_bar = ctk.CTkProgressBar(left, width=200, height=20)
        self.power_bar.pack(pady=5)
        self.power_bar.set(1.0)

        right = ctk.CTkFrame(content)
        right.pack(side='right', fill='both', expand=True, padx=10, pady=10)

        ctk.CTkLabel(right, text="Temperature History", font=ctk.CTkFont(size=18)).pack(pady=10)
        self.chart = tk.Canvas(right, width=700, height=400, bg='#f8f9fa', highlightthickness=0)
        self.chart.pack(fill='both', expand=True)
        self._draw_chart_background()

    def setup_logs(self):
        tab = self.tabview.tab("Logs")
        self.log_text = tk.Text(tab, height=20, bg='#ecf0f1', fg='#2c3e50', font=('Consolas', 12))
        self.log_text.pack(fill='both', expand=True, padx=10, pady=10)
        ctk.CTkButton(tab, text='Clear Log', command=self.clear_log).pack(pady=10)

    def _draw_chart_background(self):
        self.chart.delete('bg')
        w, h = 700, 400
        for i in range(0, w, 50):
            self.chart.create_line(i, 0, i, h, fill='#d1ecf1', tags='bg')
        for i in range(0, h, 50):
            self.chart.create_line(0, i, w, i, fill='#d1ecf1', tags='bg')

    def read_stream(self):
        for line in sys.stdin:
            self.msg_queue.put(line.strip())

    def process_queue(self):
        while not self.msg_queue.empty():
            line = self.msg_queue.get()
            self.handle_message(line)
            self.log_text.insert(tk.END, line + '\n')
            self.log_text.see(tk.END)
        self.root.after(100, self.process_queue)

    def handle_message(self, line):
        print(f"DEBUG: Received line: {line}")  # Debug print
        if "[SENSOR] Temp:" in line:
            match = re.search(r"Temp: (\d+)", line)
            if match:
                temp = int(match.group(1))
                print(f"DEBUG: Parsed temp: {temp}")  # Debug print
                self.history.append(temp)
                self.update_display(temp)
        if "[SYS]" in line:
            if "SLEEP" in line:
                self.status = 'SLEEP'
                self.status_label.configure(text="SLEEP", fg_color="#e67e22")
            elif "ACTIVE" in line:
                self.status = 'ACTIVE'
                self.status_label.configure(text="ACTIVE", fg_color="#2ecc71")

    def update_display(self, temp):
        self.val_label.configure(text=f"{temp}°C")
        if self.history:
            self.min_label.configure(text=f"Min: {min(self.history)}°C")
            self.avg_label.configure(text=f"Avg: {sum(self.history)/len(self.history):.1f}°C")
            self.max_label.configure(text=f"Max: {max(self.history)}°C")
        self._draw_chart()

    def _draw_chart(self):
        self.chart.delete('line')
        vals = list(self.history)
        if not vals:
            return
        w, h = 700, 400
        vmin = min(vals)
        vmax = max(max(vals), 45)
        if vmax == vmin:
            vmax = vmin + 1

        def to_xy(idx, val):
            x = int(idx * (w / max(1, (len(vals) - 1))))
            y = int(h - ((val - vmin) / (vmax - vmin)) * h)
            return x, y

        points = []
        for i, v in enumerate(vals):
            points.extend(to_xy(i, v))
        if len(points) >= 4:
            self.chart.create_line(*points, fill='#1abc9c', width=3, tags='line', smooth=True)
        lx, ly = to_xy(len(vals) - 1, vals[-1])
        self.chart.create_oval(lx-4, ly-4, lx+4, ly+4, fill='#e67e22', outline='', tags='line')

    def clear_log(self):
        self.log_text.delete(1.0, tk.END)

if __name__ == "__main__":
    root = ctk.CTk()
    app = IoTMonitor(root)
    root.mainloop()
