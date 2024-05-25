#include "Brennercontrol.h"

//   ************   Define Pins   *******************

//   ************   Load Libraries   *******************



#ifdef RADIO
	RF24 radio(A1, A2);  //http://tmrh20.github.io/RF24/
#endif
// radio must be attached and powered otherwise "radio.available()" will stuck - even if failure handling is enabled

// Constants and Pins
//const byte PIN_BRENNER_RELAIS = A0; //A3; // must be 2 if interupt is used   //A3; //17 = analog 3   Pin wird auf Low gezogen wenn Brenner angeht und das Relais schliesst
//const byte interruptPin = BrennerSensorPin;              //interrupt 0, pin D2    jetzt BrennerSensorPin
//const byte BurnIndicatorLEDPin = A1;//9;
//

// Soft Serial
//const byte PIN_ENABLE_RS485 = 3;
//const unsigned long BAUD_RATE = 9600;  // 9600
const byte RECEIVE_BUFFER_SIZE = 50;  //

SoftwareSerial softserial (PIN_SOFTSERIAL_RX, PIN_SOFTSERIAL_TX);  // receive pin, transmit pin
void fWrite (const byte what)
{
	softserial.write (what);
}
int fAvailable ()
{
	return softserial.available ();
}
int fRead ()
{
	return softserial.read ();
}
RS485 myChannel (fRead, fAvailable, fWrite, RECEIVE_BUFFER_SIZE);  //a maximum buffer length (this only applies for reading).


//   ************    Initialize  *******************
// Temperatur Sensor Dallas 18B20P
// #define ONE_WIRE_BUS 8 //8 alternative 16
OneWire oneWire(PIN_ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);




bool BurnerState_idle = true; // Brennerzustand initial aus - Relaiskontakt offen sieh burner.cpp
bool lastBurnerState_idle = true;

// Variables
unsigned long gesamt_brenndauer = 0;
//tmElements_t tm;
int Temperature10 = -900;    // -900 is invalid
int Temperature10_vorlauf = -900;    // -900 is invalid
int max_T10 = -32768;  // new
int min_T10 = 32767;  // new
int max_T10_vorlauf = -32768;  // new
int min_T10_vorlauf = 32767;  // new
byte temp_meas_count = 0;
int average_temp = 0;
// bool send_current_T = 0;    // send once the current Temperature reading



// ****** Neu Brennercontrol    *********************************************

// Datastructure for transmission
//struct DayAverageStruct
//{
//	  byte receiver = 1;				//An    1: master, 2: brennercontrol
//  byte sender = 2;					//Von   1: master, 2: brennercontrol
//  byte command = 2;               	// 0:empty answer,  1: current sensor values, 2: day average
//  byte bad_transmit_count = 0;
//  byte temp_meas_count = 0;
//  byte transmitted_flag = 0;		// 0 not yet transmitted
//  int average_temp = -900;
//  int max_T10 = -32768 ;  //
//  int min_T10 =  32767;  //
//  unsigned long gesamt_brenndauer = 0;
//  unsigned int burn_day = 0;
//};
// struct DayAverageStruct DayAverage;


//struct Transmit_Sensors_Struct       // declaration
//{
//  byte receiver = 1;				// An   1: master, 2: brennercontrol
//  byte sender = 2;					// Von  1: master, 2: brennercontrol
//  byte command = 2;               	// 0:empty answer,  1: current sensor values, 2: day average
//  byte bad_transmit_count = 0;
//  byte transmitted_flag = 0;		// 0 not yet transmitted
//  int current_temperature = -900;
//  int vorlauf = -900;
//  int vorlauf_max = -900;
//  int vorlauf_min = -900;
//};

 // struct Transmit_Sensors_Struct result.sensordata;

// struct CurrentBurntimeStruct       // declaration
// {
// 	byte receiver = 1;				//An    1: master, 2: brennercontrol
// 	byte sender = 2;				//Von   1: master, 2: brennercontrol
// 	byte command = 3;               // 0:empty answer,  1: current sensor values, 2: day average, 3: current burntime
// 	byte bad_transmit_count = 0;
// 	byte transmitted_flag = 0;		// 0 not yet transmitted
//
// 	unsigned long gesamt_brenndauer = 0;
// 	unsigned int current_burntime 	= 0;   // burntime in 0.1s units
// };


// struct CurrentBurntimeStruct CurrentBurntime;
struct all_result_data_objects result;


//byte lasthour = 0;  // remembers last hour

// Variables for day average
unsigned long gesamt_brenndauer_last_day = 0;
int lastday = 0;

// Datastructure for transmission
// command 0: test transmission 1: 24h log - average temperature, burntime day, current temperature
// 2: Transmit only current temperature to Master  3:  current burntime & temperature
//struct BurnDataStruct_alt
//{
//  int average_temp = -900;
//  int current_temperature = -900;    // new
//  unsigned int burn_day = 0;
//  byte temp_meas_count = 0;
//  unsigned long gesamt_brenndauer = 0;  // new
//  byte bad_transmit_count = 0;  // new
//  byte command = 0;
//  unsigned int current_burntime = 0;   // burntime in 0.1s units
//};

//struct BurnDataStruct
//{
//  byte command = 0;
//  unsigned long gesamt_brenndauer = 0;
//  unsigned int burn_day = 0;
//  unsigned int current_burntime = 0;   // burntime in 0.1s units
//  int average_temp = -900;
//  int current_temperature = -900;
//  int max_T10 = -32768 ;  // new
//  int min_T10 =  32767;  // new
//  byte temp_meas_count = 0;
//  byte bad_transmit_count = 0;
//};

// Datastructure for device configuration heating
/*
struct device_configuration
{
  byte command = 0;   // 0: neue konfiguration   1: transmit current temperature
  byte dcVersion = 0;
  bool  transmit_each_burn = false;
  unsigned int t_meas_sensors = 20; 	// intervall in s  read all local sensors
  unsigned int t_publish_sensors = 20;	// intervall in s   provide data at Serial Interface
} ; */
struct device_configuration_burner_struct dc;

byte bad_transmit_count = 0;
byte T_fail_count = 0;  // failed temperature meeasssurements

// #ifdef DEBUG1  // #endif

// global variables
// char strBuffer20[20] = "blabla";
//bool firstloop = true;

// unsigned int current_burntime = 0;   // current burntime in 0.1 s   123 = 12.3 sec

// timer
unsigned long timenow;
unsigned long last_time_Burn_On; // Burntime counter
unsigned long last_time_display; //
unsigned long last_time_temp_meas; //
unsigned long last_time_transmit_T;  // temperature transmit
unsigned long last_time_average_Temp;

// eeprom   Location of data in the eeprom.
const byte addr_gesamt_brenndauer = 10;    // NEW address            
const byte addr_gesamt_brenndauer_last_day = addr_gesamt_brenndauer
		+ sizeof(gesamt_brenndauer);   // gesamt_brenndauer_last_day
const byte addr_temp_meas_count = addr_gesamt_brenndauer_last_day
		+ sizeof(gesamt_brenndauer_last_day);      // temp_meas_count
const byte addr_average_temp = addr_temp_meas_count + sizeof(temp_meas_count); // average_temp
const byte addr_min_T10 = addr_average_temp + sizeof(average_temp);
const byte addr_max_T10 = addr_min_T10 + sizeof(min_T10);

// ***************************************************************************************************************************

void dummy() {
}
;
