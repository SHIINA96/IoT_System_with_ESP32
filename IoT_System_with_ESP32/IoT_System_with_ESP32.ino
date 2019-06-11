#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <dht.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// Pin Define
#define LED 2
#define RELAY_PUMP 25
#define RELAY_FAN 4
#define Button_LED 26
#define Button_FAN 27
#define Button_PUMP 14

volatile byte LED_state;
volatile byte FAN_state;
volatile byte PUMP_state;
char* state = "";

// OTA Configure
const char* host = "esp32";
WebServer server(80);

// MUC Mac_address Configuration
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 22, en = 23, d4 = 15, d5 = 18, d6 = 19, d7 = 21;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// MySQL Insert Query
char INSERT_DATA_Temperature[] = "INSERT INTO Arduino.Temperature_Data (Sensor_Name, Temperature_Value) VALUES ('%s', %s)";
char query_Temperature[128]; //contain all the data from the upload query
char INSERT_DATA_Humidity[] = "INSERT INTO Arduino.Humidity_Data (Sensor_Name, Humidity_Value) VALUES ('%s', %s)";
char query_Humidity[128];
char INSERT_DATA_SoilMoisture[] = "INSERT INTO Arduino.Soil_Moisture_Data (Sensor_Name, Sensor_Value, Soil_State) VALUES ('%s','%s','%s')";
char query_SoilMoisture[128];
char temperature[10];
char humidity[10];
char Soil_Moisture_Value[10];


// MySQL Insert Query for Manual Control
char INSERT_DATA_LED_Manual[] = "INSERT INTO Arduino.Actuator (Actuator_Name, Actuator_State, Control_Type) VALUES ('%s','%s','%s')";
char query_LED_Manual[128];

char INSERT_DATA_FAN_Manual[] = "INSERT INTO Arduino.Actuator (Actuator_Name, Actuator_State, Control_Type) VALUES ('%s','%s','%s')";
char query_FAN_Manual[128];

char INSERT_DATA_PUMP_Manual[] = "INSERT INTO Arduino.Actuator (Actuator_Name, Actuator_State, Control_Type) VALUES ('%s','%s','%s')";
char query_PUMP_Manual[128];


// MySQL Select Query
char QUERY_LED[] = "select Actuator_State from Arduino.Actuator where Actuator_Name = 'LED' order by A_ID DESC LIMIT 1;";
char query_LED[128];
char QUERY_RELAY_PUMP[] = "select Actuator_State from Arduino.Actuator where Actuator_Name = 'PUMP' order by A_ID DESC LIMIT 1;";
char query_RELAY_PUMP[128];
char QUERY_RELAY_FAN[] = "select Actuator_State from Arduino.Actuator where Actuator_Name = 'FAN' order by A_ID DESC LIMIT 1;";
char query_RELAY_FAN[128];
//char QUERY_SERVO[] = "select Actuator_State from Arduino.Actuator where Actuator_Name = 'SERVO' order by A_ID DESC LIMIT 1;";
//char query_SERVO[128];

// Use this for WiFi instead of EthernetClient
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// DHT11 Configurations
dht DHT;
int DHT11_Pin = 13;
int chk = 0; 
String Temperature_Input = "";

// Soil Moisture Sensor Configurations
extern int val = 0; //value for storing moisture value 
extern int soilPin = 33;//Declare a variable for the soil moisture sensor 
extern int soilPower = 32;//Variable for Soil moisture Power
String Soil_State = "";
char SoilBuf[100];

//// Servo Configurations
//Servo servo1;
//static const int servoPin = 12;
int gate_state = 0;
//int input_state = 0;

// WiFi Configurations
int n = 0;//for wifi_Scan only
String wifi_state = "";
char ssid[100] = "";    // your SSID
char pass[100] = "";    // your SSID Password

// General variable to storge the input
String Received = "";  // storge input

// Switch Case Configurations
unsigned char Function = 1;
#define Beginning 1
#define Input_Temperature 2
#define Sensor 3
#define wifi_Scan 4
#define wifi_Connection 5
#define Manual_Control 6
#define Manual_Control_Fan 7
#define Manual_Control_Pump 8


void setup()
    {
        pinMode(LED,OUTPUT);
        pinMode(RELAY_PUMP,OUTPUT);
        pinMode(RELAY_FAN,OUTPUT);
        pinMode(Button_LED,INPUT);
        Serial.begin(115200); //setting boud rate
        // servo1.attach(servoPin);  //init servo
        Function = Beginning;
        Serial.println();
        Serial.println("Initiation sequins finish");
        // set up the LCD's number of columns and rows:
        lcd.begin(16, 2);
        // Print a message to the LCD.
        lcd.print(" IoT with ESP32");

        pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
        digitalWrite(soilPower, LOW);
    }

    
void loop() 
    { 
//        server.handleClient();       
        switch(Function)
            {              
                case Beginning :
                        Serial.println("System start");
                        lcd.setCursor(2,1);
                        lcd.print("System Start");                             
                        Function = wifi_Scan;
                    break;


                case wifi_Scan :
                      WiFi_Scan();                    
                    break;


                case wifi_Connection:
                        //String wifi_choose = "";
                        Serial.println("Please input CONNECT, SKIP or OTA");
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("1-CONNECT 2-SKIP");
                        lcd.setCursor(0,1);
                        lcd.print("3-OTA");
//                        for (int positionCounter = 0; positionCounter < 13; positionCounter++) 
//                            {
//                              // scroll one position left:
//                              lcd.scrollDisplayLeft();
//                              // wait a bit:
//                              delay(1000);
//                            }
                                       
                        while(Serial.available()==0)
                            {}
                        wifi_state = Serial.readString();
                        
                        if(wifi_state != "SKIP" && wifi_state != "CONNECT" && wifi_state != "OTA" )
                            {
                                Serial.println("Please try again");
                                Function = wifi_Connection;
                                break;                             
                            }
                            
                        if(wifi_state == "SKIP")
                            {
                                Serial.println("Skip Wi-Fi Connection");
                                delay(1000);
                                Function = Input_Temperature;
                                break;
                            }
                        
                        if(wifi_state == "CONNECT")
                            {                                                    
                                WiFi_Connection();
                                Function = Input_Temperature;                    
                            }
                        if(wifi_state == "OTA")
                            {
                                WiFi_Connection();
                                OTA_Function();
                                while(true)
                                    {
                                        server.handleClient(); 
                                    }  
                            }
                    
                    break;

                    
                case Input_Temperature : 
                        Database_Setup();
                        Serial.println("Enter the Temperature Setting:");
                        while(Serial.available()==0)  //wait for input
                            {}
                        Temperature_Input = Serial.readString();  //transmit the input to the virable
                        if(Temperature_Input != "RES")
                            {                        
                                Serial.println("Temperature setting: " + Temperature_Input);
                                delay(20);                    
                                Function = Sensor;
                            }
                        if(Temperature_Input == "RES")
                            {
                                Serial.println("System Reset");
                                delay(5000);
                                Function = Beginning;                         
                            }
                    break;

                case Sensor :                                         
                        while(digitalRead(Button_LED)==LOW && digitalRead(Button_FAN)==LOW && digitalRead(Button_PUMP)==LOW)
                            {
                                  lcd.clear();
                                
                                  //get soil moisture value from the function below and print it
                                  Serial.print("Soil Moisture = "+ Soil_State);
                                  Serial.println(readSoil());
                                
                                  Soil_State.toCharArray(SoilBuf,50);
                                  DHT11();
                                  lcd.setCursor(0,0);
                                  lcd.print(String("Temp=") + String(DHT.temperature) + String("C"));
                                  lcd.setCursor(0,1);
                                  lcd.print(String("Humi=") + String(DHT.humidity) + String("%rh"));
                                  
                                  Data_Upload();
                                  Remote_Control(query_LED, QUERY_LED, LED);
                                  Remote_Control(query_RELAY_PUMP, QUERY_RELAY_PUMP, RELAY_PUMP);
                                  Remote_Control(query_RELAY_FAN, QUERY_RELAY_FAN, RELAY_FAN);
                                  
                                  
                                  //Remote_Control_SERVO(query_SERVO, QUERY_SERVO); 
                                  conn.close();
                                  
                                  if(Serial.available() > 0)
                                      {
                                          Function = Input_Temperature;
                                      }                           
                            }
                        if(digitalRead(Button_LED)!=LOW && digitalRead(Button_FAN)==LOW && digitalRead(Button_PUMP)==LOW)
                           {Function = Manual_Control;}
                        if(digitalRead(Button_LED)==LOW && digitalRead(Button_FAN)!=LOW && digitalRead(Button_PUMP)==LOW)
                           {Function = Manual_Control_Fan;}
                        if(digitalRead(Button_LED)==LOW && digitalRead(Button_FAN)==LOW && digitalRead(Button_PUMP)!=LOW)
                           {Function = Manual_Control_Pump;}
                    break;

                    
                case Manual_Control:
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Manual Control");
                        
                        LED_state = digitalRead(LED);
                        LED_state = !LED_state;

                        lcd.setCursor(0,1);
                        lcd.print(String("LED State:") + String(LED_state));
                        Serial.println("Manual Control");
                        //Serial.print("Interrupt");
                        digitalWrite(LED,LED_state);
                        if(LED_state == HIGH);
                            {
                                state = "ON";
                            }
                        if(LED_state == LOW)
                            {
                                state = "OFF";
                            }
                        LED_Manual_Control_Upload();
                        Function = Sensor;
                    break;

                case Manual_Control_Fan:
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Manual Control");
                        
                        FAN_state = digitalRead(RELAY_FAN);
                        FAN_state = !FAN_state;

                        lcd.setCursor(0,1);
                        lcd.print(String("FAN State:") + String(FAN_state));
                        Serial.println("Manual Control");
                        //Serial.print("Interrupt");
                        digitalWrite(RELAY_FAN,FAN_state);
                        if(FAN_state == HIGH);
                            {
                                state = "ON";
                            }
                        if(FAN_state == LOW)
                            {
                                state = "OFF";
                            }
                        FAN_Manual_Control_Upload();
                        Function = Sensor;
                    break;
                    
                case Manual_Control_Pump:
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Manual Control");
                        
                        PUMP_state = digitalRead(RELAY_PUMP);
                        PUMP_state = !PUMP_state;

                        lcd.setCursor(0,1);
                        lcd.print(String("PUMP State:") + String(PUMP_state));
                        Serial.println("Manual Control");
                        //Serial.print("Interrupt");
                        digitalWrite(RELAY_PUMP,PUMP_state);
                        if(PUMP_state == HIGH);
                            {
                                state = "ON";
                            }
                        if(PUMP_state == LOW)
                            {
                                state = "OFF";
                            }
                        PUMP_Manual_Control_Upload();
                        Function = Sensor;
                    break;
            }
    }
