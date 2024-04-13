#include "Brennercontrol.h"

void loop()
{
  // delay(300);
  // Serial.print("gesamt\n"); Serial.println (gesamt_brenndauer);
  // Serial.print("sizeof burnlog:  "); Serial.println(sizeof(burnlog));
	//timenow = millis();

	//db_px("diff",millis()-timenow);

  // *******  get time and update display
  timenow = millis();
  if (( ((timenow - last_time_display) > 3000L) || (firstloop) )  && BurnerState_idle  ) {
    last_time_display = timenow;
    //db_pln("display"); db_m;
    RTC.read(tm);
    //update_display( gesamt_brenndauer, Temperature10 ) ;


	  char strBuffer20[20] = "";
	  // date
	  sprintf(strBuffer20, "%02d.%02d.%04d " , tm.Day, tm.Month, tmYearToCalendar(tm.Year)  ); // Date  10.05.2020
//	   db_px("date ", strBuffer20);

	  // time
	  sprintf(strBuffer20, "%02d:%02d:%02d " , tm.Hour, tm.Minute, tm.Second  );  //  07:58:26
//	  db_px("time ", strBuffer20);



  }  // *****  get time and update display



  // *******  check for new config file or command
//  check_radio();
//
//  evaluate_joystick();

	checkSerial();
	measure_sensors(20);    // intervall in s
	publish_sensors(20);   // intervall in s

check_burner();


  // *****   make a measurement every 30 min for average day temperature

  timenow = millis();
  if (( ((timenow - last_time_average_Temp) > 30L * 60L * 1000L) || (firstloop) )  && BurnerState_idle  ) {
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


  // ******************* its a new day - write average from yesterday
  if (tm.Day != lastday) {
    db_pln("new day....");

    if (temp_meas_count > 44)  average_temp = (average_temp / temp_meas_count) ;      // should be 47 values
    else average_temp = -900; // invalid value

   // Write_Logline_day(tm, average_temp, gesamt_brenndauer - gesamt_brenndauer_last_day, gesamt_brenndauer, temp_meas_count);


    // Datastructure for transmission

//    struct DayAverageStruct
//    {
//      byte receiver = 1;				//An    1: master, 2: brennercontrol
//      byte sender = 2;					//Von   1: master, 2: brennercontrol
//      byte command = 2;               	// 0:empty answer,  1: current sensor values, 2: day average
//      byte bad_transmit_count = 0;
//      byte transmitted_flag = 0;		// 0 not yet transmitted
//      byte temp_meas_count = 0;
//      int average_temp = -900;
//      int max_T10 = -32768 ;  //
//      int min_T10 =  32767;  //
//      unsigned long gesamt_brenndauer = 0;
//      unsigned int burn_day = 0;
//    };
//	struct DayAverageStruct DayAverage;


    DayAverage.transmitted_flag = 0;
	DayAverage.bad_transmit_count = bad_transmit_count;
	DayAverage.temp_meas_count = temp_meas_count;
	DayAverage.average_temp = average_temp;
	DayAverage.max_T10 = max_T10;  // new
	DayAverage.min_T10 = min_T10;  // new
	DayAverage.gesamt_brenndauer = gesamt_brenndauer;     //new
	DayAverage.burn_day = gesamt_brenndauer - gesamt_brenndauer_last_day;




    // reset day counters
    temp_meas_count = 0;
    lastday = tm.Day;
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


  if (firstloop)  firstloop = false;

}   // end of main loop
