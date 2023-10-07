import math
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

            # Draw lines on y-axis
            if self.is_in_grid(x0, self.padding):
                self.canvas.create_line(x0, y0, x1, y1, fill=fill_color)

            x0, y0, x1, y1 = (self.padding,
                              i * self.canvas_size / self.grid_steps,
                              self.canvas_size - self.padding,
                              i * self.canvas_size / self.grid_steps)

            # Draw lines on x-axis
            if self.is_in_grid(self.padding, y0):
                self.canvas.create_line(x0, y0, x1, y1, fill=fill_color)

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

    def setup_touchscreen(self):
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<Motion>", self.on_hover)
        self.ball_selector.bind("<<ComboboxSelected>>", self.on_ball_select)
        self.canvas.tag_bind(self.base, "<Enter>", self.show_base_label)
        self.canvas.tag_bind(self.base, "<Leave>", self.hide_base_label)

    def create_buttons(self):
        self.grid_position = tk.Label(self.root, text="")
        self.grid_position.grid(row=2, column=0)

        self.ball_selector_value = tk.StringVar()
        self.ball_selector = ttk.Combobox(self.root, textvariable=self.ball_selector_value,
                                          values=[ball.name for ball in self.balls], state='readonly')
        self.ball_selector.grid(row=3, column=0, pady=(10, 20))
        self.ball_selector.set('Select a ball')

        delete_button = tk.Button(self.root, text="Disconnect Ball", command=self.delete_selected_ball)
        delete_button.grid(row=4, column=0, padx=(0, 350))

        forward_button = tk.Button(self.root, text="Forward", command=self.move_forward)
        forward_button.grid(row=4, column=0, padx=(0, 100))

        backward_button = tk.Button(self.root, text="Backward", command=self.move_backward)
        backward_button.grid(row=4, column=0, padx=(100, 0))

        stop_button = tk.Button(self.root, text="Stop", command=self.stop_movement)
        stop_button.grid(row=4, column=0, padx=(300, 0))

        max_speed_label = tk.Label(self.root, text="Max Speed:")
        max_speed_label.grid(row=8, column=0, padx=(0, 200))

        self.max_speed_value = tk.IntVar()
        self.max_speed_slider = tk.Scale(self.root, from_=0, to=100, orient="horizontal", variable=self.max_speed_value,
                                         command=self.on_max_speed_update)
        self.max_speed_slider.grid(row=8, column=0, pady=(0, 20))
        self.max_speed_slider.set(100)

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

    def show_position_label(self, ball):
        if ball.position_label:
            self.canvas.itemconfig(ball.position_label, state="normal")

    def hide_position_label(self, ball):
        if ball.position_label:
            self.canvas.itemconfig(ball.position_label, state="hidden")

    def show_base_label(self, event):
        self.canvas.itemconfig(self.base_label, state="normal")

    def hide_base_label(self, event):
        self.canvas.itemconfig(self.base_label, state="hidden")

    def tag_bind_position_label(self, ball):
        self.canvas.tag_bind(ball.gui_obj, "<Enter>",
                             lambda event, sel_ball=ball: self.show_position_label(sel_ball))
        self.canvas.tag_bind(ball.gui_obj, "<Leave>",
                             lambda event, sel_ball=ball: self.hide_position_label(sel_ball))

    def on_hover(self, event):
        if self.is_in_grid(event.x, event.y):
            grid_x, grid_y = self.canvas_to_grid_coords(event.x, event.y)
            self.grid_position.config(text=f"Cursor location: ({grid_x:.2f}, {grid_y:.2f})")
        else:
            self.grid_position.config(text="Cursor not in grid")

    def on_click(self, event):
        if self.is_in_grid(event.x, event.y):
            if self.selected_ball:
                grid_x, grid_y = self.canvas_to_grid_coords(event.x, event.y)
                data = {'x': grid_x, 'y': grid_y}

                if self.selected_ball.gui_obj is None:
                    # Create ball object for the first time
                    ball_obj = self.canvas.create_oval(event.x - 10,
                                                       event.y - 10,
                                                       event.x + 10,
                                                       event.y + 10,
                                                       fill=BALL_COLOR)
                    self.selected_ball.set_gui_object(ball_obj)

                    # Create position marker for the first time
                    position_label = self.canvas.create_text(event.x, event.y + 15,
                                                             text=f"{self.selected_ball.name}\n({grid_x:.2f}, {grid_y:.2f})",
                                                             anchor="n", state="hidden")
                    self.selected_ball.set_position_label(position_label)

                    # Set hover functionality to show and hide the ball position label
                    self.tag_bind_position_label(self.selected_ball)

                    # Update believes about current position
                    self.selected_ball.x_pos = grid_x
                    self.selected_ball.y_pos = grid_y

                    # Send initial position to ball
                    self.init_position(data)

                    # Remove first placement warning after placement
                    self.initial_ball_warning.destroy()
                    print(f"[GUI] Set initial ball location to: ({grid_x:.2f}, {grid_y:.2f})")
                else:
                    if self.selected_ball.has_target_location:
                        print(f"[GUI] Cannot override existing target location for {self.selected_ball.name}")
                        return
                    # Create move objective
                    self.move_to(data)
                    self.selected_ball.has_target_location = True

                    # Draw red cross at target location
                    self.selected_ball.set_target_object((
                        self.canvas.create_line(event.x - 10, event.y, event.x + 10, event.y, fill="red"),
                        self.canvas.create_line(event.x, event.y - 10, event.x, event.y + 10, fill="red")))

                    print(f"[GUI] Set ball target location to: ({grid_x:.2f}, {grid_y:.2f})")
        else:
            print("[GUI] Selection not in grid")

    def on_ball_select(self, event):
        # Remove the first placement warning when selecting other ball
        if self.initial_ball_warning:
            self.initial_ball_warning.destroy()

        selected_ball = self.ball_selector_value.get()
        print("[GUI] Selected ball:", selected_ball)

        for ball in self.balls:
            if ball.name == selected_ball:
                self.selected_ball = ball
                self.max_speed_slider.set(self.selected_ball.max_speed)
                if ball.gui_obj is None:
                    # If ball not placed show a warning
                    self.initial_ball_warning = tk.Label(self.root,
                                                         text="Please select the initial ball position on the grid",
                                                         bg="red",
                                                         fg="white")
                    self.initial_ball_warning.grid(row=1, column=0)
                return

    def on_max_speed_update(self, event):
        if self.selected_ball and self.selected_ball.gui_obj:
            new_max_speed = self.max_speed_value.get()
            self.selected_ball.max_speed = new_max_speed

    def update_state(self, ball_name, state_update):
        for ball in self.balls:
            if ball.name == ball_name:
                if ball.gui_obj:
                    # Retrieve data from state update
                    ball.x_pos = state_update['x']
                    ball.y_pos = state_update['y']
                    ball.rotation = state_update['rotation']
                    ball.cur_action = get_action_from_value(state_update['action'])

                    # If new state is IDLE clear all objectives
                    if ball.cur_action == ActionType.IDLE:
                        if ball.has_target_location:
                            ball.has_target_location = False
                            self.canvas.delete(ball.target_obj[0])
                            self.canvas.delete(ball.target_obj[1])
                            ball.target_obj = None

                    # Clear old GUI components
                    self.canvas.delete(ball.gui_obj)
                    self.canvas.delete(ball.position_label)
                    self.canvas.delete(ball.direction_obj)

                    # Draw the ball
                    x_canvas, y_canvas = self.grid_to_canvas_coords(ball.x_pos, ball.y_pos)
                    ball_obj = self.canvas.create_oval(x_canvas - 10,
                                                       y_canvas - 10,
                                                       x_canvas + 10,
                                                       y_canvas + 10,
                                                       fill=BALL_COLOR)
                    ball.set_gui_object(ball_obj)

                    # Create position marker
                    position_label = self.canvas.create_text(x_canvas, y_canvas + 15,
                                                             text=f"{ball.name}\n({ball.x_pos:.2f}, {ball.y_pos:.2f})",
                                                             anchor="n", state="hidden")
                    ball.set_position_label(position_label)
                    self.tag_bind_position_label(ball)

                    # Draw direction arrow
                    arrow_length = 25
                    arrow_width = 3
                    arrow_x = x_canvas + arrow_length * math.cos(math.radians(-1 * ball.rotation + 90))
                    arrow_y = y_canvas - arrow_length * math.sin(math.radians(-1 * ball.rotation + 90))
                    direction_arrow = self.canvas.create_line(x_canvas, y_canvas, arrow_x, arrow_y,
                                                              fill="green", arrow=tk.LAST, width=arrow_width)
                    ball.set_direction_object(direction_arrow)

                    print(f"[GUI] New state of {ball_name} is x={ball.x_pos}, y={ball.y_pos}, rotation={ball.rotation}. {ball.cur_action}")
                return

    def register_ball(self, ball_id, mqtt_connector):
        # If ball not yet registered
        if all(ball_id != ball.id for ball in self.balls):
            registered_ball = Ball(ball_id, mqtt_connector)
            registered_ball.set_gui_object(None)

            self.balls.append(registered_ball)
            self.ball_selector['values'] = [ball.name for ball in self.balls]

            registered_ball.mqtt_connector.subscribe(registered_ball.name + "/state")
        else:
            print("[GUI] Duplicate ball registration: " + str(ball_id))

    def delete_selected_ball(self):
        if self.selected_ball:
            self.delete_ball(self.selected_ball)

    def deregister_ball(self, ball_name):
        for ball in self.balls:
            if ball.name == ball_name:
                self.delete_ball(ball)
                return True
        return False

    def delete_ball(self, ball):
        # If ball is placed remove all GUI components
        if ball.gui_obj:
            self.canvas.delete(ball.gui_obj)
            self.canvas.delete(ball.position_label)

        # If ball had move objective remove all GUI components
        if ball.has_target_location:
            self.canvas.delete(ball.target_obj[0])
            self.canvas.delete(ball.target_obj[1])
            ball.target_obj = None

        # If ball had a known direction remove the arrow GUI component
        if ball.direction_obj:
            self.canvas.delete(ball.direction_obj)

        # Remove internal data of ball
        self.balls.remove(ball)
        ball.mqtt_connector.unsubscribe(ball.name + "/state")
        self.disconnect()

        # Refresh ball selector list
        self.ball_selector['values'] = [ball.name for ball in self.balls]

        # Reset GUI components if deleted ball was currently selected
        if self.selected_ball and ball.name == self.selected_ball.name:
            self.initial_ball_warning.destroy()
            self.ball_selector.set('Select a ball')
            self.max_speed_slider.set(100)
            self.selected_ball = None

        print(f"[GUI] Disconnected ball: {ball.name}")

    def move_forward(self):
        if self.selected_ball:
            if self.selected_ball.has_target_location:
                print(f"[GUI] Action not permitted because {self.selected_ball.name} has a target")
                return
            speed = self.max_speed_value.get()
            data = {'speed': speed}
            self.selected_ball.action(ActionType.FORWARD, data)

    def move_backward(self):
        if self.selected_ball:
            if self.selected_ball.has_target_location:
                print(f"[GUI] Action not permitted because {self.selected_ball.name} has a target")
                return
            speed = self.max_speed_value.get()
            data = {'speed': speed}
            self.selected_ball.action(ActionType.BACKWARD, data)

    def move_to(self, data):
        if self.selected_ball:
            data['speed'] = self.max_speed_value.get()
            self.selected_ball.action(ActionType.MOVETO, data)

    def init_position(self, data):
        if self.selected_ball:
            self.selected_ball.action(ActionType.INIT, data)

    def disconnect(self):
        if self.selected_ball:
            self.selected_ball.action(ActionType.DISCONNECT)

    def stop_movement(self):
        if self.selected_ball:
            self.selected_ball.action(ActionType.STOP)
