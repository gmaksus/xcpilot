# xcpilot
Autopilot for XPLANE flight simulator/
It is a small part of UAV project. To control the aircraft in the simulator usually a joystick is used. For training in the management of radio-controlled aircraft, the corresponding simulators are also used. To interface standard remote controllers with simulators, there is a PPJOY program that converts the input data stream of the RS232 interface, created according to certain rules, to virtual joystick control signals. The program also contains a driver, which is defined in the system as a joystick. Description of the program PPJOY can be found in free access on the Internet. Here you can find an application that can work with Win7.To receive data from X-Plane airplane sensors, the simulator is configured to output data over the network UDP protocol. To do this, it is necessary, as shown in the movie, to configure the simulator to issue data from the "sensors" of the aircraft through the network to the selected IP address and the specified port.The autopilot program is written in the Borland C ++ Builder 6(Ask me if needed). The "Devices" field (Figure below) shows the initial data from the sensors, which are updated 20 times per second. Here we see Pitch, which is calculated from the accelerometer in a radio-controlled plane, Roll, which is calculated from the longitudinally installed gyro, which measures the angular speed of the airplane's turn around Z(Yaw) axis. It should be noted here that the pitch in the simulator I'm here taking a real one, and not an accelerometer, although in a real airplane I calculate the pitch by the accelerometer, which means that the sharp acceleration during acceleration will "feel" the aircraft as an additive to the positive pitch. This is clearly seen in the video fragments of the tests: during acceleration, the aircraft immediately lifts its tail. This is the difference from reality. But the roll is really calculated by the angular speed of the turn of the aircraft.
Checkbox GPS includes a flight along the route recorded in the table. The Requested Heading, Requested Altitude, and Requested Speed fields are populated automatically from the structure:
typedef struct WPNT {int pid; int speed; int altitude; int heading; int latitude; int longitude; } WPNT;
In the demonstration, the structure contains an example of a route near the Los Angeles airport. (Figure below)
To change the route, you need to change the structure and recompile the project. If GPS is off, these fields can be filled manually. Activation of the value in the field occurs when the mouse is double-clicked.
https://www.rcgroups.com/forums/showthread.php?3081951-Autopilot-on-AT91SAM7-Simulation-of-autopilot
Исходный блог на русском языке: https://maksimuzz.blogspot.com/2018/01/at91sam7-4.html
