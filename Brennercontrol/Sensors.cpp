/*
 * Senors.cpp
 *
 *  Created on: 15.02.2024
 *      Author: matthias
 */

#include "Brennercontrol.h"

void update_sensor_data(unsigned int interval_s, bool Firstrun){
	timenow = millis();
	static unsigned long last_time_this = timenow;
	if ( ( ((timenow - last_time_this) > 1L * interval_s * 1000L) || (Firstrun) ) && BurnerState_idle  )  {
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

		result.sensordata.transmitted_flag = 0;
		result.sensordata.current_temperature = Temperature10;
		result.sensordata.vorlauf = Temperature10_vorlauf;
		//result.sensordata.vorlauf_max	= max_T10_vorlauf;
		//result.sensordata.vorlauf_min = min_T10_vorlauf;

	    // reset  counters
	    max_T10_vorlauf = -32768 ;
	    min_T10_vorlauf =  32767;

		//dumpddata
		db_px("sensordata.receiver ", result.sensordata.receiver);
		db_px("sensordata.sender ", result.sensordata.sender);
		db_px("sensordata.command ", result.sensordata.command);
		//db_px("sensordata.bad_transmit_count ", result.sensordata.bad_transmit_count);
		db_px("sensordata.transmitted_flag ", result.sensordata.transmitted_flag);
		db_px("sensordata.current_temperature ", result.sensordata.current_temperature);
		db_px("sensordata.vorlauf ", result.sensordata.vorlauf);
		//db_px("sensordata.vorlauf_max ", result.sensordata.vorlauf_max);
		//db_px("sensordata.vorlauf_min ", result.sensordata.vorlauf_min);


	}

} // end publish_sensors

void measure_sensors(unsigned int interval_s, bool Firstrun){
	// checks all sensors

	timenow = millis();
	if ( ( ((timenow - last_time_temp_meas) > 1L * interval_s * 1000L) || (Firstrun) ) && BurnerState_idle  )  {
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
		if ( getTemp10_index(Temperature10_vorlauf, 1) )    // meas. is valid
		{
			T_fail_count = 0;
			//  store max and min temperature of Vorlauf
			if (Temperature10_vorlauf > max_T10_vorlauf) max_T10_vorlauf = Temperature10_vorlauf;
			if (Temperature10_vorlauf < min_T10_vorlauf) min_T10_vorlauf = Temperature10_vorlauf;

			db_px("T Vorlauf act: ", Temperature10_vorlauf);
			db_px("T Vorlauf max ", max_T10_vorlauf);
			db_px("T Vorlauf min ", min_T10_vorlauf);
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



