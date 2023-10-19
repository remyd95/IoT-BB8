from devices.action_type import ActionType


class Ball:

    def __init__(self, ball_id, mqtt_connector):
        # Ball identification
        self.id = ball_id
        self.name = "ball" + str(ball_id)

        # Internal state
        self.cur_action = None
        self.max_speed = 100
        self.x_pos = None
        self.y_pos = None
        self.rotation = None
        self.has_target_location = False

        # MQTT data
        self.mqtt_connector = mqtt_connector
        self.topic = self.name + "/action"

        # GUI data
        self.position_label = None
        self.gui_obj = None
        self.target_obj = None
        self.direction_obj = None

    def set_gui_object(self, gui_obj):
        self.gui_obj = gui_obj

    def set_target_object(self, target_obj):
        self.target_obj = target_obj

    def set_direction_object(self, direction_obj):
            self.direction_obj = direction_obj

    def set_position_label(self, position_label):
        self.position_label = position_label

    def action(self, action_type, data=None):
        if action_type == ActionType.FORWARD:
            self.mqtt_connector.publish(self.topic, f"FW {float(data['speed'])}")
        elif action_type == ActionType.BACKWARD:
            self.mqtt_connector.publish(self.topic, f"BW {float(data['speed'])}")
        elif action_type == ActionType.STOP:
            self.mqtt_connector.publish(self.topic, "ST")
        elif action_type == ActionType.REBOOT:
            self.mqtt_connector.publish(self.topic, "RB")
        elif action_type == ActionType.FIND_AVAILABLE:
            self.mqtt_connector.publish(self.topic, f"FA {data['client_id']}")
        elif action_type == ActionType.MOVETO:
            self.mqtt_connector.publish(self.topic, f"MV {data['x']} {data['y']} {float(data['speed'])}")
        elif action_type == ActionType.INIT:
            self.mqtt_connector.publish(self.topic, f"IN {data['x']} {data['y']}")