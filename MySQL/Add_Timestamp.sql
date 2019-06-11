use test_arduino;
alter table Temperature_data
modify column Recorded_Time datetime null default current_timestamp;
alter table Humidity_data
modify column Recorded_Time datetime null default current_timestamp;