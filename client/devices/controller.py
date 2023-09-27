from devices.action_type import ActionType


class Controller:

    def __init__(self, controller_id, mqtt_connector, x_pos=0, y_pos=0):
        self.id = controller_id
        self.name = "controller" + controller_id
        self.topic = self.name + "/motor"
        self.action = None
        self.speed = 0.5
        self.cur_speed = 0
        self.x_pos = x_pos
        self.y_pos = y_pos
        self.mqtt_connector = mqtt_connector

    def action(self, tk_root, action_type, duration=-1):
        if action_type == ActionType.FORWARD:
            self.mqtt_connector.publish(self.topic, "FW")
            if duration > 0:
                tk_root.after(int(duration * 1000), lambda: self.action(tk_root, ActionType.STOP))
        elif action_type == ActionType.BACKWARD:
            self.mqtt_connector.publish(self.topic, "BW")
            if duration > 0:
                tk_root.after(int(duration * 1000), lambda: self.action(tk_root, ActionType.STOP))
        elif action_type == ActionType.STOP:
            self.mqtt_connector.publish(self.topic, "ST")

