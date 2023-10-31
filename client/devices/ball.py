from devices.action_type import ActionType


class Ball:
    """
    Class representing a ball. Contains all the information about the ball.
    Also contains the methods to perform actions on the ball.
    """
    def __init__(self, ball_id):
        """
        Constructor for Ball. Initializes the ball with the given ID. Contains GUI data.
        :param ball_id: The ID of the ball.
        """
        # Ball identification
        self.id = ball_id
        self.name = "ball" + str(ball_id)

        # Internal state
        self.cur_action = None
        self.max_speed = 100
        self.x_pos = None
        self.y_pos = None

        # IMU data
        self.rotation = None
        self.pitch = None
        self.roll = None
        self.speed = None
        self.acceleration = None

        # Target location
        self.has_target_location = False
        self.target_x_pos = None
        self.target_y_pos = None

        # MQTT data
        self.topic = self.name + "/action"

        # GUI data
        self.position_label = None
        self.gui_obj = None
        self.target_obj = None
        self.direction_obj = None

    def set_gui_object(self, gui_obj):
        """
        Sets the GUI object of the ball.
        :param gui_obj: The GUI object.
        :return: None
        """
        self.gui_obj = gui_obj

    def set_target_object(self, target_obj):
        """
        Sets the target object of the ball.
        :param target_obj: The target object.
        :return: None
        """
        self.target_obj = target_obj

    def set_direction_object(self, direction_obj):
        """
        Sets the direction object of the ball.
        :param direction_obj: The direction object.
        :return: None
        """
        self.direction_obj = direction_obj

    def set_position_label(self, position_label):
        """
        Sets the position label of the ball.
        :param position_label: The position label.
        :return: None
        """
        self.position_label = position_label

    def action(self, action_type, mqtt_connector, data=None):
        """
        Performs the given action on the ball. The action is performed by sending a message to the ball.
        :param action_type: The action type.
        :param mqtt_connector: The MQTT connector.
        :param data: The data to send to the ball.
        :return: None
        """
        if action_type == ActionType.FORWARD:
            mqtt_connector.publish(self.topic, f"FW {float(data['speed'])}")
        elif action_type == ActionType.BACKWARD:
            mqtt_connector.publish(self.topic, f"BW {float(data['speed'])}")
        elif action_type == ActionType.TURN_LEFT:
            mqtt_connector.publish(self.topic, f"TL {float(data['speed'])}")
        elif action_type == ActionType.TURN_RIGHT:
            mqtt_connector.publish(self.topic, f"TR {float(data['speed'])}")
        elif action_type == ActionType.STOP:
            mqtt_connector.publish(self.topic, "ST")
        elif action_type == ActionType.REBOOT:
            mqtt_connector.publish(self.topic, "RB")
        elif action_type == ActionType.MOVETO:
            mqtt_connector.publish(self.topic, f"MV {data['x']} {data['y']} {float(data['speed'])}")
        elif action_type == ActionType.INIT:
            mqtt_connector.publish(self.topic, f"IN {data['x']} {data['y']}")
