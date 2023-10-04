import tkinter as tk
from tkinter import ttk

from devices.action_type import ActionType, get_action_from_value
from devices.ball import Ball

BALL_COLOR = "yellow"
BASE_COLOR = "white"


class ControlPanel:
    def __init__(self, root):
        # General tkinter settings
        self.root = root
        self.root.title("Control Panel IotBB8")

        # Registered ball storage
        self.balls = list()
        self.selected_ball = None

        # Canvas & grid settings
        self.canvas_size = 600
        self.padding = 60
        self.grid_steps = 30
        self.base = None
        self.canvas = None

        # Tkinter menu variables
        self.max_speed_value = None
        self.max_speed_slider = None
        self.ball_selector = None
        self.grid_position = None
        self.ball_selector_value = None
        self.base_label = None
        self.initial_ball_warning = None

        # Setup the GUI
        self.setup_canvas()
        self.draw_grid()
        self.create_buttons()
        self.set_basestation_icon()
        self.setup_touchscreen()

    def setup_canvas(self):
        self.canvas = tk.Canvas(self.root, width=self.canvas_size, height=self.canvas_size)
        self.canvas.grid(row=0, column=0)

    def draw_grid(self):
        for i in range(0, self.grid_steps + 1, 1):
            fill_color = "blue" if i == self.grid_steps // 2 else "gray"

            x0, y0, x1, y1 = (i * self.canvas_size / self.grid_steps,
                              self.padding,
                              i * self.canvas_size / self.grid_steps,
                              self.canvas_size - self.padding)

            if self.is_in_grid(x0, self.padding):
                self.canvas.create_line(x0, y0, x1, y1, fill=fill_color)

            x0, y0, x1, y1 = (self.padding,
                              i * self.canvas_size / self.grid_steps,
                              self.canvas_size - self.padding,
                              i * self.canvas_size / self.grid_steps)

            if self.is_in_grid(self.padding, y0):
                self.canvas.create_line(x0, y0, x1, y1, fill=fill_color)

    def canvas_to_grid_coords(self, x, y):
        x_grid = x - self.canvas_size // 2
        y_grid = -1 * y + self.canvas_size // 2
        return x_grid, y_grid

    def grid_to_canvas_coords(self, x, y):
        x_canvas = x + self.canvas_size // 2
        y_canvas = -1 * y + self.canvas_size // 2
        return x_canvas, y_canvas

    def is_in_grid(self, x, y):
        if (x < self.padding or x > self.canvas_size - self.padding or
                y < self.padding or y > self.canvas_size - self.padding):
            return False
        else:
            return True

    def on_click(self, event):
        if self.is_in_grid(event.x, event.y):
            if self.selected_ball and self.selected_ball.gui_obj is None:
                grid_x, grid_y = self.canvas_to_grid_coords(event.x, event.y)

                ball_obj = self.canvas.create_oval(event.x - 10,
                                                   event.y - 10,
                                                   event.x + 10,
                                                   event.y + 10,
                                                   fill=BALL_COLOR)
                self.selected_ball.set_gui_object(ball_obj)

                position_label = self.canvas.create_text(event.x, event.y, text=f"({grid_x:.2f}, {grid_y:.2f})")
                self.selected_ball.set_position_label(position_label)

                self.selected_ball.x_pos = grid_x
                self.selected_ball.y_pos = grid_y

                self.initial_ball_warning.destroy()
                print(f"[GUI] Set initial ball location to: ({grid_x:.2f}, {grid_y:.2f})")
                return
        else:
            print("[GUI] Selection not in grid")

    def on_hover(self, event):
        if self.is_in_grid(event.x, event.y):
            grid_x, grid_y = self.canvas_to_grid_coords(event.x, event.y)
            self.grid_position.config(text=f"Cursor location: ({grid_x:.2f}, {grid_y:.2f})")
        else:
            self.grid_position.config(text="Cursor not in grid")

    def on_ball_select(self, event):
        selected_ball = self.ball_selector_value.get()
        print("[GUI] Selected ball:", selected_ball)

        for ball in self.balls:
            if ball.name == selected_ball:
                self.selected_ball = ball
                self.max_speed_slider.set(self.selected_ball.max_speed)
                if ball.gui_obj is None:
                    self.initial_ball_warning = tk.Label(self.root,
                                                         text="Please select the initial ball position on the grid",
                                                         bg="red",
                                                         fg="white")
                    self.initial_ball_warning.grid(row=1, column=0)
                else:
                    self.initial_ball_warning.destroy()
                return

    def register_ball(self, ball_id, mqtt_connector):
        if all(ball_id != ball.id for ball in self.balls):
            registered_ball = Ball(ball_id, mqtt_connector)
            registered_ball.set_gui_object(None)
            self.balls.append(registered_ball)
            self.ball_selector['values'] = [ball.name for ball in self.balls]
            registered_ball.mqtt_connector.subscribe(registered_ball.name+"/state")
        else:
            print("[GUI] Duplicate ball registration: " + str(ball_id))

    def update_state(self, ball_id, state_update):
        for ball in self.balls:
            if ball.name == ball_id:
                if ball.gui_obj:
                    ball.x_pos = state_update['x']
                    ball.y_pos = state_update['y']
                    ball.cur_action = get_action_from_value(state_update['action'])

                    self.canvas.delete(ball.gui_obj)
                    self.canvas.delete(ball.position_label)
                    x_canvas, y_canvas = self.grid_to_canvas_coords(ball.x_pos, ball.y_pos)
                    ball_obj = self.canvas.create_oval(x_canvas - 10,
                                                       y_canvas - 10,
                                                       x_canvas + 10,
                                                       y_canvas + 10,
                                                       fill=BALL_COLOR)
                    ball.set_gui_object(ball_obj)

                    position_label = self.canvas.create_text(x_canvas, y_canvas, text=f"({ball.x_pos:.2f}, {ball.y_pos:.2f})")
                    ball.set_position_label(position_label)
                print(f"[GUI] New state of {ball_id} is {ball.x_pos}, {ball.y_pos}, {ball.cur_action}")
                return

    def create_buttons(self):
        self.grid_position = tk.Label(self.root, text="")
        self.grid_position.grid(row=2, column=0)

        self.ball_selector_value = tk.StringVar()
        self.ball_selector = ttk.Combobox(self.root, textvariable=self.ball_selector_value,
                                          values=[ball.name for ball in self.balls], state='readonly')
        self.ball_selector.grid(row=3, column=0, pady=(10, 20))
        self.ball_selector.set('Select a ball')

        delete_button = tk.Button(self.root, text="Disconnect Ball", command=self.delete_selected_ball)
        delete_button.grid(row=4, column=0,  padx=(0, 350))

        forward_button = tk.Button(self.root, text="Forward", command=self.move_forward)
        forward_button.grid(row=4, column=0,  padx=(0, 100))

        backward_button = tk.Button(self.root, text="Backward", command=self.move_backward)
        backward_button.grid(row=4, column=0,  padx=(100, 0))

        stop_button = tk.Button(self.root, text="Stop", command=self.stop_movement)
        stop_button.grid(row=4, column=0,  padx=(300, 0))

        max_speed_label = tk.Label(self.root, text="Max Speed:")
        max_speed_label.grid(row=8, column=0, padx=(0, 150))

        self.max_speed_value = tk.IntVar()
        self.max_speed_slider = tk.Scale(self.root, from_=0, to=100, orient="horizontal", variable=self.max_speed_value,
                                         command=self.on_max_speed_update)
        self.max_speed_slider.grid(row=8, column=0, pady=(0, 20))
        self.max_speed_slider.set(100)

    def move_forward(self):
        if self.selected_ball:
            speed = self.max_speed_value.get()
            data = {'speed' : speed}
            self.selected_ball.action(ActionType.FORWARD, data)

    def move_backward(self):
        if self.selected_ball:
            speed = self.max_speed_value.get()
            data = {'speed' : speed}
            self.selected_ball.action(ActionType.BACKWARD, data)

    def move_to(self, data):
        if self.selected_ball:
            data['speed'] = self.max_speed_value.get()
            self.selected_ball.action(ActionType.MOVETO, data)

    def stop_movement(self):
        if self.selected_ball:
            self.selected_ball.action(ActionType.STOP)

    def setup_touchscreen(self):
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<Motion>", self.on_hover)
        self.ball_selector.bind("<<ComboboxSelected>>", self.on_ball_select)
        self.canvas.tag_bind(self.base, "<Enter>", self.show_base_label)
        self.canvas.tag_bind(self.base, "<Leave>", self.hide_base_label)

    def set_basestation_icon(self):
        canvas_x, canvas_y = self.grid_to_canvas_coords(0, 0)
        self.base = self.canvas.create_rectangle(canvas_x - 10,
                                                 canvas_y - 10,
                                                 canvas_x + 10,
                                                 canvas_y + 10,
                                                 fill=BASE_COLOR)
        # Add a label
        label_x = (canvas_x - 10 + canvas_x + 10) / 2
        label_y = canvas_y + 20
        self.base_label = self.canvas.create_text(label_x, label_y, text="Base Station", anchor="n", state="hidden")

    def show_base_label(self, event):
        self.canvas.itemconfig(self.base_label, state="normal")

    def hide_base_label(self, event):
        self.canvas.itemconfig(self.base_label, state="hidden")

    def delete_selected_ball(self):
        if self.selected_ball:
            if self.selected_ball.gui_obj:
                self.canvas.delete(self.selected_ball.gui_obj)
                self.canvas.delete(self.selected_ball.position_label)
            else:
                self.initial_ball_warning.destroy()
            self.balls.remove(self.selected_ball)
            self.selected_ball.mqtt_connector.unsubscribe(self.selected_ball.name + "/state")
            self.ball_selector['values'] = [ball.name for ball in self.balls]
            self.ball_selector.set('Select a ball')
            self.max_speed_slider.set(100)
            print(f"[GUI] Disconnected ball: {self.selected_ball.name}")
            self.selected_ball = None
            return

    def on_max_speed_update(self, event):
        new_max_speed = self.max_speed_value.get()
        if self.selected_ball:
            self.selected_ball.max_speed = new_max_speed
            print(f"[GUI] Updated max speed for {self.selected_ball.name} to {new_max_speed}%")