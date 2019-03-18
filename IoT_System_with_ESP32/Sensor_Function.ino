int DHT11(void)
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
//        if(Serial.available() > 0)
//            {
//                Function = Input_Temperature;
//            }
        Serial.print("Temperature="); //output will be here
        Serial.println(DHT.temperature);
        Serial.print("Humidity=");
        Serial.println(DHT.humidity);
    }

int readSoil()
    {
        digitalWrite(soilPower, HIGH);//turn D7 "On"
        delay(10);//wait 10 milliseconds 
        val = analogRead(soilPin);//Read the SIG value form sensor 
        //SparkFunRead = val;
        digitalWrite(soilPower, LOW);//turn D7 "Off"
        if(val>=0 && val<=300)
            {
                Soil_State = "Dry";
            }
        else if(val>300 && val<=700)
            {
                Soil_State = "Humid";
            }
        else if(val>700)
            {
                Soil_State = "Watery";
            }
        else
            {
                Soil_State = "Error";
            }
        return val;
    }
