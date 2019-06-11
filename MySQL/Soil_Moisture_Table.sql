use Arduino;
CREATE TABLE Soil_Mositure_Data
(
	Data_ID int  auto_increment  NOT NULL  PRIMARY KEY,
	Sensor_Name varchar(255),
	Sensor_Value varchar(255),
    Soil_State varchar(255),
	Data_Time datetime null default current_timestamp
)