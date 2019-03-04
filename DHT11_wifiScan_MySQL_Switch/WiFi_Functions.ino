// This function will scan the Wi-Fi signals
 int WiFi_Scan(void)
    {
        Serial.println("Scan start");   
        //WiFi.scanNetworks will return the number of networks found
        //Define of 'n' is in the beginning
        n = WiFi.scanNetworks();
        Serial.println("Scan done");
        if (n == 0) 
            {
                Serial.println("No networks found");
            } 
        else 
            {
                Serial.print(n);
                Serial.println(" Networks found");
                for (int i = 0; i < n; ++i) 
                    {
                        // Print SSID and RSSI for each network found
                        Serial.print(i + 1);
                        Serial.print(": ");
                        Serial.print(WiFi.SSID(i));
                        Serial.print(" (");
                        Serial.print(WiFi.RSSI(i));
                        Serial.print(")");
                        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
                        delay(10);
                    }
            }
        Serial.println("");                   
        // Wait a bit before scanning again
        delay(1000);
        Function = wifi_Connection;
    }
// Wi-Fi scan section end


// This function will connect Wi-Fi
int WiFi_Connection(void)
    {
        
        Serial.println("Preparing Wi-Fi");
        Serial.println("Input your SSID");

        while(Serial.available()==0)  //wait for input
            {}
        Received = Serial.readString();
        Received.toCharArray(ssid, sizeof(Received)+1);
        Received = "";
            
        Serial.print("SSID:");
        Serial.println(ssid);

        Serial.println("Input your Password");
        while(Serial.available()==0)
            {}
        Received = Serial.readString();
        Received.toCharArray(pass, sizeof(Received)+1);
        Received = "";
        Serial.print("Password:");
        Serial.println(pass);
        
        Serial.println(ssid);
        Serial.println(pass);
        
        WiFi.begin(ssid,pass);    
        while (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.print(".");
            }
        
        Serial.println("");
        Serial.println("WiFi connected.");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println();
    }
// Wi-Fi connecting section end
