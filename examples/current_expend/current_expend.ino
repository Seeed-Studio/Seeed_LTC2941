
#include "LTC2941.h"

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

float coulomb = 0, mAh = 0;

void setup(void)
{
    Wire.begin();
    
    SERIAL.begin(115200);
    while(!SERIAL.available());
    
    SERIAL.println("LTC2941 Raw Data");
    
    ltc2941.initialize();
    ltc2941.setPrescaler(PRESCALAR_M_1);
}

void loop(void)
{
    coulomb = ltc2941.getCoulombsExpend();
    mAh = ltc2941.getmAhExpend();
    
    SERIAL.print(coulomb);
    SERIAL.print("C,");
    SERIAL.print(mAh);
    SERIAL.print("mAh");
    
    SERIAL.println();
    
    delay(5000);
}
