import re
import threading
import time
import logging
import paho.mqtt.client as mqtt

# Logging config
logging.basicConfig(encoding='utf-8',
                    level=logging.DEBUG,
                    format='[%(asctime)s] [%(levelname)s] %(message)s',
                    datefmt='%H:%M:%S')

# MQTT config
TIMEOUT_MS = 30000


def on_mqtt_connect(client, userdata, flags, rc):
    """
    Callback for when the MQTT client connects to the broker. Logs the result code.
    :param client: The MQTT client.
    :param userdata: The userdata.
    :param flags: The flags.
    :param rc: The result code.
    :return: None
    """
    logging.info("[MQTT] Connected to broker with result code " + str(rc))


class MQTTClient:
    """
    MQTT client for communication with the MQTT broker. Handles all MQTT communication.
    """

    def __init__(self, mqtt_broker, mqtt_port, mqtt_keepalive):
        """
        Constructor for MQTTClient. Initializes the MQTT client and sets the callbacks.
        :param mqtt_broker: The MQTT broker to connect to.
        :param mqtt_port: The port of the MQTT broker.
        :param mqtt_keepalive: The keepalive time for the MQTT connection.
        """
        # General MQTT config
        self.broker = mqtt_broker
        self.port = mqtt_port
        self.client = mqtt.Client()
        self.keepalive = mqtt_keepalive

        # MQTT callbacks
        self.client.on_connect = on_mqtt_connect
        self.client.on_message = self.on_mqtt_message

        # Handlers
        self.state_handler = None
        self.register_handler = None
        self.deregister_handler = None

        # Connection info
        self.last_state_update = {}

    def set_register_handler(self, handler):
        """
        Sets the register handler. This handler is called when a new ball is registered.
        :param handler: The handler to set.
        :return: None
        """
        self.register_handler = handler

    def set_deregister_handler(self, handler):
        """
        Sets the deregister handler. This handler is called when a ball is deregistered.
        :param handler: The handler to set.
        :return: None
        """
        self.deregister_handler = handler

    def set_state_handler(self, handler):
        """
        Sets the state handler. This handler is called when a new state is received.
        :param handler: The handler to set.
        :return: None
        """
        self.state_handler = handler

    def on_mqtt_message(self, client, userdata, msg):
        """
        Callback for when a message is received. Calls the appropriate handler.

        - If the message is a state update, the last state update time is updated.
        - If the message is a registration message, the ball is registered.
        :param client: The MQTT client.
        :param userdata: The userdata.
        :param msg: The message.
        :return: None
        """

        # If the message is a registration message, call the register handler
        if msg.topic == "register":
            if self.register_handler:
                payload = msg.payload.decode('utf-8').split()
                pattern = r"^\d{6}$"

                # Check if the payload is valid
                if len(payload) >= 1:
                    ball_name = None

                    # Check if the payload is a valid ball id
                    if re.match(pattern, payload[0]):
                        ball_id = payload[0]
                        ball_name = "ball" + str(ball_id)
                        self.last_state_update[ball_name] = time.time()
                        self.register_handler(ball_id)

                    # Check if the payload is a valid ball name and the payload has 5 elements.
                    # We need this to be able to display the ball after a find available balls request.
                    if ball_name is not None and len(payload) == 5:
                        x = float(payload[1])
                        y = float(payload[2])
                        rotation = float(payload[3])
                        action = int(payload[4])
                        state_update = {'x': x, 'y': y, 'rotation': rotation, 'action': action}
                        self.state_handler(ball_name, state_update, create_ball=True)
                else:
                    logging.error("Invalid payload format: Expected ball id of 6 digits.")

        # If the message is a state update, update the last state update time and call the state handler
        elif msg.topic.startswith("ball") and msg.topic.endswith("state"):
            ball_name = msg.topic.split('/')[0]
            payload = msg.payload.decode('utf-8').split()

            if len(payload) == 10:
                try:
                    x = float(payload[0])
                    y = float(payload[1])
                    rotation = float(payload[2])
                    action = int(payload[3])
                    pitch = float(payload[4])
                    roll = float(payload[5])
                    speed = float(payload[6])
                    acceleration = float(payload[7])
                    duty_cycle = float(payload[8])
                    objective = int(payload[9])

                    state_update = {'x': x,
                                    'y': y,
                                    'rotation': rotation,
                                    'action': action,
                                    'pitch': pitch,
                                    'roll': roll,
                                    'speed': speed,
                                    'acceleration': acceleration,
                                    'duty_cycle': duty_cycle,
                                    'objective': objective
                                    }

                    self.last_state_update[ball_name] = time.time()
                    self.state_handler(ball_name, state_update)
                except ValueError:
                    logging.error("Invalid payload format: Unable to convert to numbers.")
            else:
                logging.error("Invalid payload format: Expected 10 space-separated values.")

    def remove_inactive_balls(self):
        """
        Removes inactive balls from the last state update dict. If a ball is inactive for more than TIMEOUT_MS,
        it is removed from the dict and the deregister handler is called.
        :return: None
        """
        current_time = time.time()
        inactive_balls = []

        # Find inactive balls and add them to the list for removal
        for ball_name, last_update_time in self.last_state_update.items():
            if current_time - last_update_time > TIMEOUT_MS // 1000:
                inactive_balls.append(ball_name)

        # Remove inactive balls from the dict and call the deregister handler
        for ball_name in inactive_balls:
            del self.last_state_update[ball_name]
            if self.deregister_handler(ball_name):
                logging.info(f"[MQTT] Removing {ball_name} due to timeout (60s)")

    def start_inactive_ball_checker(self):
        """
        Starts a thread that checks for inactive balls. If a ball is inactive for more than TIMEOUT_MS, it is
        removed from the last state update dict and the deregister handler is called.
        :return: None
        """
        while True:
            self.remove_inactive_balls()
            time.sleep(1)

    def connect(self):
        """
        Connects to the MQTT broker and starts the inactive ball checker thread.
        :return: None
        """
        self.client.connect(self.broker, self.port, self.keepalive)
        self.client.loop_start()
        checker_thread = threading.Thread(target=self.start_inactive_ball_checker)
        checker_thread.daemon = True
        checker_thread.start()
        self.subscribe("register")

    def unsubscribe(self, mqtt_topic):
        """
        Unsubscribes from the given topic.
        :param mqtt_topic: The topic to unsubscribe from.
        :return: None
        """
        logging.info(f"[MQTT] Unsubscribed from {mqtt_topic}")
        self.client.unsubscribe(mqtt_topic)

    def subscribe(self, mqtt_topic):
        """
        Subscribes to the given topic.
        :param mqtt_topic: The topic to subscribe to.
        :return: None
        """
        logging.info(f"[MQTT] Subscribed to {mqtt_topic}")
        self.client.subscribe(mqtt_topic)

    def publish(self, mqtt_topic, message):
        """
        Publishes a message to the given topic.
        :param mqtt_topic: The topic to publish to.
        :param message: The message to publish.
        :return: None
        """
        logging.info(f"[MQTT] Sent {message} to {mqtt_topic}")
        self.client.publish(mqtt_topic, message)

    def find_available_balls(self, client_id):
        """
        Sends a find available balls request to the MQTT broker.
        The balls that are available sent a registration message back.
        :param client_id: The id of the client that sends the request.
        :return: None
        """
        self.publish("id", str("FA ") + str(client_id))
