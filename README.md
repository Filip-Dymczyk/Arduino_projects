# Arduino_projects
## This repo contains personal Arduino projects done to increase hands-on hardware and embedded programming knowledge.

LCD_LM35_TEMP project:
- LM35 analog thermometer used for reading temperature,
- 16x2 lcd control - displaying current temperature level,
- yellow, green and blue LED control using PWM outputs for displaying custom temperature levels.

LCD_LM35_TEMP_dc_motor project:
- extending previous project with DC motor control,
- using MOT-00949 DC motor controlled via L293D controller,
- push-button with a pull down resistor used for stopping/starting the motor.

DHT_11 project:
- test of a DHT11 digital temperature and humidity sensor done with external libraries.

RGB_diode_distance_sensor project:
- HC-SR04 ultrasound sensor used for distance measurement,
- RGB diode illumination control via above mentioned sensor.

RGB_diode_distance_sensor_dc_motor project:
- extending of previous project,
- added MOT-00949 DC motor speed control (via L293D controller) with ultrasound distance sensor,
- start/stop push-button (pulled down) for the motor with debouncing.
