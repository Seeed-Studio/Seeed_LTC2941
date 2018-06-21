
#include "LTC2941.h"

float coulomb = 0, mAh = 0;

void setup(void)
{
    Wire.begin();
    
    Serial.begin(115200);
    while(!Serial.available());
    
    Serial.println("LTC2941 Raw Data");
    
    ltc2941.initialize();
    ltc2941.setPrescaler(PRESCALAR_M_1);
}

void loop(void)
{
    coulomb = ltc2941.getCoulombsExpend();
    mAh = ltc2941.getmAhExpend();
    
    Serial.print(coulomb);
    Serial.print("mC,");
    Serial.print(mAh);
    Serial.print("mAh");
    
    Serial.println();
    
    delay(5000);
}
