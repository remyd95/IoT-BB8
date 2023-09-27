import tkinter as tk

from connector.mqtt import MQTTClient
from devices.controller import Controller
from gui.controlpanel import ControlPanel


MQTT_BROKER = "duijsens.dev"
MQTT_PORT = 1883
MQTT_KEEPALIVE = 60


def main():
    print("Starting Client Application..")

    mqtt_connector = MQTTClient(MQTT_BROKER, MQTT_PORT, MQTT_KEEPALIVE)

    controller0 = Controller(0, mqtt_connector, x_pos=1, y_pos=0)
    controller1 = Controller(1, mqtt_connector, x_pos=-1, y_pos=0)
    controller2 = Controller(2, mqtt_connector, x_pos=0, y_pos=1)

    root = tk.Tk()

    control_panel = ControlPanel(root)

    control_panel.register_controller(controller0)
    control_panel.register_controller(controller1)
    control_panel.regisster_controller(controller2)

    root.mainloop()


if __name__ == '__main__':
    main()
