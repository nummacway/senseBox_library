#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "SenseBox.h"
#include <SPI.h>
//RTC
#if defined(__AVR__)
# include <avr/io.h>
#endif
#if ARDUINO >= 100
# include "Arduino.h"
#else
# include "WProgram.h"
#endif
#include "Wire.h"
#include "RTClib.h"
#include <math.h>



//-----Ultraschall Distanz Sensor HC-S04 begin----//
Ultrasonic::Ultrasonic(int rx, int tx)
{
	_rx = rx;
	_tx = tx;
}
/*The measured distance from the range 0 to 400 Centimeters*/
long Ultrasonic::getDistance(void)
{
	pinMode(_rx, OUTPUT);
	digitalWrite(_rx, LOW);
	delayMicroseconds(2);
	digitalWrite(_rx, HIGH);
	delayMicroseconds(5);
	digitalWrite(_rx,LOW);
	pinMode(_tx,INPUT);
	long duration;
	duration = pulseIn(_tx,HIGH);
	long distance;
	distance = duration/58;
	return distance;
}
//-----Ultraschall Distanz Sensor HC-S04 End----//

//-----HDC100X Stuff begin----//

/*
This library was written for the Texas Instruments
HDC100X temperature and humidity sensor.
It has been tested for the HDC1000 and the HDC1008
Buy the HDC1008 breakout board at: https://www.tindie.com/stores/RFgermany
This library is made by Florian Roesner.
Released under GNU GPL v2.0 license.

*************************/
//PUBLIC:

HDC100X::HDC100X(){
	ownAddr = HDC100X_DEFAULT_ADDR;
	dataReadyPin = -1;
}
//-----------------------------------------------------------------------
HDC100X::HDC100X(uint8_t address){
	ownAddr = address;
	//dataReadyPin = pin;
}
//-----------------------------------------------------------------------
HDC100X::HDC100X(bool addr0, bool addr1){
	// set the two bits the way you set the address jumpers
	ownAddr = 0b1000000 |(addr0|(addr1<<1));
	//dataReadyPin = pin;
}

//######-----------------------------------------------------------------------
//######-----------------------------------------------------------------------
uint8_t HDC100X::begin(){
	HDC100X::begin(HDC100X_TEMP_HUMI,HDC100X_14BIT, HDC100X_14BIT, DISABLE);
}
uint8_t HDC100X::begin(uint8_t mode, uint8_t tempRes, uint8_t humiRes, bool heaterState){
	/* sets the mode and resolution and the state of the heater element.  care must be taken, because it will change the temperature reading
	** in:
	** mode: HDC100X_TEMP_HUMI
	** tempRes: HDC100X_11BIT/HDC100X_14BIT
	** humiRes:  HDC100X_8BIT/HDC100X_11BIT/HDC100X_14BIT
	** heaterState: ENABLE/DISABLE
	** out:
	** high byte of the configuration register
	*/
	Wire.begin();
	HDCmode = mode;
	return writeConfigData(mode|(tempRes<<2)|humiRes|(heaterState<<5));
}
//-----------------------------------------------------------------------
uint8_t HDC100X::begin(uint8_t mode, uint8_t resulution, bool heaterState){
		/* sets the mode, resolution and heaterState. Care must be taken, because it will change the temperature reading
	** in:
	** mode: HDC100X_TEMP/HDC100X_HUMI
	** resolution: HDC100X_8BIT(just for the humidity)/HDC100X_11BIT(both)/HDC100X_14BIT(both)
	** heaterState: ENABLE/DISABLE
	** out:
	** high byte of the configuration register
	*/
	Wire.begin();
	HDCmode = mode;
	if(mode == HDC100X_HUMI) 	return writeConfigData(resulution|(heaterState<<5));
	else 						return writeConfigData((resulution<<2)|(heaterState<<5));
}

//######-----------------------------------------------------------------------

void HDC100X::setAddr(uint8_t address){
	/* sets the slave address
	** in:
	** address: slave address byte
	** out:
	** none
	*/
	ownAddr = address;
}
//-----------------------------------------------------------------------
void HDC100X::setAddr(bool addr0, bool addr1){
	/* sets the slave address
	** in:
	** addr0: true/false
	** addr1: true/false
	** out:
	** none
	*/
	ownAddr = 0b1000000 |(addr0|(addr1<<1));
}
//-----------------------------------------------------------------------
void HDC100X::setDrPin(int8_t pin){
	dataReadyPin = pin;
}

//######-----------------------------------------------------------------------

uint8_t HDC100X::setMode(uint8_t mode, uint8_t tempRes, uint8_t humiRes){
	/* sets the mode and resolution
	** in:
	** mode: HDC100X_TEMP_HUMI
	** tempRes: HDC100X_11BIT/HDC100X_14BIT
	** humiRes:  HDC100X_8BIT/HDC100X_11BIT/HDC100X_14BIT
	** out:
	** high byte of the configuration register
	*/
	uint8_t tempReg = getConfigReg() & 0xA0;
	HDCmode = mode;
	return writeConfigData(tempReg|mode|(tempRes<<2)|humiRes);
}
//-----------------------------------------------------------------------
uint8_t HDC100X::setMode(uint8_t mode, uint8_t resolution){
	/* sets the mode and resolution
	** in:
	** mode: HDC100X_TEMP/HDC100X_HUMI
	** resolution: HDC100X_8BIT(just for the humidity)/HDC100X_11BIT(both)/HDC100X_14BIT(both)
	** out:
	** high byte of the configuration register
	*/
	uint8_t tempReg = getConfigReg() & 0xA0;
	HDCmode = mode;
	if(mode == HDC100X_HUMI) 	return writeConfigData(tempReg|resolution);
	else 						return writeConfigData(tempReg|(resolution<<2));
}

//######-----------------------------------------------------------------------

uint8_t HDC100X::setHeater(bool state){
	/* turns on the heater to get rid of condensation. Care must be taken, because it will change the temperature reading
	** in:
	** state: true/false
	** out:
	** high byte of the configuration register
	*/
	uint8_t regData = getConfigReg() & 0x5F;
	if(state) return writeConfigData(regData|(state<<5));
	return writeConfigData(regData);
}

//######-----------------------------------------------------------------------

bool HDC100X::battLow(void){
	// returns a false if input voltage is higher than 2.8V and if lower a true

	if(getConfigReg() & 0x08) return true;
	return false;
}

//######-----------------------------------------------------------------------

float HDC100X::getTemp(void){
	// returns the a float number of the temperature in degrees Celsius
	if(HDCmode == HDC100X_TEMP || HDCmode == HDC100X_TEMP_HUMI)
		return ((float)getRawTemp()/65536.0*165.0-40.0);
}
//-----------------------------------------------------------------------
float HDC100X::getHumi(void){
	// returns the a float number of the humidity in percent
	if(HDCmode == HDC100X_HUMI || HDCmode == HDC100X_TEMP_HUMI)
		return ((float)getRawHumi()/65536.0*100.0);
}

//######-----------------------------------------------------------------------

uint16_t HDC100X::getRawTemp(void){
	// returns the raw 16bit data of the temperature register
	if(HDCmode == HDC100X_TEMP || HDCmode == HDC100X_TEMP_HUMI)
		return read2Byte(HDC100X_TEMP_REG);
}
//-----------------------------------------------------------------------
uint16_t HDC100X::getRawHumi(void){
	// returns the raw 16bit data of the humidity register
	if(HDCmode == HDC100X_HUMI || HDCmode == HDC100X_TEMP_HUMI)
		return read2Byte(HDC100X_HUMI_REG);
}

uint8_t HDC100X::getConfigReg(void){
	// returns the high byte of the configuration register
	return (read2Byte(HDC100X_CONFIG_REG)>>8);
}

uint16_t HDC100X::read2Byte(uint8_t reg){
	/* reads two bytes from the defined register
	** in:
	** reg: HDC100X_TEMP_REG/HDC100X_HUMI_REG/HDC100X_CONFIG_REG/HDC100X_ID1_REG/HDC100X_ID2_REG/HDC100X_ID3_REG
	** out:
	** two byte of data from the defined register
	*/
	setRegister(reg);
	uint16_t data;
	Wire.requestFrom(ownAddr, 2U);
	if(Wire.available()>=2){
		data = Wire.read()<<8;
		data += Wire.read();
	}
	return data;
}

uint8_t HDC100X::writeConfigData(uint8_t config){
	/* writes the config byte to the configuration register
	** in:
	** config: one byte
	** out:
	** one byte 0:success  1:data too long to fit in transmit buffer    2:received NACK on transmit of address    3:received NACK on transmit of data    4:other error
	*/
	Wire.beginTransmission(ownAddr);
	Wire.write(HDC100X_CONFIG_REG);
	Wire.write(config);
	Wire.write(0x00); 					//the last 8 bits are always 0
	return Wire.endTransmission();
}

//PRIVATE:
void HDC100X::setRegister(uint8_t reg){
	/* set the register for the next read or write cycle
	** in:
	** reg: HDC100X_TEMP_REG/HDC100X_HUMI_REG/HDC100X_CONFIG_REG/HDC100X_ID1_REG/HDC100X_ID2_REG/HDC100X_ID3_REG
	** out:
	** none
	*/
	Wire.beginTransmission(ownAddr);
	Wire.write(reg);
	Wire.endTransmission();
	delay(10);	// wait a little so that the sensor can set its register
}
//------------------------------------------------------------------------------------------HDC100X Stuff End------------------------//


//--------------------------------------------------------------------------------Helligkeitssensor 3216 begin------------------------//
/***************************************************
  This is a library for the CJMCU-3216/AP3216 Lux sensor breakout board made compatible with the TSL45315 lib
  These sensors use I2C to communicate, 2 pins are required to interface

  Written by Redeemer (numma_cway)
  BSD license, all text above must be included in any redistribution
 ****************************************************/

TSL45315::TSL45315(void)
{
	range = 1;
  factor = 0.35;
}

/*========================================================================*/
/*                           PRIVATE FUNCTIONS                            */
/*========================================================================*/

void TSL45315::AP3216_write(int regAddress, int value) {
  Wire.beginTransmission(0x1E); // I2C Address of AP3216 sensor is 0x1E
  Wire.write(regAddress);
  Wire.write(value);
  Wire.endTransmission(); 
}

byte TSL45315::AP3216_read(int regAddress) {
  Wire.beginTransmission(0x1E); // I2C Address of AP3216 sensor is 0x1E
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(0x1E, 1, true);
  return Wire.read() & 0xFF;
}

word TSL45315::alsReadCount()
{
  return (AP3216_read(0x0D) << 8) | AP3216_read(0x0C);
}

void TSL45315::alsSetRange(byte newRange)
{
  AP3216_write(0x10, (newRange - 1) << 4);
  range = newRange;
  switch ( range )
  {
    case 1: factor = 0.35;
            break;
    case 2: factor = 0.0788;
            break;
    case 3: factor = 0.0197;
            break;
    case 4: factor = 0.0049;
            break;
  }
  // Found by testing
  delay(50);
  alsReadCount();
  delay(50);
  alsReadCount();
  delay(50);
  alsReadCount();
  delay(50);
  alsReadCount();
  delay(50);
}



/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/
boolean TSL45315::begin(void)
{
  Wire.begin();
	AP3216_write(0x00, 0x01);
  return true;
}


float TSL45315::getLux(void)
{
	word count = alsReadCount();

  // Can we get more accuracy by using a higher range number?
  // To be able to fall back, we do this first, because a less accurate measurement by a lower range number is better than an exceed range
  if ( ( (range == 1) && ( count < 14754 ) ) || // < 5164.158 lx
       ( ( (range == 2) || ( range == 3) ) && ( count < 16383 ) ) ) // < 1291.0395 lx or < 321.1215 lx
  {
    alsSetRange(range + 1);
    count = alsReadCount();
  }
  
  // Do we have to change the range to be able to get a measurement that doesn't exceed the range?
  while ( (count == 65535) && (range > 1) )
  {
    alsSetRange(range - 1);
    count = alsReadCount();
  }

  return factor * count;
}


boolean TSL45315::powerDown(void)
{
	AP3216_write(0x00, 0x00);
  return true;
}
//-----Helligkeitssensor 3216 end----//


//-----VEML6070 UV Sensor begin ----///
/*
Copyright (c) 2016, Embedded Adventures
All rights reserved.
Contact us at source [at] embeddedadventures.com
www.embeddedadventures.com
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
- Neither the name of Embedded Adventures nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.
*/
boolean VEML6070::begin(void)
{
	Wire.begin();

 Wire.beginTransmission(UV_ADDR);
 Wire.write((IT_1<<2) | 0x02);
 Wire.endTransmission();
 delay(500);
}

int VEML6070::getUV(void)
{
	byte msb=0, lsb=0;
uint16_t uvValue;

Wire.requestFrom(UV_ADDR+1, 1); //MSB
delay(1);
if(Wire.available()) msb = Wire.read();

Wire.requestFrom(UV_ADDR+0, 1); //LSB
delay(1);
if(Wire.available()) lsb = Wire.read();

uvValue = (msb<<8) | lsb;

return (int) uvValue*5;
}

//-----------------RTC BEGIN-------
/*
  Texas Instruments DS1307 RTC Lib for Arduino
  Adapter that converts calls to the RV8523 RTC lib into calls to Adafruit RTC lib
 */


//-------------------- Constructor --------------------


RV8523::RV8523(void)
{
  Wire.begin();
  clockObj = RTC_DS1307();
}


//-------------------- Public --------------------


void RV8523::start(void)
{
  begin();
}

void RV8523::get(uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *month, uint16_t *year)
{
  DateTime now = clockObj.now();
  *sec = now.second();
  *min = now.minute();
  *hour = now.hour();
  *day = now.day();
  *month = now.month();
  *year = now.year();
  
  return;
}


void RV8523::get(int *sec, int *min, int *hour, int *day, int *month, int *year)
{
  DateTime now = clockObj.now();
  *sec = now.second();
  *min = now.minute();
  *hour = now.hour();
  *day = now.day();
  *month = now.month();
  *year = now.year();

  return;
}


void RV8523::set(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint16_t year)
{
  clockObj.adjust(DateTime(year, month, day, hour, min, sec));
}


void RV8523::set(int sec, int min, int hour, int day, int month, int year)
{
  return set((uint8_t)sec, (uint8_t)min, (uint8_t)hour, (uint8_t)day, (uint8_t)month, (uint16_t)year);
}

//-----new----RTC---Functions

void RV8523::begin(void)
{
	clockObj.begin();
}

// A convenient constructor for using "the compiler's time":
void RV8523::setTime(const char* date, const char* time)
{
  clockObj.adjust(DateTime(date, time));
}

uint16_t RV8523::getYear(void)
{
  return clockObj.now().year();
}

uint8_t RV8523::getMonth(void)
{
  return clockObj.now().month();
}

uint8_t RV8523::getDay(void)
{
  return clockObj.now().day();
}

uint8_t RV8523::getHour(void)
{
  return clockObj.now().hour();
}

uint8_t RV8523::getMin(void)
{
  return clockObj.now().minute();
}

uint8_t RV8523::getSec(void)
{
  return clockObj.now().second();
}

/***************************************************************************
  This is a library for the BMP280 pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/


BMP280::BMP280()  
{ }




bool BMP280::begin(uint8_t a, uint8_t  b, uint8_t chipid) {
   Wire.begin();
   Wire.beginTransmission(118);
   int error = Wire.endTransmission();
   if (error == 0)
   {
      _i2caddr = a;

   }else{
   	_i2caddr = b;
   }
    Wire.begin();
  if (read8(BMP280_REGISTER_CHIPID) != chipid)
    return false;

  readCoefficients();
  write8(BMP280_REGISTER_CONTROL, 0x3F);
  return true;
}

uint8_t BMP280::spixfer(uint8_t x) {
    return SPI.transfer(x);

  uint8_t reply = 0;
  for (int i=7; i>=0; i--) {
    reply <<= 1;
    digitalWrite(-1, LOW);
    digitalWrite(-1, x & (1<<i));
    digitalWrite(-1, HIGH);
    if (digitalRead(-1))
      reply |= 1;
  }
  return reply;
}

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C/SPI
*/
/**************************************************************************/
void BMP280::write8(byte reg, byte value)
{

    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
    Wire.endTransmission();

}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C/SPI
*/
/**************************************************************************/
uint8_t BMP280::read8(byte reg)
{
  uint8_t value;

    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
    value = Wire.read();


  return value;
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C/SPI
*/
/**************************************************************************/
uint16_t BMP280::read16(byte reg)
{
  uint16_t value;
	    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)2);
    value = (Wire.read() << 8) | Wire.read();
  return value;
}

uint16_t BMP280::read16_LE(byte reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);

}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C/SPI
*/
/**************************************************************************/
int16_t BMP280::readS16(byte reg)
{
  return (int16_t)read16(reg);

}

int16_t BMP280::readS16_LE(byte reg)
{
  return (int16_t)read16_LE(reg);

}


/**************************************************************************/
/*!
    @brief  Reads a 24 bit value over I2C/SPI
*/
/**************************************************************************/
uint32_t BMP280::read24(byte reg)
{
  uint32_t value;
    Wire.beginTransmission((uint8_t)_i2caddr);
    Wire.write((uint8_t)reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)_i2caddr, (byte)3);
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
    value <<= 8;
    value |= Wire.read();
  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void BMP280::readCoefficients(void)
{
    _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

    _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float BMP280::getTemperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bmp280_calib.dig_T1 <<1))) *
	   ((int32_t)_bmp280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
	   ((int32_t)_bmp280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float BMP280::getPressure(void) {
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  getTemperature();

  int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bmp280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bmp280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)_bmp280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bmp280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7)<<4);
  return (float)p/256;
}

float BMP280::getAltitude(float seaLevelhPa) {
  float altitude;
  float pressure = getPressure(); // in Si units for Pascal
  pressure /= 100;
  altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
  return altitude;
}
