/*
 * Senors.cpp
 *
 *  Created on: 15.02.2024
 *      Author: matthias
 */

#include "Brennercontrol.h"


DHT_Sensors DHT1(PIN_DHT1, 1);


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

		result.sensordata.DHT1_T10  	= ds.DHT1_T10;
		result.sensordata.DHT1_humidity = ds.DHT1_humidity;
		result.sensordata.succ_count 	= ds.succ_count;
		result.sensordata.sensor_id 	= ds.sensor_id;
	    // reset  counters
	    max_T10_vorlauf = -32768 ;
	    min_T10_vorlauf =  32767;

		//dumpddata
		db_pxln("sensordata.receiver ", result.sensordata.receiver);
		db_pxln("sensordata.sender ", result.sensordata.sender);
		db_pxln("sensordata.command ", result.sensordata.command);
		//db_pxln("sensordata.bad_transmit_count ", result.sensordata.bad_transmit_count);
		db_pxln("sensordata.transmitted_flag ", result.sensordata.transmitted_flag);
		db_pxln("sensordata.current_temperature ", result.sensordata.current_temperature);
		db_pxln("sensordata.vorlauf ", result.sensordata.vorlauf);
		//db_pxln("sensordata.vorlauf_max ", result.sensordata.vorlauf_max);
		//db_pxln("sensordata.vorlauf_min ", result.sensordata.vorlauf_min);


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
		if ( getTemp10_index(Temperature10, 0, sensors) )    // meas. is valid
		{
			T_fail_count = 0;
			//  store max and min temperature value
			if (Temperature10 > max_T10) max_T10 = Temperature10;
			if (Temperature10 < min_T10) min_T10 = Temperature10;
			db_pxln("T aussen: ", Temperature10);
		}
		else T_fail_count++;
		if (T_fail_count > 7 ) { Temperature10 = -900; T_fail_count = 0; }                        // die letzten Messungen waren alle schlecht, keine werte für das berechnen des durchschnitts


		// Vorlauf
	//	if ( getTemp10_index(Temperature10_vorlauf, 1, sensors2) )    // meas. is valid  @mn
		if ( getTemp10_index(Temperature10_vorlauf, 1, sensors) )    // meas. is valid
		{
			T_fail_count = 0;
			//  store max and min temperature of Vorlauf
			if (Temperature10_vorlauf > max_T10_vorlauf) max_T10_vorlauf = Temperature10_vorlauf;
			if (Temperature10_vorlauf < min_T10_vorlauf) min_T10_vorlauf = Temperature10_vorlauf;

			db_pxln("T Vorlauf act: ", Temperature10_vorlauf);
			db_pxln("T Vorlauf max ", max_T10_vorlauf);
			db_pxln("T Vorlauf min ", min_T10_vorlauf);
		}
		int t; int h;
	 DHT1.read_DHT_valuesx(ds.DHT1_T10, ds.DHT1_humidity);
	// 	DHT1.read_DHT_valuesx(t, h);
	//		DHT1.read_DHT_simple(t, h);
		ds.succ_count = DHT1.successCountx;
		ds.sensor_id = DHT1.dev_nr;
	}



} // measure_sensors

