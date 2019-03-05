// MySQL Connection Configuration
extern IPAddress server_addr(192,168,1,80);  // IP of the MySQL *server* here
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
                sprintf(query_Temperature, INSERT_DATA_Temperature, "DHT11", temperature);
                sprintf(query_Humidity, INSERT_DATA_Humidity, "DHT11", humidity);
                // Execute the query
                cur_mem->execute(query_Temperature);
                cur_mem->execute(query_Humidity);
                // Note: since there are no results, we do not need to read any data
                // Deleting the cursor also frees up memory used
                delete cur_mem;
                Serial.println("Temeperature Data recorded.");
                delay(100);
                Serial.println("Humidity Data recorded.");
            }
        else
        Serial.println("Connection failed.");
        // conn.close();
    }
// Insert section end
