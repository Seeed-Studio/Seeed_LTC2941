/*    
 * A library for Grove - Coulomb Counter for 3.3V to 5V (LTC2941)
 *   
 * Copyright (c) 2018 seeed technology co., ltd.  
 * Author      : Wayen Weng  
 * Create Time : June 2018
 * Change Log  : 
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */ 

#ifndef __LINEAR_LTC2941_H__
#define __LINEAR_LTC2941_H__

#include <Arduino.h>
#include <Wire.h>

#define LINEAR_LTC2941_ADDRESS                  0x64

#define LTC2941_STATUS_REG                      0x00
#define LTC2941_CONTROL_REG                     0x01
#define LTC2941_ACCUM_CHARGE_MSB_REG            0x02
#define LTC2941_ACCUM_CHARGE_LSB_REG            0x03
#define LTC2941_CHARGE_THRESH_HIGH_MSB_REG      0x04
#define LTC2941_CHARGE_THRESH_HIGH_LSB_REG      0x05
#define LTC2941_CHARGE_THRESH_LOW_MSB_REG       0x06
#define LTC2941_CHARGE_THRESH_LOW_LSB_REG       0x07

#define LTC2941_VBAT_ALERT_MASK     0x3F
#define LTC2941_PRESCALAR_MASK      0xC7
#define LTC2941_ALERT_MODE_MASK     0xF9
#define LTC2941_SHUTDOWN_MASK       0xFE

#define MEASURE_VBAT_ALERT_POS		6
#define MEASURE_PRESCALAR_POS		3
#define MEASURE_ALERT_MODE_POS		1
#define MEASURE_SHUTDOWN_POS		0

#define LTC2941_BATTERY_MAX         5570 // mAh

typedef enum
{
    VBAT_ALERT_OFF = 0,
    VBAT_2_8_V = 1,
    VBAT_2_9_V = 2,
    VBAT_3_0_V = 3,
} LTC2941_VBAT_ALERT;

typedef enum
{
    PRESCALAR_M_1 = 0,
    PRESCALAR_M_2 = 1,
    PRESCALAR_M_4 = 2,
    PRESCALAR_M_8 = 3,
    PRESCALAR_M_16 = 4,
    PRESCALAR_M_32 = 5,
    PRESCALAR_M_64 = 6,
    PRESCALAR_M_128 = 7,
} LTC2941_PRESCALAR;

typedef enum
{
    ALERT_DISABLED = 0,
    CHARGE_COMPLETE = 1,
    ALERT_MODE = 2,
} LTC2941_ALERT_CONF;

class LTC2941
{
    public:
        
        LTC2941(void);
        
        void initialize(void);

        void setBatteryAlert(LTC2941_VBAT_ALERT voltage);
        void setPrescaler(LTC2941_PRESCALAR prescale);
        void setAlertConfig(LTC2941_ALERT_CONF config);
        void setShutdown(bool enable);
        
        void setAccumulatedCharge(uint16_t thresh);
        void setChargeThresholdHigh(uint16_t thresh);
        void setChargeThresholdLow(uint16_t thresh);
        
        uint8_t getStatus(void);
        
        void setBatteryFullMAh(uint16_t mAh, bool flag = true);
        float getCoulombs(void);
        float getmAh(void);
        float getPercent(void);
        
        float getCoulombsExpend(void);
        float getmAhExpend(void);
    
    private:
        
        void write8(uint8_t reg, uint8_t val);
        void write16(uint8_t reg, uint16_t val);
        uint8_t read8(uint8_t reg);
        uint16_t read16(uint8_t reg);
        void updateReg(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t val);

        float resistor;
        uint16_t prescalar;
        uint16_t powerMaxAMh;
        uint32_t powerExpend;
        uint8_t devAddr;
};

extern LTC2941 ltc2941;

#endif
