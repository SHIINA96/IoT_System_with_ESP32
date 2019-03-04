// THE SERVO MOTOR FUNCTIONS
int Gate_Open(void)
    {
        for(int posDegrees = 0; posDegrees <= 180; posDegrees++) 
            {
                servo1.write(posDegrees);
                delay(20);
            }
    }
int Gate_Close(void)
    {
        for(int posDegrees = 180; posDegrees >= 0; posDegrees--)
            {
                servo1.write(posDegrees);
                delay(20);
            } 
    }
