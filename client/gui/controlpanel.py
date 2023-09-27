import tkinter as tk
from tkinter import ttk

from gui.bb8 import BB8

BALL_COLOR = "yellow"
BASE_COLOR = "white"


class ControlPanel:
    def __init__(self, root):
        self.root = root
        self.root.title("Control Panel IotBB8")
        self.controllers = list()
        self.canvas_width = 400
        self.canvas_height = 400
        self.base = None
        self.grid_canvas = None
        self.setup_canvas()
        self.create_buttons()

    def setup_canvas(self):
        self.grid_canvas = tk.Canvas(self.root, width=self.canvas_width, height=self.canvas_height)
        self.grid_canvas.pack()
        self.base = self.grid_canvas.create_rectangle(0, 0, 20, 20, fill=BASE_COLOR)

    def register_controller(self, controller):
        gui_object = self.grid_canvas.create_oval(controller.x_pos*20, controller.y_pos*20,
                                                  (controller.x_pos+1)*20,  (controller.y_pos+1)*20,
                                                  fill=BALL_COLOR)
        bb8 = BB8(controller, gui_object)
        self.controllers.append(bb8)

    def create_buttons(self):
        self.controller_var = tk.StringVar()
        self.controller_var.set(self.controllers[0].name)  # Set default value
        controller_menu = ttk.Combobox(self.root, textvariable=self.controller_var,
                                       values=[controller.name for controller in self.controllers])
        controller_menu.pack(pady=10)

        # Buttons to perform actions
        forward_button = tk.Button(self.root, text="Forward", command=lambda: self.move_forward())
        forward_button.pack(pady=5)
        backward_button = tk.Button(self.root, text="Backward", command=lambda: self.move_backward())
        backward_button.pack(pady=5)
        stop_button = tk.Button(self.root, text="Stop", command=lambda: self.stop_movement())
        stop_button.pack(pady=5)

    def move_forward(self):
        self.current_position = (self.current_position[0], self.current_position[1] + 1)
        self.update_ball_position()

    def move_backward(self):
        self.current_position = (self.current_position[0], self.current_position[1] - 1)
        self.update_ball_position()

    def stop_movement(self):
        pass

    def update_ball_position(self):
        x, y = self.current_position
        ball_x1, ball_y1 = x * 20, (y + 1) * 20
        ball_x2, ball_y2 = (x + 1) * 20, (y + 2) * 20
        self.grid_canvas.coords(self.ball, ball_x1, ball_y1, ball_x2, ball_y2)
