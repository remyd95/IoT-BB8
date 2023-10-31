from devices.action_type import ActionType, get_action_from_value
from devices.ball import Ball

import atexit
import math
import os
import tkinter as tk
from tkinter import ttk
import logging

# Logging settings
logging.basicConfig(encoding='utf-8',
                    level=logging.DEBUG,
                    format='[%(asctime)s] [%(levelname)s] %(message)s',
                    datefmt='%H:%M:%S')

# Important fix for key release behaviour
os.system('xset r off')

# GUI settings
BALL_COLOR = "yellow"
BASE_COLOR = "white"


def clean_up():
    """
    Clean up function that is called when the program exits.
    :return: None
    """
    os.system('xset r on')


class ControlPanel:
    """
    The ControlPanel class is responsible for the GUI of the control panel.
    It provides a grid where the user can select a ball and place it on the grid.
    The user can also select a target location for the ball to move to.
    """

    def __init__(self, root, client_id, mqtt_connector):
        """
        Initializes the ControlPanel class
        :param root:
        :param client_id:
        :param mqtt_connector:
        """
        # General tkinter settings
        self.client_id = client_id
        self.root = root
        self.root.title("Control Panel IotBB8")
        self.mqtt_connector = mqtt_connector

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
        self.keyboard_button = None
        self.imu_labels = {}
        self.grid_row_offset = 20

        # Set up the GUI
        self.setup_canvas()
        self.draw_grid()
        self.create_buttons()
        self.set_basestation_icon()
        self.setup_touchscreen()

        # Keyboard bindings
        self.keyboard_mode = False
        self.active_key = None
        self.root.bind("<KeyPress>", self.on_key_press)
        self.root.bind("<KeyRelease>", self.on_key_release)

        # Cleanup after exiting control panel
        atexit.register(clean_up)

    def setup_canvas(self):
        """
        Sets up the canvas for the grid.
        :return: None
        """
        self.canvas = tk.Canvas(self.root, width=self.canvas_size, height=self.canvas_size)
        self.canvas.grid(row=0, rowspan=self.grid_row_offset, column=0)

    def draw_grid(self):
        """
        Draws the grid on the canvas.
        :return: None
        """
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
        """
        Sets the base station icon on the grid.
        :return: None
        """
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
        """
        Sets up the touchscreen functionality.
        :return: None
        """
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<Motion>", self.on_hover)
        self.ball_selector.bind("<<ComboboxSelected>>", self.on_ball_select)
        self.canvas.tag_bind(self.base, "<Enter>", self.show_base_label)
        self.canvas.tag_bind(self.base, "<Leave>", self.hide_base_label)

    def create_buttons(self):
        """
        Creates the buttons for the GUI.
        :return: None
        """
        self.grid_position = tk.Label(self.root, text="")
        self.grid_position.grid(row=self.grid_row_offset + 2, column=0)

        self.ball_selector_value = tk.StringVar()
        self.ball_selector = ttk.Combobox(self.root, textvariable=self.ball_selector_value,
                                          values=[ball.name for ball in self.balls], state='readonly')
        self.ball_selector.grid(row=self.grid_row_offset + 3, column=0, pady=(10, 20))
        self.ball_selector.set('Select a ball')

        delete_button = tk.Button(self.root, text="Disconnect Ball", command=self.delete_selected_ball)
        delete_button.grid(row=self.grid_row_offset + 4, column=0, padx=(0, 350))

        forward_button = tk.Button(self.root, text="Forward", command=self.move_forward)
        forward_button.grid(row=self.grid_row_offset + 4, column=0, padx=(0, 100))

        backward_button = tk.Button(self.root, text="Backward", command=self.move_backward)
        backward_button.grid(row=self.grid_row_offset + 4, column=0, padx=(100, 0))

        stop_button = tk.Button(self.root, text="Stop", command=self.stop_movement)
        stop_button.grid(row=self.grid_row_offset + 4, column=0, padx=(300, 0))

        reboot_button = tk.Button(self.root, text="Reboot", command=self.reboot)
        reboot_button.grid(row=self.grid_row_offset + 5, column=0, padx=(0, 400))

        find_available_button = tk.Button(self.root, text="Find Available", command=self.find_available)
        find_available_button.grid(row=self.grid_row_offset + 5, column=0, padx=(0, 150))

        turn_left_button = tk.Button(self.root, text="Turn Left", command=self.turn_left)
        turn_left_button.grid(row=self.grid_row_offset + 5, column=0, padx=(100, 0))

        turn_right_button = tk.Button(self.root, text="Turn Right", command=self.turn_right)
        turn_right_button.grid(row=self.grid_row_offset + 5, column=0, padx=(325, 0))

        self.keyboard_button = tk.Button(self.root, text="Enable Keyboard Mode", command=self.toggle_keyboard)
        self.keyboard_button.grid(row=self.grid_row_offset + 6, column=0, padx=(0, 300))

        remove_target_button = tk.Button(self.root, text="Remove Target", command=self.remove_target)
        remove_target_button.grid(row=self.grid_row_offset + 6, column=0, padx=(25, 0))

        move_to_target_button = tk.Button(self.root, text="Move To Target", command=self.move_to_target)
        move_to_target_button.grid(row=self.grid_row_offset + 6, column=0, padx=(300, 0))

        max_speed_label = tk.Label(self.root, text="Max Speed:")
        max_speed_label.grid(row=self.grid_row_offset + 8, column=0, padx=(0, 150))

        self.max_speed_value = tk.IntVar()
        self.max_speed_slider = tk.Scale(self.root, from_=0, to=100, orient="horizontal",
                                         variable=self.max_speed_value,
                                         command=self.on_max_speed_update)
        self.max_speed_slider.grid(row=self.grid_row_offset + 8, column=0, padx=(50, 0), pady=(0, 20))
        self.max_speed_slider.set(100)

        imu_header = tk.Label(self.root, text=f"\t\tIMU Data:\t\t")
        imu_header.grid(row=1, column=1)

    def canvas_to_grid_coords(self, x, y):
        """
        Converts canvas coordinates to grid coordinates.
        :param x: The x coordinate on the canvas
        :param y: The y coordinate on the canvas
        :return: Tuple of grid coordinates
        """
        x_grid = x - self.canvas_size // 2
        y_grid = -1 * y + self.canvas_size // 2
        return x_grid, y_grid

    def grid_to_canvas_coords(self, x, y):
        """
        Converts grid coordinates to canvas coordinates.
        :param x: The x coordinate on the grid
        :param y: The y coordinate on the grid
        :return: Tuple of canvas coordinates
        """
        x_canvas = x + self.canvas_size // 2
        y_canvas = -1 * y + self.canvas_size // 2
        return x_canvas, y_canvas

    def is_in_grid(self, x, y):
        """
        Checks if the coordinates are in the grid.
        :param x: The x coordinate
        :param y: The y coordinate
        :return: Boolean
        """
        if (x < self.padding or x > self.canvas_size - self.padding or
                y < self.padding or y > self.canvas_size - self.padding):
            return False
        else:
            return True

    def show_position_label(self, ball):
        """
        Shows the position label of the ball.
        :param ball: The ball object
        :return: None
        """
        if ball.position_label:
            self.canvas.itemconfig(ball.position_label, state="normal")

    def hide_position_label(self, ball):
        """
        Hides the position label of the ball.
        :param ball: The ball object
        :return: None
        """
        if ball.position_label:
            self.canvas.itemconfig(ball.position_label, state="hidden")

    def show_base_label(self, event):
        """
        Shows the base label.
        :param event: The hover event
        :return: None
        """
        self.canvas.itemconfig(self.base_label, state="normal")

    def hide_base_label(self, event):
        """
        Hides the base label.
        :param event: The hover event
        :return: None
        """
        self.canvas.itemconfig(self.base_label, state="hidden")

    def tag_bind_position_label(self, ball):
        """
        Sets the hover functionality for the ball position label.
        :param ball: The ball object
        :return: None
        """
        self.canvas.tag_bind(ball.gui_obj, "<Enter>",
                             lambda event, sel_ball=ball: self.show_position_label(sel_ball))
        self.canvas.tag_bind(ball.gui_obj, "<Leave>",
                             lambda event, sel_ball=ball: self.hide_position_label(sel_ball))

    def on_hover(self, event):
        """
        Shows the grid coordinates of the cursor.
        :param event: The hover event
        :return: None
        """
        if self.is_in_grid(event.x, event.y):
            grid_x, grid_y = self.canvas_to_grid_coords(event.x, event.y)
            self.grid_position.config(text=f"Cursor location: ({grid_x:.2f}, {grid_y:.2f})")
        else:
            self.grid_position.config(text="Cursor not in grid")

    def on_click(self, event):
        """
        Handles the click event on the canvas. If a ball is selected it will be placed on the grid.
        If a ball is selected and is already placed on the grid, the target location will be set.
        :param event: The click event
        :return: None
        """
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
                                                             text=f"{self.selected_ball.name}\n({grid_x:.2f},"
                                                                  f" {grid_y:.2f})",
                                                             anchor="n", state="hidden")
                    self.selected_ball.set_position_label(position_label)

                    # Set hover functionality to show and hide the ball position label
                    self.tag_bind_position_label(self.selected_ball)

                    # Update believes about current position
                    self.selected_ball.x_pos = grid_x
                    self.selected_ball.y_pos = grid_y

                    # Send initial position to ball
                    self.init_position(data)

                    # Remove first placement warning after
                    if self.initial_ball_warning:
                        self.initial_ball_warning.destroy()
                    logging.info(f"[GUI] Set initial ball location to: ({grid_x:.2f}, {grid_y:.2f})")
                else:
                    if self.selected_ball.has_target_location:
                        logging.warning(f"[GUI] Cannot override existing target location for {self.selected_ball.name}")
                        return
                    if self.keyboard_mode:
                        logging.warning(f"[GUI] Cannot set target location whehn keyboard mode is enabled.")
                        return

                    # Set target location
                    self.selected_ball.has_target_location = True
                    self.selected_ball.target_x_pos = grid_x
                    self.selected_ball.target_y_pos = grid_y

                    # Draw red cross at target location
                    self.selected_ball.set_target_object((
                        self.canvas.create_line(event.x - 10, event.y, event.x + 10, event.y, fill="red"),
                        self.canvas.create_line(event.x, event.y - 10, event.x, event.y + 10, fill="red")))

                    logging.info(f"[GUI] Set ball target location to: ({grid_x:.2f}, {grid_y:.2f})")
        else:
            logging.debug("[GUI] Selection not in grid")

    def toggle_keyboard(self):
        """
        Toggles the keyboard mode. If the keyboard mode is enabled, the user can control the ball with the arrow keys.
        :return: None
        """
        if self.keyboard_mode:
            self.keyboard_mode = False
            self.keyboard_button.config(text="Enable Keyboard Mode")
            for widget in self.root.winfo_children():
                if widget != self.keyboard_button:
                    widget.configure(state="normal")
        else:
            if self.selected_ball:
                self.keyboard_mode = True
                self.keyboard_button.config(text="Disable Keyboard Mode")
                for widget in self.root.winfo_children():
                    if widget != self.keyboard_button:
                        widget.configure(state="disabled")
            else:
                logging.warning("[GUI] Keyboard mode can not be enabled, ball is not selected.")

    def on_key_press(self, event):
        """
        Handles the key press event. If the keyboard mode is enabled, the ball will move in the direction of the key.
        :param event: The key press event
        :return: None
        """
        if self.keyboard_mode and not self.active_key:
            if event.keysym == 'Up':
                self.active_key = event.keysym
                self.move_forward()
            elif event.keysym == 'Down':
                self.active_key = event.keysym
                self.move_backward()
            elif event.keysym == 'Left':
                self.active_key = event.keysym
                self.turn_left()
            elif event.keysym == 'Right':
                self.active_key = event.keysym
                self.turn_right()

    def on_key_release(self, event):
        """
        Handles the key release event. If the keyboard mode is enabled, the ball will stop moving.
        :param event: The key release event
        :return: None
        """
        if self.keyboard_mode and event.keysym == self.active_key:
            self.active_key = None
            self.stop_movement()

    def on_ball_select(self, event):
        """
        Handles the ball selection event. If a ball is selected, the GUI will be updated.
        :param event: The ball selection event
        :return: None
        """
        # Remove the first placement warning when selecting other ball
        if self.initial_ball_warning:
            self.initial_ball_warning.destroy()

        selected_ball = self.ball_selector_value.get()
        logging.info(f"[GUI] Selected ball: {selected_ball}")

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
                    self.initial_ball_warning.grid(row=11, column=0)
                return

    def on_max_speed_update(self, event):
        """
        Handles the max speed update event. If a ball is selected, the max speed right will be updated.
        :param event: The max speed slider event
        :return: None
        """
        if self.selected_ball and self.selected_ball.gui_obj:
            new_max_speed = self.max_speed_value.get()
            self.selected_ball.max_speed = new_max_speed

    def update_state(self, ball_name, state_update, create_ball=False):
        """
        Updates the state of the ball. If the ball is not yet registered, it will be registered. If the ball is
        registered, the GUI will be updated.
        TODO: Move ball creation to register_ball function and refactor
        :param ball_name: The name of the ball
        :param state_update: The state update
        :param create_ball: True if the ball should be created
        :return: None
        """
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

                    logging.info(
                        f"[GUI] New state of {ball_name} is x={ball.x_pos}, y={ball.y_pos}, yaw={ball.rotation},"
                        f" pitch={ball.pitch}, roll={ball.roll}, {ball.cur_action}")
                elif create_ball and get_action_from_value(state_update['action']) != ActionType.INIT:
                    canvas_x, canvas_y = self.grid_to_canvas_coords(state_update['x'], state_update['y'])

                    if ball.gui_obj is None:
                        # Create ball object for the first time
                        ball_obj = self.canvas.create_oval(canvas_x - 10,
                                                           canvas_y - 10,
                                                           canvas_x + 10,
                                                           canvas_y + 10,
                                                           fill=BALL_COLOR)
                        ball.set_gui_object(ball_obj)

                        # Create position marker for the first time
                        position_label = self.canvas.create_text(canvas_x, canvas_y + 15,
                                                                 text=f"{ball.name}\n({state_update['x']:.2f},"
                                                                      f" {state_update['y']:.2f})",
                                                                 anchor="n", state="hidden")
                        ball.set_position_label(position_label)

                        # Set hover functionality to show and hide the ball position label
                        self.tag_bind_position_label(ball)

                        # Update believes about current position
                        ball.x_pos = state_update['x']
                        ball.y_pos = state_update['y']

                if ball_name in self.imu_labels:
                    yaw = state_update['rotation']
                    try:
                        pitch = state_update['pitch']
                        roll = state_update['roll']
                        imu_label = self.imu_labels[ball_name]
                        imu_label.config(
                            text=f"{ball_name} - Yaw: {yaw}, Pitch: {pitch}, Roll: {roll}\n"
                                 f"\tSpeed: -, Accel: -"
                        )
                    except KeyError:
                        logging.warning("State update does not contain Pitch or Roll")

                return

    def register_ball(self, ball_id):
        """
        Registers a new ball. If the ball is already registered, a warning will be logged.
        :param ball_id: The id of the ball
        :return: None
        """
        # If ball not yet registered
        if all(ball_id != ball.id for ball in self.balls):
            registered_ball = Ball(ball_id)
            registered_ball.set_gui_object(None)

            self.balls.append(registered_ball)
            self.ball_selector['values'] = [ball.name for ball in self.balls]
            self.mqtt_connector.subscribe(registered_ball.name + "/state")

            imu_label = tk.Label(self.root, text=f"{registered_ball.name} - Yaw: -, Pitch: -, Roll: -\n"
                                                 f"\tSpeed: -, Accel: -")
            imu_label.grid(row=len(self.balls) + 1, column=1)
            self.imu_labels[registered_ball.name] = imu_label

        else:
            logging.warning("[GUI] Duplicate ball registration: " + str(ball_id))

    def delete_selected_ball(self):
        """
        Deletes the selected ball. If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            self.delete_ball(self.selected_ball)
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def deregister_ball(self, ball_name):
        """
        Deregisters a ball.
        :param ball_name: The name of the ball
        :return: True if the ball was deregistered, False otherwise
        """
        for ball in self.balls:
            if ball.name == ball_name:
                self.delete_ball(ball)
                return True
        return False

    def delete_ball(self, ball):
        """
        Deletes a ball. Deletes the ball from the GUI and unsubscribes from the MQTT topic.
        :param ball: The ball object
        :return: None
        """
        if ball.name in self.imu_labels:
            # Get imu label of the ball
            imu_label = self.imu_labels[ball.name]

            # Get current row
            deleted_imu_label_row = imu_label.grid_info()['row']

            # Delete entry from list
            imu_label.grid_forget()
            self.imu_labels.pop(ball.name)

            # Move all other labels up
            self.shift_imu_labels(deleted_imu_label_row)

        # If ball is placed remove all GUI components
        if ball.gui_obj:
            self.canvas.delete(ball.gui_obj)
            self.canvas.delete(ball.position_label)

        # If ball had move objective remove all GUI components
        if ball.has_target_location:
            self.canvas.delete(ball.target_obj[0])
            self.canvas.delete(ball.target_obj[1])
            ball.target_obj = None
            ball.has_target_location = False

        # If ball had a known direction remove the arrow GUI component
        if ball.direction_obj:
            self.canvas.delete(ball.direction_obj)

        # Remove internal data of baball123123ll
        self.balls.remove(ball)
        self.mqtt_connector.unsubscribe(ball.name + "/state")
        self.disconnect()

        # Refresh ball selector list
        self.ball_selector['values'] = [ball.name for ball in self.balls]

        # Reset GUI components if deleted ball was currently selected
        if self.selected_ball and ball.name == self.selected_ball.name:
            if self.initial_ball_warning:
                self.initial_ball_warning.destroy()
            self.ball_selector.set('Select a ball')
            self.max_speed_slider.set(100)
            self.selected_ball = None

        logging.info(f"[GUI] Disconnected ball: {ball.name}")

    def turn_left(self):
        """
        Turns the selected ball left. If the ball has a target location, a warning will be logged.
        If no ball is selected, a warning will be logged.
        :return: None
        """
        self.single_movement_action(ActionType.TURN_LEFT)

    def turn_right(self):
        """
        Turns the selected ball right. If the ball has a target location, a warning will be logged.
        If no ball is selected, a warning will be logged.
        :return: None
        """
        self.single_movement_action(ActionType.TURN_RIGHT)

    def move_forward(self):
        """
        Moves the selected ball forward. If the ball has a target location, a warning will be logged.
        If no ball is selected, a warning will be logged.
        :return: None
        """
        self.single_movement_action(ActionType.FORWARD)

    def move_backward(self):
        """
        Moves the selected ball backward. If the ball has a target location, a warning will be logged.
        If no ball is selected, a warning will be logged.
        :return: None
        """
        self.single_movement_action(ActionType.BACKWARD)

    def single_movement_action(self, action_type):
        """
        Performs a single movement action. If the ball has a target location, a warning will be logged.
        :param action_type: The action type
        :return: None
        """
        if self.selected_ball:
            if self.selected_ball.has_target_location:
                logging.warning(f"[GUI] Action not permitted because {self.selected_ball.name} has a target")
                return
            speed = self.max_speed_value.get()
            data = {'speed': speed}
            self.selected_ball.action(action_type, self.mqtt_connector, data)
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def move_to(self, data):
        """
        Moves the selected ball to the target location. If the ball has a target location, a warning will be logged.
        If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            data['speed'] = self.max_speed_value.get()
            self.selected_ball.action(ActionType.MOVETO, self.mqtt_connector, data)
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def init_position(self, data):
        """
        Initializes the position of the selected ball. If no ball is selected, a warning will be logged.
        :param data: The data containing the position
        :return: None
        """
        if self.selected_ball:
            self.selected_ball.action(ActionType.INIT, self.mqtt_connector, data)
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def disconnect(self):
        """
        Disconnects the selected ball. If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            return
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def reboot(self):
        """
        Reboots the selected ball. If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            self.selected_ball.action(ActionType.REBOOT, self.mqtt_connector)
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def find_available(self):
        """
        Finds the available balls. Balls will send a registration message to the MQTT broker.
        :return: None
        """
        self.mqtt_connector.find_available_balls(self.client_id)

    def remove_target(self):
        """
        Removes the target location of the selected ball. If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            if self.selected_ball.has_target_location:
                self.stop_movement()
                self.canvas.delete(self.selected_ball.target_obj[0])
                self.canvas.delete(self.selected_ball.target_obj[1])
                self.selected_ball.target_obj = None
                self.selected_ball.has_target_location = False
                self.selected_ball.target_x_pos = None
                self.selected_ball.target_y_pos = None
            else:
                logging.error("[GUI] Action ignored, selected ball has no target location.")
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def move_to_target(self):
        """
        Moves the selected ball to the target location. If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            if self.selected_ball.has_target_location:
                self.move_to({'x': self.selected_ball.target_x_pos, 'y': self.selected_ball.target_y_pos})
            else:
                logging.error("[GUI] Action ignored, selected ball has no target location.")
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def stop_movement(self):
        """
        Stops the movement of the selected ball. If no ball is selected, a warning will be logged.
        :return: None
        """
        if self.selected_ball:
            self.selected_ball.action(ActionType.STOP, self.mqtt_connector)
        else:
            logging.error("[GUI] Action ignored, ball is not selected.")

    def shift_imu_labels(self, deleted_imu_label_row):
        """
        Shifts all imu labels up by one row.
        :param deleted_imu_label_row: The row of the deleted imu label
        :return: None
        """
        for key, _ in self.imu_labels.items():
            imu_label_row = self.imu_labels[key].grid_info()['row']
            if deleted_imu_label_row < imu_label_row:
                self.imu_labels[key].grid_forget()
                self.imu_labels[key].grid(row=imu_label_row - 1, column=1)
