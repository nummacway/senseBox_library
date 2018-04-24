/*
  SenseBox.h Arduino
*/
#ifndef SenseBox_h
#define SenseBox_h

#include "Arduino.h"
#include <inttypes.h>
#include "Wire.h"

#include "OpenSenseMap.h"
#include "RTClib.h"

//----------------------------------------------------------------------HDC100X Stuff begin----//
/*
This library was written for the Texas Instruments
HDC100X temperature and humidity sensor.
It has been tested for the HDC1000 and the HDC1008
Buy the HDC1008 breakout board at: https://www.tindie.com/stores/RFgermany
This library is made by Florian Roesner.
Released under GNU GPL v2.0 license.
*/
#define HDC100X_DEFAULT_ADDR		0x43

#define HDC100X_TEMP_REG			0x00
#define HDC100X_HUMI_REG			0x01
#define	HDC100X_CONFIG_REG			0x02
#define HDC100X_ID1_REG				0xFB
#define HDC100X_ID2_REG				0xFC
#define HDC100X_ID3_REG				0xFD


#define HDC100X_RST					0x80
#define	HDC100X_TEMP_HUMI			0x16
#define	HDC100X_HUMI				1
#define	HDC100X_TEMP				0

#define HDC100X_14BIT				0x00
#define HDC100X_11BIT				0x01
#define HDC100X_8BIT				0x02

#define DISABLE						0
#define ENABLE						1
//------------------------------------------------------------------------HDC100X Stuff End----//

//------------------------------------------------------------------------TSL45315 Stuff Begin-//
#define TSL45315_I2C_ADDR 		(0x29)

#define TSL45315_REG_CONTROL  	(0x00)
#define TSL45315_REG_CONFIG   	(0x01)
#define TSL45315_REG_DATALOW  	(0x04)
#define TSL45315_REG_DATAHIGH 	(0x05)
#define TSL45315_REG_ID       	(0x0A)

// Sensing time for one measurement
#define TSL45315_TIME_M1		(0x00)	// M=1 T=400ms
#define TSL45315_TIME_M2		(0x01)	// M=2 T=200ms
#define TSL45315_TIME_M4		(0x02)	// M=4 T=100ms
//-----TSL45315 Stuff End-//

//-----------------------------------------------------------------------VEML6040 Stuff Begin-//
#define UV_ADDR 0x38
// Integrationszeiten
#define IT_0_5 0x0 // 1/2T
#define IT_1   0x1 // 1T
#define IT_2   0x2 // 2T
#define IT_4   0x3 // 4T
// Referenzwert: 0,01 W/m^2 ist äquivalent zu 0.4 als UV-Index
//-----------------------------------------------------------------------VEML6070 Stuff End--/

//----------------------------------------------------------------------RTC Stuff Begin-//
#define RV8523_h
//----------------------------------------------------------------------RTC Stuff End--//

//----------------------------------------------------------------------BMP_STUFF Begin----//


//	=========================================================================
    //I2C ADDRESS/BITS/SETTINGS

    #define BMP280_ADDRESS                (0x76)
	#define BMP280_ADDRESS_B              (0x77)
    #define BMP280_CHIPID                 (0x58)
	#define BMP_SCK 13
	#define BMP_MISO 12
	#define BMP_MOSI 11
	#define BMP_CS 10

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    enum
    {
      BMP280_REGISTER_DIG_T1              = 0x88,
      BMP280_REGISTER_DIG_T2              = 0x8A,
      BMP280_REGISTER_DIG_T3              = 0x8C,

      BMP280_REGISTER_DIG_P1              = 0x8E,
      BMP280_REGISTER_DIG_P2              = 0x90,
      BMP280_REGISTER_DIG_P3              = 0x92,
      BMP280_REGISTER_DIG_P4              = 0x94,
      BMP280_REGISTER_DIG_P5              = 0x96,
      BMP280_REGISTER_DIG_P6              = 0x98,
      BMP280_REGISTER_DIG_P7              = 0x9A,
      BMP280_REGISTER_DIG_P8              = 0x9C,
      BMP280_REGISTER_DIG_P9              = 0x9E,

      BMP280_REGISTER_CHIPID             = 0xD0,
      BMP280_REGISTER_VERSION            = 0xD1,
      BMP280_REGISTER_SOFTRESET          = 0xE0,

      BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

      BMP280_REGISTER_CONTROL            = 0xF4,
      BMP280_REGISTER_CONFIG             = 0xF5,
      BMP280_REGISTER_PRESSUREDATA       = 0xF7,
      BMP280_REGISTER_TEMPDATA           = 0xFA,
    };

/*=========================================================================*/

/*=========================================================================
    CALIBRATION DATA
    -----------------------------------------------------------------------*/
    typedef struct
    {
      uint16_t dig_T1;
      int16_t  dig_T2;
      int16_t  dig_T3;

      uint16_t dig_P1;
      int16_t  dig_P2;
      int16_t  dig_P3;
      int16_t  dig_P4;
      int16_t  dig_P5;
      int16_t  dig_P6;
      int16_t  dig_P7;
      int16_t  dig_P8;
      int16_t  dig_P9;

      uint8_t  dig_H1;
      int16_t  dig_H2;
      uint8_t  dig_H3;
      int16_t  dig_H4;
      int16_t  dig_H5;
      int8_t   dig_H6;
    } bmp280_calib_data;
/*=========================================================================*/
//----------------------------------------------------------------------BMP_STUFF End----//


//-----Ultraschall Distanz Sensor HC-S04----//
class Ultrasonic
{
  public:
    Ultrasonic(int rx, int tx);
		long getDistance(void);
  private:
    int _rx;//pin of rx pin
    int _tx;//pin of tx pin
};

//-----Temperatur und Luftfeuchtigkeit Sensor HDC100X----//
class HDC100X{
	public:
		HDC100X();
		HDC100X(uint8_t address);
		HDC100X(bool addr0, bool addr1);

		uint8_t begin(void);
		uint8_t begin(uint8_t mode, uint8_t tempRes, uint8_t humiRes, bool heaterState);
		uint8_t begin(uint8_t mode, uint8_t resulution, bool heaterState);

		void setAddr(bool addr0, bool addr1);
		void setAddr(uint8_t address);
		void setDrPin(int8_t pin);

		uint8_t setMode(uint8_t mode, uint8_t tempRes, uint8_t humiRes);
		uint8_t setMode(uint8_t mode, uint8_t resolution);

		uint8_t setHeater(bool state);
		bool battLow(void);

		float getTemp(void);
		float getHumi(void);

		uint16_t getRawTemp(void);
		uint16_t getRawHumi(void);

		uint8_t getConfigReg(void);
		uint16_t read2Byte(uint8_t reg);

		uint8_t writeConfigData(uint8_t config);

	private:
  		uint8_t ownAddr;
		uint8_t dataReadyPin;
		uint8_t HDCmode;
  		void setRegister(uint8_t reg);

};
//---------------HDC------END-------------------------//

//-----Helligkeitssensor 3216----//
/***************************************************
  This is a library for the CJMCU-3216/AP3216 Lux sensor breakout board made compatible with the TSL45315 lib
  These sensors use I2C to communicate, 2 pins are required to interface

  Written by Redeemer (numma_cway)
  BSD license, all text above must be included in any redistribution
 ****************************************************/

class TSL45315 {
public:
	TSL45315(void);
	boolean begin(void);
	float getLux(void);
	boolean powerDown(void);


private:
	byte range;
  float factor;
  void AP3216_write(int regAddress, int value);
  byte AP3216_read(int regAddress);
  word alsReadCount(void);
  void alsSetRange(byte newRange);
  
};
//----------------------------------------//

//------VEML6070 -------//
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
class VEML6070{
public:
  boolean begin(void);
  int getUV(void);


private:
float refVal = 0.4;

};

//----------------------rtc-------------------------------
/*
  RV8523 RTC Lib for Arduino
 */
class RV8523
{
  public:
    RV8523();

    void start(void);
    void get(uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *month, uint16_t *year);
    void get(int *sec, int *min, int *hour, int *day, int *month, int *year);
    void set(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint16_t year);
    void set(int sec, int min, int hour, int day, int month, int year);


    //new funtions
    void begin(void);
    void setTime(const char* date, const char* time);
    uint16_t getYear(void);
    uint8_t getMonth(void);
    uint8_t getDay(void);
    uint8_t getHour(void);
    uint8_t getMin(void);
    uint8_t getSec(void);


  private:
    RTC_DS1307 clockObj;
};






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


class BMP280
{
  public:
    BMP280();
    bool  begin(uint8_t addr = BMP280_ADDRESS, uint8_t addr_b = BMP280_ADDRESS_B, uint8_t chipid = BMP280_CHIPID);
    float getTemperature(void);
    float getPressure(void);
    float getAltitude(float seaLevelhPa = 1013.25);

  private:

    void readCoefficients(void);
    uint8_t spixfer(uint8_t x);

    void      write8(byte reg, byte value);
    uint8_t   read8(byte reg);
    uint16_t  read16(byte reg);
    uint32_t  read24(byte reg);
    int16_t   readS16(byte reg);
    uint16_t  read16_LE(byte reg); // little endian
    int16_t   readS16_LE(byte reg); // little endian

    uint8_t   _i2caddr;
    int32_t   _sensorID;
    int32_t t_fine;

    int8_t _cs, _mosi, _miso, _sck;

    bmp280_calib_data _bmp280_calib;

};

#endif
