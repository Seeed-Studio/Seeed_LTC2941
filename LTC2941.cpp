/*
    A library for Grove - Coulomb Counter for 3.3V to 5V (LTC2941)

    Copyright (c) 2018 seeed technology co., ltd.
    Author      : Wayen Weng
    Create Time : June 2018
    Change Log  :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "LTC2941.h"

const float LTC2941_CHARGE_LSB = 0.085;
const uint8_t prescalarTable[8] = {1, 2, 4, 8, 16, 32, 64, 128};

LTC2941::LTC2941(void) {
    resistor = 0.050;
    prescalar = 128;
    powerMaxAMh = LTC2941_BATTERY_MAX;
    powerExpend = 0;
    devAddr = LINEAR_LTC2941_ADDRESS;
}

void LTC2941::initialize(void) {
    setBatteryAlert(VBAT_ALERT_OFF);
    setAlertConfig(ALERT_DISABLED);
    setPrescaler(PRESCALAR_M_128);
    setAccumulatedCharge(0xffff);
}

void LTC2941::setBatteryFullMAh(uint16_t mAh, bool flag) {
    uint8_t i = 0;
    float qLSB = 0, temp = mAh;

    if (mAh > LTC2941_BATTERY_MAX) {
        return;
    }

    qLSB = temp / 65536;

    for (i = 0; i < 8; i ++) {
        temp = (LTC2941_CHARGE_LSB * prescalarTable[i] * 0.05) / (resistor * 128);
        if (qLSB <= temp) {
            break;
        }
    }

    if (i >= 8) {
        return;
    }

    powerMaxAMh = mAh;

    prescalar = prescalarTable[i];

    qLSB = (LTC2941_CHARGE_LSB * prescalar * 0.05) / (resistor * 128);

    temp = mAh;
    temp = temp / qLSB;
    mAh = (uint16_t)temp;

    if (flag) {
        setAccumulatedCharge(mAh);
    }
}

void LTC2941::setBatteryAlert(LTC2941_VBAT_ALERT voltage) {
    updateReg(LTC2941_CONTROL_REG, LTC2941_VBAT_ALERT_MASK, MEASURE_VBAT_ALERT_POS, voltage);
}

void LTC2941::setPrescaler(LTC2941_PRESCALAR prescale) {
    prescalar = prescalarTable[prescale];

    updateReg(LTC2941_CONTROL_REG, LTC2941_PRESCALAR_MASK, MEASURE_PRESCALAR_POS, prescale);
}

void LTC2941::setAlertConfig(LTC2941_ALERT_CONF config) {
    updateReg(LTC2941_CONTROL_REG, LTC2941_ALERT_MODE_MASK, MEASURE_ALERT_MODE_POS, config);
}
void LTC2941::setShutdown(bool enable) {
    updateReg(LTC2941_CONTROL_REG, LTC2941_SHUTDOWN_MASK, MEASURE_SHUTDOWN_POS, enable);
}

void LTC2941::setAccumulatedCharge(uint16_t thresh) {
    write16(LTC2941_ACCUM_CHARGE_MSB_REG, thresh);
}

void LTC2941::setChargeThresholdHigh(uint16_t thresh) {
    write16(LTC2941_CHARGE_THRESH_HIGH_MSB_REG, thresh);
}

void LTC2941::setChargeThresholdLow(uint16_t thresh) {
    write16(LTC2941_CHARGE_THRESH_LOW_MSB_REG, thresh);
}

uint8_t LTC2941::getStatus(void) {
    return read8(LTC2941_STATUS_REG);
}

float LTC2941::getCoulombs(void) {
    uint16_t data = 0;
    float coulomb = 0;

    data = read16(LTC2941_ACCUM_CHARGE_MSB_REG);

    coulomb = (float)(data * LTC2941_CHARGE_LSB * prescalar * 0.05) / (resistor * 128);
    coulomb = coulomb * 3.6f;

    return coulomb;
}

float LTC2941::getmAh(void) {
    uint16_t data = 0;
    float mAh = 0;

    data = read16(LTC2941_ACCUM_CHARGE_MSB_REG);

    mAh = (float)(data * LTC2941_CHARGE_LSB * prescalar * 0.05) / (resistor * 128);

    return mAh;
}

float LTC2941::getPercent(void) {
    float mAh = getmAh();
    return (mAh * 100 / powerMaxAMh);
}

float LTC2941::getCoulombsExpend(void) {
    uint16_t data = 0;
    float coulomb = 0;

    data = 0xffff - read16(LTC2941_ACCUM_CHARGE_MSB_REG);

    if (data == 0xffff) {
        data = 0;
    } else if (data >= 0xf000) {
        powerExpend += 0xf000;
        data -= 0xf000;
        setAccumulatedCharge(0xffff - data);
    }

    coulomb = (float)((data + powerExpend) * LTC2941_CHARGE_LSB * prescalar * 0.05) / (resistor * 128);
    coulomb = coulomb * 3.6f;

    return coulomb;
}

float LTC2941::getmAhExpend(void) {
    uint16_t data = 0;
    float mAh = 0;

    data = 0xffff - read16(LTC2941_ACCUM_CHARGE_MSB_REG);

    if (data == 0xffff) {
        data = 0;
    } else if (data >= 0xf000) {
        powerExpend += 0xf000;
        data -= 0xf000;
        setAccumulatedCharge(0xffff - data);
    }

    mAh = (float)((data + powerExpend) * LTC2941_CHARGE_LSB * prescalar * 0.05) / (resistor * 128);

    return mAh;
}

void LTC2941::write8(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(devAddr);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void LTC2941::write16(uint8_t reg, uint16_t val) {
    Wire.beginTransmission(devAddr);
    Wire.write(reg);
    Wire.write(val >> 8);
    Wire.write(val & 0xff);
    Wire.endTransmission();
}

uint8_t LTC2941::read8(uint8_t reg) {
    uint8_t data = 0;

    Wire.beginTransmission(devAddr);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom((int16_t)devAddr, 1);
    while (Wire.available()) {
        data = Wire.read();
    }

    return data;
}

uint16_t LTC2941::read16(uint8_t reg) {
    uint16_t msb = 0, lsb = 0;

    Wire.beginTransmission(devAddr);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom((int16_t)devAddr, 2);
    while (Wire.available()) {
        msb = Wire.read();
        lsb = Wire.read();
    }

    return (lsb | (msb << 8));
}

void LTC2941::updateReg(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t val) {
    uint8_t tmp;

    tmp = read8(reg);

    tmp &= mask;
    tmp |= (val << shift) & ~mask;

    write8(reg, tmp);
}

LTC2941 ltc2941;
