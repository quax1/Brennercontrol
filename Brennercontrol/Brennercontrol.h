// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _Brennercontrol_H_
#define _Brennercontrol_H_
#include "Arduino.h"


//add your includes for the project Brennercontrol here
#define SKETCH_NAME       "Brennerlog-6.0"
//#define RADIO
// #define SDCard
#define DEBUG
#define db_m
//#define LCD_DISPLAY  //LCD4884


//   ************   Define Pins   *******************

//   ************   Load Libraries   *******************



#include "helper.h"       // debug funktionen
#include "Common_Declarations.h"
#ifdef RADIO
	#include <SPI.h>
	#include "RF24.h"    //http://tmrh20.github.io/RF24/
#endif
// radio must be attached and powered otherwise "radio.available()" will stuck - even if failure handling is enabled


#include <SoftwareSerial.h>
//#include "RS485_protocol.h"
#include <RS485_non_blocking.h>

// RTC
//#include <Time.h>        // Library for time functions
#include <DS1307RTC.h>   // a basic DS1307 library that returns time as a time_t
#include <Wire.h>

// SD Card
#ifdef SDCard    //
	#include <SD.h>
#endif

// LCD_DISPLAY
#ifdef LCD_DISPLAY
	#include "LCD4884.h"
#endif

// Temperatur Sensor Dallas 18B20P
//#define ONE_WIRE_BUS 8 //8 alternative 16


#include <OneWire.h>
#include <DallasTemperature.h>

// EEPROM
#include <EEPROM.h>


//   ************   Declarations   *******************

#ifdef RADIO
	extern RF24 radio;
#endif



//	// Neu
//	extern volatile unsigned long startTime;
//	extern volatile unsigned long endTime;
//	extern volatile boolean BurnerState_idle;  // Brennerzustand initial aus - Relaiskontakt offen


// **********   Constants and Pins
const byte PIN_ONE_WIRE_BUS = 8;
const byte PIN_BRENNER_RELAIS = A0;         //A3; // must be 2 if interupt is used   //A3; //17 = analog 3   Pin wird auf Low gezogen wenn Brenner angeht und das Relais schliesst
const byte PIN_ENABLE_RS485 = 3;
const byte PIN_SOFTSERIAL_RX = 10; // receive pin,
const byte PIN_SOFTSERIAL_TX = 11; // transmit pin

const byte PIN_LED_LIFECHK = 4; //  grün extern
const byte PIN_LED_BurnIndicator = A1;//9;



//extern const byte PIN_ENABLE_RS485;
//extern const byte  BurnIndicatorLEDPin;
//extern const byte interruptPin;
//extern const unsigned long BAUD_RATE;
//extern const byte RECEIVE_BUFFER_SIZE; //
extern SoftwareSerial softserial;  // receive pin, transmit pin
extern RS485 myChannel;  //



extern bool BurnerState_idle;         // Brennerzustand initial aus - Relaiskontakt offen
extern bool lastBurnerState_idle;

// Variables
extern unsigned long gesamt_brenndauer;
extern tmElements_t tm;
extern int Temperature10 ;    // -900 is invalid
extern int Temperature10_vorlauf ;    // -900 is invalid
extern int max_T10;  // new
extern int min_T10;  // new
extern int max_T10_vorlauf;  // new  Vorlauf
extern int min_T10_vorlauf;  // new  Vorlauf
extern byte temp_meas_count;
extern int average_temp;
// bool send_current_T = 0;    // send once the current Temperature reading


//   ************    Initialize  *******************



// Temperatur Sensor Dallas 18B20P
extern OneWire oneWire;
extern DallasTemperature sensors;


//byte lasthour = 0;  // remembers last hour


// Variables for day average
extern unsigned long gesamt_brenndauer_last_day;
extern int  lastday;



// Datastructure for transmission
// command 0: test transmission 1: 24h log - average temperature, burntime day, current temperature
// 2: Transmit only current temperature to Master  3:  current burntime & temperature


//struct BurnDataStructx       // declaration
//{
//  byte command = 0;
//  unsigned long gesamt_brenndauer = 0;
//  unsigned int burn_day = 0;
//  unsigned int current_burntime = 0;   // burntime in 0.1s units
//  int average_temp = -900;
//  int current_temperature = -900;
//  int max_T10 = -32768 ;  //
//  int min_T10 =  32767;  //
//  byte temp_meas_count = 0;
//  byte bad_transmit_count = 0;
//  int vorlauf = -900;
//  int vorlauf_max = -900;
//  int vorlauf_min = -900;
//};


struct device_configuration_burner_struct
{
  byte command = 0;   // 0: neue konfiguration   1: transmit current temperature
  byte dcVersion = 0;
  bool  transmit_each_burn = false;
  unsigned int t_meas_sensors = 20; 	// intervall in s  read all local sensors
  unsigned int t_publish_sensors = 20;	// intervall in s   provide data at Serial Interface
} ;

extern struct device_configuration_burner_struct dc;   // Datastructure for device configuration Burner Control

extern byte bad_transmit_count;
extern byte T_fail_count;  // failed temperature meeasssurements

// #ifdef DEBUG1  // #endif

// global variables
// char strBuffer20[20] = "blabla";
extern bool Firstrun;

// unsigned int current_burntime = 0;   // current burntime in 0.1 s   123 = 12.3 sec

// timer
extern unsigned long timenow;
extern unsigned long last_time_Burn_On; // Burntime counter
extern unsigned long last_time_display; //
extern unsigned long last_time_temp_meas; //
extern unsigned long last_time_transmit_T;  // temperature transmit
extern unsigned long last_time_average_Temp;


// eeprom   Location of data in the eeprom.
extern const byte addr_gesamt_brenndauer;    // NEW address
extern const byte addr_gesamt_brenndauer_last_day ;   // gesamt_brenndauer_last_day
extern const byte addr_temp_meas_count;      // temp_meas_count
extern const byte addr_average_temp;                 // average_temp
extern const byte addr_min_T10;
extern const byte addr_max_T10;





// ****** Neu Brennercontrol    *********************************************

//extern struct CurrentBurntimeStruct CurrentBurntime;


struct all_result_data_objects
{
	struct Transmit_Sensors_Struct sensordata;
	struct DayAverageStruct DayAverage;
	struct CurrentBurntimeStruct CurrentBurntime;
};
extern struct all_result_data_objects result;

bool getTemp10_index(int &temperature1, byte index);
void check_serial();

// ****** ENDE  Neu Brennercontrol    *********************************************







// ***************************************************************************************************************************



#line 187 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/Brennerlog_6.0.ino"
void dummy();
#line 1 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/01_setup.ino"
void setup();
#line 1 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/02_main.ino"
void loop();
#line 2 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/03_Radio.ino"
//void check_radio();
//#line 70 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/03_Radio.ino"
//bool transmit(struct BurnDataStruct &sens1);
//#line 99 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/03_Radio.ino"
//void init_radio();
//#line 4 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//bool getTemp10x(int &temperature , int &temperature2);
//#line 30 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//void write_info();
//#line 44 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//void Write_Logline_day(tmElements_t tm1, int average_temp, unsigned int tages_brenndauer, unsigned long gesamt_brenndauer, int temp_meas_count);
//#line 82 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//void Write_Logline_detailed(tmElements_t tm1, int temp2, unsigned long brenndauer, unsigned long gesamt_brenndauer);
//#line 125 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//void success_info();
//#line 133 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//void fail_info();
//#line 141 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/04_functions.ino"
//byte joystick();
//#line 1 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/Burner.ino"
void check_burner();
//void check_burner_isr_int0();
void checkSerial_incoming_msg();
void measure_sensors(unsigned int interval_s, bool Firstrun);
void update_sensor_data(unsigned int interval_s, bool Firstrun);
//void evaluate_joystick();
//#line 111 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/Burner.ino"
//void update_display(unsigned long gesamt_brenndauer, int Temperature10 );
//#line 187 "/home/matthias/Arduino/00 Sketch/00 Brennerlogger/Brennerlog_6.0/Brennerlog_6.0.ino"
//


//Do not add code below this line
#endif /* _Brennercontrol_H_ */
