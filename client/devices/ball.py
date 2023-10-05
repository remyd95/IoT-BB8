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

        # MQTT data
        self.mqtt_connector = mqtt_connector
        self.topic = self.name + "/action"

        # GUI data
        self.gui_obj = None
        self.position_label = None

    def action(self, action_type, data=None):
        if action_type == ActionType.FORWARD:
            self.mqtt_connector.publish(self.topic, f"FW {data['speed']}")
            self.cur_action = ActionType.FORWARD
        elif action_type == ActionType.BACKWARD:
            self.mqtt_connector.publish(self.topic, f"BW {data['speed']}")
            self.cur_action = ActionType.BACKWARD
        elif action_type == ActionType.STOP:
            self.mqtt_connector.publish(self.topic, "ST")
            self.cur_action = None
        elif action_type == ActionType.MOVETO:
            self.mqtt_connector.publish(self.topic, f"MV {data['x']} {data['y']} {data['speed']}")
            self.cur_action = ActionType.MOVETO

    def set_gui_object(self, gui_obj):
        self.gui_obj = gui_obj

    def set_position_label(self, position_label):
        self.position_label = position_label