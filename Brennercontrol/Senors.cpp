/*
 * Senors.cpp
 *
 *  Created on: 15.02.2024
 *      Author: matthias
 */

#include "Brennercontrol.h"

void publish_sensors(unsigned int interval_s){
	timenow = millis();
	static unsigned long last_time_this = timenow;
	if ( ( ((timenow - last_time_this) > 1L * interval_s * 1000L) || (firstloop) ) && BurnerState_idle  )  {
		last_time_this = timenow;

		db_p("publish sensor data");

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

		//struct Transmit_Sensors_Struct sensordata;

		sensordata.transmitted_flag = 0;
		sensordata.current_temperature = Temperature10;
		sensordata.vorlauf = Temperature10b;
		sensordata.vorlauf_max	= max_T10b;
		sensordata.vorlauf_min = min_T10b;

	    // reset  counters
	    max_T10b = -32768 ;
	    min_T10b =  32767;

		//dumpddata
		db_px("sensordata.receiver ", sensordata.receiver);
		db_px("sensordata.sender ", sensordata.sender);
		db_px("sensordata.command ", sensordata.command);
		db_px("sensordata.bad_transmit_count ", sensordata.bad_transmit_count);
		db_px("sensordata.transmitted_flag ", sensordata.transmitted_flag);
		db_px("sensordata.current_temperature ", sensordata.current_temperature);
		db_px("sensordata.vorlauf ", sensordata.vorlauf);
		db_px("sensordata.vorlauf_max ", sensordata.vorlauf_max);
		db_px("sensordata.vorlauf_min ", sensordata.vorlauf_min);


	}

} // end publish_sensors

void measure_sensors(unsigned int interval_s){
	// checks all sensors

	timenow = millis();
	if ( ( ((timenow - last_time_temp_meas) > 1L * interval_s * 1000L) || (firstloop) ) && BurnerState_idle  )  {
		last_time_temp_meas = timenow;

		db_p("get temperature");

		//bool getTemp10_index(int &temperature1, byte index) {      //0, 1,

		// Aussen
		if ( getTemp10_index(Temperature10, 0) )    // meas. is valid
		{
			T_fail_count = 0;
			//  store max and min temperature value
			if (Temperature10 > max_T10) max_T10 = Temperature10;
			if (Temperature10 < min_T10) min_T10 = Temperature10;
			db_px("T aussen: ", Temperature10);
		}
		else T_fail_count++;
		if (T_fail_count > 7 ) { Temperature10 = -900; T_fail_count = 0; }                        // die letzten Messungen waren alle schlecht, keine werte für das berechnen des durchschnitts


		// Vorlauf
		if ( getTemp10_index(Temperature10b, 1) )    // meas. is valid
		{
			T_fail_count = 0;
			//  store max and min temperature of Vorlauf
			if (Temperature10b > max_T10b) max_T10b = Temperature10b;
			if (Temperature10b < min_T10b) min_T10b = Temperature10b;

			db_px("Vorlauf Temperature10b: ", Temperature10b);
			db_px("Vorlauf max_T10b: ", max_T10b);
			db_px("Vorlauf min_T10b: ", min_T10b);
		}

//		// assemble message
//		byte msg [] = {
//				1,    // device 1
//				2,    // turn light on
//				3,    // turn light on
//				4    // turn light on
//		};
//		db_p("Start sending");
//		db_px("sizeof(msg): ", sizeof(msg));
//		db_px("ENABLE_PIN): ", ENABLE_PIN);
//		digitalWrite (ENABLE_PIN, HIGH);  // enable sending
//		delay(1000);
//		myChannel.sendMsg (msg, sizeof (msg));
//		delay(1000);                                        // Wait before going back to Receive mode
//		digitalWrite (ENABLE_PIN, LOW);

	}



} // measure_sensors



