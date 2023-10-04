import re

import paho.mqtt.client as mqtt


class MQTTClient:

    def __init__(self, mqtt_broker, mqtt_port, mqtt_keepalive):
        # General MQTT config
        self.broker = mqtt_broker
        self.port = mqtt_port
        self.client = mqtt.Client()
        self.keepalive = mqtt_keepalive

        # MQTT callbacks
        self.client.on_connect = self.on_mqtt_connect
        self.client.on_message = self.on_mqtt_message

        # Handlers
        self.state_handler = None
        self.register_handler = None

    def set_register_handler(self, handler):
        self.register_handler = handler

    def set_state_handler(self, handler):
        self.state_handler = handler

    def on_mqtt_connect(self, client, userdata, flags, rc):
        print("[MQTT] Connected to broker with result code " + str(rc))

    def on_mqtt_message(self, client, userdata, msg):
        if msg.topic == "register":
            if self.register_handler:
                payload = msg.payload.decode('utf-8')
                pattern = r"^\d{6}$"
                if re.match(pattern, payload):
                    ball_id = payload
                    self.register_handler(ball_id, self)
                else:
                    print("Invalid payload format: Expected ball id of 6 digits.")
        elif msg.topic.startswith("ball") and msg.topic.endswith("state"):
            ball_id = msg.topic.split('/')[0]
            payload_numbers = msg.payload.decode('utf-8').split()

            if len(payload_numbers) == 3:
                try:
                    x = float(payload_numbers[0])
                    y = float(payload_numbers[1])
                    action = int(payload_numbers[2])

                    state_update = {'x': x, 'y': y, 'action': action}
                    self.state_handler(ball_id, state_update)
                except ValueError:
                    print("Invalid payload format: Unable to convert to numbers.")
            else:
                print("Invalid payload format: Expected two space-separated numbers.")

    def connect(self):
        self.client.connect(self.broker, self.port, self.keepalive)
        self.client.loop_start()
        self.subscribe("register")

    def unsubscribe(self, mqtt_topic):
        print(f"[MQTT] Unsubscribed from {mqtt_topic}")
        self.client.unsubscribe(mqtt_topic)

    def subscribe(self, mqtt_topic):
        print(f"[MQTT] Subscribed to {mqtt_topic}")
        self.client.subscribe(mqtt_topic)

    def publish(self, mqtt_topic, message):
        print(f"[MQTT] Sent {message} to {mqtt_topic}")
        self.client.publish(mqtt_topic, message)
