import re
import threading
import time
import logging
logging.basicConfig(encoding='utf-8',
                    level=logging.DEBUG,
                    format='[%(asctime)s] [%(levelname)s] %(message)s',
                    datefmt='%H:%M:%S')

import paho.mqtt.client as mqtt

TIMEOUT_MS = 30000


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
        self.deregister_handler = None

        # Connection info
        self.last_state_update = {}

    def set_register_handler(self, handler):
        self.register_handler = handler

    def set_deregister_handler(self, handler):
        self.deregister_handler = handler

    def set_state_handler(self, handler):
        self.state_handler = handler

    def on_mqtt_connect(self, client, userdata, flags, rc):
        logging.info("[MQTT] Connected to broker with result code " + str(rc))

    def on_mqtt_message(self, client, userdata, msg):
        if msg.topic == "register":
            if self.register_handler:
                payload = msg.payload.decode('utf-8').split()
                pattern = r"^\d{6}$"

                if len(payload) >= 1:
                    ball_name = None
                    if re.match(pattern, payload[0]):
                        ball_id = payload[0]
                        ball_name = "ball" + str(ball_id)
                        self.last_state_update[ball_name] = time.time()
                        self.register_handler(ball_id)
                    if ball_name is not None and len(payload) == 5:
                        x = float(payload[1])
                        y = float(payload[2])
                        rotation = float(payload[3])
                        action = int(payload[4])
                        state_update = {'x': x, 'y': y, 'rotation': rotation, 'action': action}
                        self.state_handler(ball_name, state_update, create_ball=True)
                else:
                    logging.error("Invalid payload format: Expected ball id of 6 digits.")
        elif msg.topic.startswith("ball") and msg.topic.endswith("state"):
            ball_name = msg.topic.split('/')[0]
            payload = msg.payload.decode('utf-8').split()

            if len(payload) == 4:
                try:
                    x = float(payload[0])
                    y = float(payload[1])
                    rotation = float(payload[2])
                    action = int(payload[3])

                    state_update = {'x': x, 'y': y, 'rotation': rotation, 'action': action}

                    self.last_state_update[ball_name] = time.time()
                    self.state_handler(ball_name, state_update)
                except ValueError:
                    logging.error("Invalid payload format: Unable to convert to numbers.")
            else:
                logging.error("Invalid payload format: Expected 4 space-separated numbers.")

    def remove_inactive_balls(self):
        current_time = time.time()
        inactive_balls = []

        for ball_name, last_update_time in self.last_state_update.items():
            if current_time - last_update_time > TIMEOUT_MS // 1000:
                inactive_balls.append(ball_name)

        for ball_name in inactive_balls:
            del self.last_state_update[ball_name]
            if self.deregister_handler(ball_name):
                logging.info(f"[MQTT] Removing {ball_name} due to timeout (60s)")

    def start_inactive_ball_checker(self):
        while True:
            self.remove_inactive_balls()
            time.sleep(1)

    def connect(self):
        self.client.connect(self.broker, self.port, self.keepalive)
        self.client.loop_start()
        checker_thread = threading.Thread(target=self.start_inactive_ball_checker)
        checker_thread.daemon = True
        checker_thread.start()
        self.subscribe("register")

    def unsubscribe(self, mqtt_topic):
        logging.info(f"[MQTT] Unsubscribed from {mqtt_topic}")
        self.client.unsubscribe(mqtt_topic)

    def subscribe(self, mqtt_topic):
        logging.info(f"[MQTT] Subscribed to {mqtt_topic}")
        self.client.subscribe(mqtt_topic)

    def publish(self, mqtt_topic, message):
        logging.info(f"[MQTT] Sent {message} to {mqtt_topic}")
        self.client.publish(mqtt_topic, message)

    def find_available_balls(self, client_id):
        self.publish("id", str("FA ") + str(client_id))
