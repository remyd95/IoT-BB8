import paho.mqtt.client as mqtt


class MQTTClient:

    def __init__(self, mqtt_broker, mqtt_port, mqtt_keepalive):
        self.broker = mqtt_broker
        self.port = mqtt_port
        self.client = mqtt.Client()
        self.keepalive = mqtt_keepalive

    def connect(self):
        self.client = mqtt.Client()
        self.client.connect(self.broker, self.port, self.keepalive)
        self.client.loop_start()

    def publish(self, mqtt_topic, message):
        self.client.publish(mqtt_topic, message)
