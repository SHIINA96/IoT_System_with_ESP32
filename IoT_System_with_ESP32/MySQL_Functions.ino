// MySQL Connection Configuration
extern IPAddress server_addr(192,168,1,69);  // IP of the MySQL *server* here
// This IP address shuold be the same as your computer
extern char user[100] = "";              // MySQL user login username
extern char password[100] = "";        // MySQL user login password


// Input your MySQL port, username and password
int Database_Setup(void)
    {
        Serial.println("Input your MySQL username");
        while(Serial.available()==0)  //wait for input
            {}
        Received = Serial.readString();
        Received.toCharArray(user, sizeof(Received)+1);
        Received = "";
        Serial.print("user:");
        Serial.println(user);

        Serial.println("Input your MySQL password");
        while(Serial.available()==0)
            {}
        Received = Serial.readString();
        Received.toCharArray(password, sizeof(Received)+1);
        Received = "";
        Serial.print("Password:");
        Serial.println(password);
        
        Serial.println(user);
        Serial.println(password);   

        // Display connection detail
        Serial.println("Connecting to MySQL Database...");
        Serial.print("MySQL User: ");
        Serial.print(user);
        Serial.print("   Password: ");
        Serial.println(password);
        Serial.print("Database address: ");
        Serial.print(server_addr);
        Serial.println(":3306");
    }


// Insert Data into MySQL DataBase
int Data_Upload(void)
    {
        if (conn.connect(server_addr, 3306, user, password))
            {
                // Temperature & Humidity Uploading Sequence
                delay(100);
                // Insert data
                // Initiate the query class instance
                MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
                // Save
                dtostrf(DHT.temperature, 1, 1, temperature);
                dtostrf(DHT.humidity, 1, 1, humidity);
                dtostrf(readSoil(), 1, 1, Soil_Moisture_Value);
                sprintf(query_Temperature, INSERT_DATA_Temperature, "DHT11", temperature);
                sprintf(query_Humidity, INSERT_DATA_Humidity, "DHT11", humidity);
                sprintf(query_SoilMoisture, INSERT_DATA_SoilMoisture, "SparkFun", Soil_Moisture_Value, SoilBuf);
                // Execute the query
                cur_mem->execute(query_Temperature);
                //delete cur_mem;
                cur_mem->execute(query_Humidity);
                //delete cur_mem;
                cur_mem->execute(query_SoilMoisture);
                delete cur_mem;
                // Note: since there are no results, we do not need to read any data
                // Deleting the cursor also frees up memory used   
                Serial.println("Temeperature Data recorded.");
                delay(100);
                Serial.println("Humidity Data recorded.");
            }
        else
        Serial.println("Connection failed.");
        //conn.close();
    }

void LED_Manual_Control_Upload()
{
    if (conn.connect(server_addr, 3306, user, password))
        {
            // Temperature & Humidity Uploading Sequence
            delay(100);
            // Insert data
            // Initiate the query class instance
            MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
            sprintf(query_LED_Manual, INSERT_DATA_LED_Manual, "LED", state, "Manual");
            // Execute the query
            cur_mem->execute(query_LED_Manual);
        }
    else
    Serial.println("Connection failed.");
    conn.close();
}
// Insert section end


// Select Section
int Remote_Control (char* query, char* QUERY, int ACTUATOR)
{
        Serial.println("> Running SELECT with dynamically supplied parameter");
        
        // Initiate the query class instance
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        sprintf(query, QUERY, 9000000);
        
        // Execute the query
        cur_mem->execute(query);
        
        // Fetch the columns and print them
        column_names *cols = cur_mem->get_columns();
        for (int f = 0; f < cols->num_fields; f++) 
            {
                Serial.print(cols->fields[f]->name);
                if (f < cols->num_fields-1) 
                    {
                        Serial.print(',');
                    }
            }
        Serial.println();
        // Read the rows and print them
        row_values *row = NULL;
            do 
                {
                    row = cur_mem->get_next_row();
                    if (row != NULL) 
                        {
                            for (int f = 0; f < cols->num_fields; f++) 
                                {
                                    Serial.print(ACTUATOR);
                                    Serial.print(row->values[f]);
                                    // Serial.println( type(row->values[f]) );
                                    if (row->values[0][0]=='O' && row->values[0][1]=='N')
                                        {
                                            digitalWrite(ACTUATOR,HIGH);
                                        }
                                    if (row->values[0][0]=='O' && row->values[0][1]=='F' && row->values[0][2]=='F')
                                        {
                                            digitalWrite(ACTUATOR,LOW);
                                        }
                                    if (f < cols->num_fields-1) 
                                        {
                                            Serial.print(',');
                                        }
                                }
                            Serial.println();
                        }
                } 
            while (row != NULL);
            // Deleting the cursor also frees up memory used
            delete cur_mem;
}

int Remote_Control_SERVO (char* query, char* QUERY)
{
        Serial.println("> Running SELECT with dynamically supplied parameter");
        
        // Initiate the query class instance
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        sprintf(query, QUERY, 9000000);
        
        // Execute the query
        cur_mem->execute(query);
        
        // Fetch the columns and print them
        column_names *cols = cur_mem->get_columns();
        for (int f = 0; f < cols->num_fields; f++) 
            {
                Serial.print(cols->fields[f]->name);
                if (f < cols->num_fields-1) 
                    {
                        Serial.print(',');
                    }
            }
        Serial.println();
        // Read the rows and print them
        row_values *row = NULL;
            do 
                {
                    row = cur_mem->get_next_row();
                    if (row != NULL) 
                        {
                            for (int f = 0; f < cols->num_fields; f++) 
                                {
                                    Serial.print("SERVO ");
                                    Serial.print(row->values[f]);
                                    // Serial.println( type(row->values[f]) );
                                    if (row->values[0][0]=='O' && row->values[0][1]=='N')
                                        {
                                            for(int posDegrees = 0; posDegrees <= 180; posDegrees++) 
                                                {
                                                    servo1.write(posDegrees);
                                                    delay(20);
                                                }
                                        }
                                    if (row->values[0][0]=='O' && row->values[0][1]=='F' && row->values[0][2]=='F')
                                        {
                                            for(int posDegrees = 180; posDegrees >= 0; posDegrees--)
                                                {
                                                    servo1.write(posDegrees);
                                                    delay(20);
                                                } 
                                        }
                                    if (f < cols->num_fields-1) 
                                        {
                                            Serial.print(',');
                                        }
                                }
                            Serial.println();
                        }
                } 
            while (row != NULL);
            // Deleting the cursor also frees up memory used
            delete cur_mem;
}
// Select Section End
