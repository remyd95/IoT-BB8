# IoT-BB8


## Repository Navigation

- /client - contains the control panel GUI
- /include - contains header files for the system firmware
- /lib - contains used library code, mainly drivers and i2c related code.
- /scripts - contains data and scripts for experiment analysis
- /src - contains the source files for the system firmware

  ## Control Panel

  To run the control panel:
  1. Install the requirements using ```pip3 install -r requirements.txt``` 
  2. Run the main.py file using ```python3 main.py```
 
  Balls will automatically register to the GUI when powered up. You can manually search for balls using the "Find Available" button.
  If a ball does not respond a reboot can be considered using the "Reboot" button.

  All other control options are related to the balls movement. The ball can either be controlled using the buttons available, or a target location can be selected be selecting a position in the grid.

  ## Firmware

  Firmware depends on Platformio. When importing this project in Platformio agree with the installation of the drivers.
  The driver selection is based on the model of the ESP that is used. The model can be changed in ```platformio.ini```

  The device can then be flashed using the Upload (or Upload and Monitor for additional debugging opportunity).

  When the device is powered it will automatically boot and try to connect to the defined WiFi gateway in ```/src/main.c```.
  The MQTT server can also be modified here. Note that the client needs to be modified as well when a new MQTT server is configured.

  Another consideration is the GPIO pin layout of the ESP. This is chip and sometimes even manufacturer specific. Edit the pins accordingly in  ```/src/main.c```, ```/include/imu.h```, and  ```/include/pwm_motor.h```.

  ## Analysis

  The scripts folder contains several analysis scripts.

  - ```angular_exp.py``` contains an experiment setup to create a polar plot of IMU and Real Positions with Curved Line
  - ```diff_estimation_real.py``` contains an experiment setup to calculate the difference between the actual path distance and the reported distances.
  - ```distance_deviation_analysis.py``` contains an experiment setup to find a relation between the reported distance and the actual distance the ball rolled.
