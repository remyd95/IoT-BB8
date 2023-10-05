import tkinter as tk

from connector.mqtt import MQTTClient
from gui.control_panel import ControlPanel


MQTT_BROKER = "duijsens.dev"
MQTT_PORT = 1883
MQTT_KEEPALIVE = 60


def main():
    print("Starting Client Application..")

    mqtt_connector = MQTTClient(MQTT_BROKER, MQTT_PORT, MQTT_KEEPALIVE)
    mqtt_connector.connect()

    root = tk.Tk()

    control_panel = ControlPanel(root)

    mqtt_connector.set_register_handler(control_panel.register_ball)
    mqtt_connector.set_deregister_handler(control_panel.deregister_ball)
    mqtt_connector.set_state_handler(control_panel.update_state)

    root.mainloop()


if __name__ == '__main__':
    main()
