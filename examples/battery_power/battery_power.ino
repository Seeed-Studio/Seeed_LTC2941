
#include "LTC2941.h"

float coulomb = 0, mAh = 0, percent = 0;

void setup(void)
{
    Wire.begin();
    
    Serial.begin(115200);
    while(!Serial.available());
    
    Serial.println("LTC2941 Raw Data");
    
    ltc2941.initialize();
    ltc2941.setBatteryFullMAh(1050);
}

void loop(void)
{
    coulomb = ltc2941.getCoulombs();
    mAh = ltc2941.getmAh();
    percent = ltc2941.getPercent();
    
    Serial.print(coulomb);
    Serial.print("mC,");
    Serial.print(mAh);
    Serial.print("mAh,");
    Serial.print(percent);
    Serial.print("%");
    
    Serial.println();
    
    delay(1000);
}
