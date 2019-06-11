# IoT-System-with-ESP32

A simple project to connect your ESP32 with MySQL database

In this project you will need a DHT11 sensor and a SG90 servo motor，microcontroller will upload temperature data and moisture data to your local MySQL database.  
You can set a specific temerature value to tell the servo how to work. Also you can choose which WiFi you want to connect.  

I'm still trying to get the database IP address from the serial monitor of arduino IDE. If you solved this problem, please leave a issue.


# All you need to prepared
1. [Arduino IDE](https://www.arduino.cc/en/Main/Software)
2. [USB to UART drive](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
3. Arduino WiFi library, DHT11 library, Servo library
4. [MySQL database](https://www.mysql.com/cn/downloads/)
5. In case you don't know how to manipulate SQL with terminal, please use MySQL Workbench
6. [Dr.Bell's MySQl_connector_Arduino](https://github.com/ChuckBell/MySQL_Connector_Arduino)
7. A little bit of C language coding, a bit of luck and some magic
