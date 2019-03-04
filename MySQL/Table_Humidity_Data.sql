use test_arduino;
create table Humidity_data
(
	Data_ID int not null primary key auto_increment,
    Sensor_Name varchar(255),
    Humidity_Value double,
    Recorded_Time timestamp 
)
    
    
    
    