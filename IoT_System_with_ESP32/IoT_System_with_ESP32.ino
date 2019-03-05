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

// OTA Configure
#define LED 2
const char* host = "esp32";
WebServer server(80);

// MUC Mac_address Configuration
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 22, en = 23, d4 = 5, d5 = 18, d6 = 19, d7 = 21;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// MySQL Insert Query
char INSERT_DATA_Temperature[] = "INSERT INTO Arduino.Temperature_Data (Sensor_Name, Temperature_Value) VALUES ('%s', %s)";
char query_Temperature[128]; //contain all the data from the upload query
char INSERT_DATA_Humidity[] = "INSERT INTO Arduino.Humidity_Data (Sensor_Name, Humidity_Value) VALUES ('%s', %s)";
char query_Humidity[128];
char temperature[10];
char humidity[10];

// Use this for WiFi instead of EthernetClient
WiFiClient client;
MySQL_Connection conn((Client *)&client);

//DHT11 Configurations
dht DHT;
int DHT11_Pin = 13;
int chk = 0; 
String Temperature_Input = "";

//Servo Configurations
Servo servo1;
static const int servoPin = 12;
int gate_state = 0;
int input_state = 0;

//WiFi Configurations
int n = 0;//for wifi_Scan only
String wifi_state = "";
char ssid[100] = "";    // your SSID
char pass[100] = "";    // your SSID Password

// General variable to storge the input
String Received = "";  // storge input

//Switch Case Configurations
unsigned char Function = 1;
#define Beginning 1
#define Input_Temperature 2
#define DHT_Sensor 3
#define wifi_Scan 4
#define wifi_Connection 5


void setup()
    {
        Serial.begin(115200); //setting boud rate
        servo1.attach(servoPin);  //init servo
        Function = Beginning;
        Serial.println();
        Serial.println("Initiation sequins finish");
        // set up the LCD's number of columns and rows:
        lcd.begin(16, 2);
        // Print a message to the LCD.
        lcd.print(" IoT with ESP32");
    }

    
void loop() 
    { 
//        server.handleClient();       
        switch(Function)
            {              
                case Beginning :
                        Serial.println("System start");                                
                        Function = wifi_Scan;
                    break;


                case wifi_Scan :
                      WiFi_Scan();                    
                    break;


                case wifi_Connection:
                        //String wifi_choose = "";
                        Serial.println("Please input CONNECT, SKIP or OTA");                       
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
                                Function = DHT_Sensor;
                            }
                        if(Temperature_Input == "RES")
                            {
                                Serial.println("System Reset");
                                delay(5000);
                                Function = Beginning;                         
                            }
                    break;

                case DHT_Sensor :                                         
                        while(true)
                            {
                                  chk = DHT.read11(DHT11_Pin);
                                  //add .toInt() after the veriable to converting it from string type into a int type
                                  if(DHT.temperature >= Temperature_Input.toInt() && gate_state == 0) 
                                      {
                                          Gate_Open();
                                          gate_state = 1;       
                                      }
                                  if(DHT.temperature < Temperature_Input.toInt() && gate_state == 1)
                                      {
                                          Gate_Close();
                                          gate_state = 0;
                                      } 
                                  if(Serial.available() > 0)
                                      {
                                          Function = Input_Temperature;
                                          break;
                                      }
                                  Serial.print("Temperature="); //output will be here
                                  Serial.println(DHT.temperature);
                                  Serial.print("Humidity=");
                                  Serial.println(DHT.humidity);
                                  
                                  Data_Upload();                            
                            }            
                    break;                                    
            }
    }