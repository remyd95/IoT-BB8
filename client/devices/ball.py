from devices.action_type import ActionType
from devices.objective_type import ObjectiveType

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
        self.action = None
        self.objective = None
        self.max_duty_cycle = 100
        self.duty_cycle = None
        self.x_pos = None
        self.y_pos = None

        # IMU data
        self.rotation = None
        self.pitch = None
        self.roll = None
        self.speed = None
        self.acceleration = None
        self.total_displacement = None

        # Target location
        self.has_target_location = False
        self.target_x_pos = None
        self.target_y_pos = None

        # MQTT data
        self.topic = self.name + "/objective"

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

    def set_objective(self, objective_type, mqtt_connector, data=None):
        """
        Performs the given objective on the ball. The objective is performed by sending a message to the ball.
        :param objective_type: The objective type.
        :param mqtt_connector: The MQTT connector.
        :param data: The data to send to the ball.
        :return: None
        """
        if objective_type == ObjectiveType.FORWARD:
            mqtt_connector.publish(self.topic, f"FW {float(data['max_duty_cycle'])}")
        elif objective_type == ObjectiveType.BACKWARD:
            mqtt_connector.publish(self.topic, f"BW {float(data['max_duty_cycle'])}")
        elif objective_type == ObjectiveType.TURN_LEFT:
            mqtt_connector.publish(self.topic, f"TL {float(data['max_duty_cycle'])}")
        elif objective_type == ObjectiveType.TURN_RIGHT:
            mqtt_connector.publish(self.topic, f"TR {float(data['max_duty_cycle'])}")
        elif objective_type == ObjectiveType.STOP:
            mqtt_connector.publish(self.topic, "ST")
        elif objective_type == ObjectiveType.MOVETO:
            mqtt_connector.publish(self.topic, f"MT {data['x']} {data['y']} {float(data['max_duty_cycle'])}")
        elif objective_type == ObjectiveType.REBOOT:
            mqtt_connector.publish(self.topic, "RB")
        elif objective_type == ObjectiveType.INIT:
            mqtt_connector.publish(self.topic, f"IN {data['x']} {data['y']}")
