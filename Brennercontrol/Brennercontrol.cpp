#include "Brennercontrol.h"


// Subs  test

void Day_average_sampling(bool Firstrun) {
			// *****   make a measurement every 30 min for average day temperature

	timenow = millis();
	if (( ((timenow - last_time_average_Temp) > (long)dc.mean_T_sample_intervall_s  * 1000L) || (Firstrun) )  && BurnerState_idle  ) {
		last_time_average_Temp = timenow;
		db_p("*average"); db_m;


		if (Temperature10 != -900) {
			temp_meas_count++;
			average_temp += Temperature10;
			EEPROM.put( addr_temp_meas_count , temp_meas_count );
			EEPROM.put( addr_average_temp , average_temp );
			EEPROM.put( addr_min_T10 ,  min_T10 );
			EEPROM.put( addr_max_T10 ,  max_T10 );
			db_pln("Temp. OK for average");
		}
		else { db_pln("Temp. for average failed"); }
	}
	}



void Day_average_publish(){


	// ******************* its a new day - write average from yesterday
	if (day() != lastday) {
		db_pln("******* new day Provide Day Average ***********");

		 int genug = (90 * 24*60*60) / (dc.mean_T_sample_intervall_s *100);   //90% valid values

		if (temp_meas_count > genug)  average_temp = (average_temp / temp_meas_count) ;      // should be 47 values
		else average_temp = -1000-temp_meas_count; // invalid value

		// Write_Logline_day(tm, average_temp, gesamt_brenndauer - gesamt_brenndauer_last_day, gesamt_brenndauer, temp_meas_count);


		result.DayAverage.transmitted_flag = 0;
		//result.DayAverage.bad_transmit_count = bad_transmit_count;
		result.DayAverage.temp_meas_count = temp_meas_count;
		result.DayAverage.average_temp = average_temp;
		result.DayAverage.max_T10 = max_T10;  // new
		result.DayAverage.min_T10 = min_T10;  // new
		result.DayAverage.gesamt_brenndauer = gesamt_brenndauer;     //new
		result.DayAverage.burn_day = gesamt_brenndauer - gesamt_brenndauer_last_day;


		// reset day counters
		temp_meas_count = 0;
		lastday = day();
		average_temp = 0;
		max_T10 = -32768 ;  // new
		min_T10 =  32767;  // new
		gesamt_brenndauer_last_day = gesamt_brenndauer;

		EEPROM.put( addr_gesamt_brenndauer_last_day , gesamt_brenndauer_last_day );
		EEPROM.put( addr_temp_meas_count , temp_meas_count );
		EEPROM.put( addr_average_temp , average_temp );
		EEPROM.put( addr_min_T10 ,  min_T10 );
		EEPROM.put( addr_max_T10 ,  max_T10 );


	}
}  // day average

void lifecheck_blink_LED(const bool Firstrun){
	// blinks if not stuck
	static unsigned long timer_lifecheck =0 ;
	static bool lifecheck_on = 1 ;
	if (((millis() - timer_lifecheck) > 3000L) || Firstrun) {
		timer_lifecheck = millis();
		//if (Daymode) digitalWrite(PIN_LED_LIFECHK, HIGH);
		digitalWrite(PIN_LED_LIFECHK, HIGH);
		lifecheck_on = true;
	}
	if (((millis() - timer_lifecheck) > 100L) && lifecheck_on) {
		lifecheck_on = false;
		digitalWrite(PIN_LED_LIFECHK, LOW);
	}
}  //lifecheck_blink_LED

void loop()
{
	static bool Firstrun =true;

	// *******  get time and update display
	timenow = millis();
	if (( ((timenow - last_time_display) > 3000L) || (Firstrun) )  && BurnerState_idle  ) {
		last_time_display = timenow;
		//RTC.read(tm);
		//update_display( gesamt_brenndauer, Temperature10 ) ;

		char strBuffer20[20] = "";
		sprintf(strBuffer20, "%02d.%02d.%04d " , day(), month(), year()  ); // Date  10.05.2020
		db_px("date ", strBuffer20);
		sprintf(strBuffer20, "%02d:%02d:%02d " , hour(), minute(), second() );  //  07:58:26
		db_px("  ", strBuffer20);

		db_px("  day ", day());
		db_pxln("  lastday ", lastday);

		if (day() != lastday)
			db_pln("******* new day ***********");




	}  // *****  get time and update display



	// *******  check for new config file or command
	//  check_radio();
	//
	//  evaluate_joystick();
	//

	checkSerial_incoming_msg();
//	dc.t_meas_sensors = 10;

	measure_sensors(dc.t_meas_sensors, Firstrun);    // intervall in s  read all local sensors

	update_sensor_data(dc.t_publish_sensors, Firstrun);   // intervall in s   provide data at Serial Interface

	check_burner();

	Day_average_sampling(Firstrun); // *****   make a measurement every 30 min for average day temperature

	Day_average_publish();  // ******************* its a new day - write average from yesterday

	lifecheck_blink_LED(Firstrun);
	LED_serial_received_timer(false);
	if (Firstrun)  Firstrun = false;


}   // end of main loop





