#include "Brennercontrol.h"


// volatile boolean started = 0;
//volatile unsigned long startTime =0;
//volatile unsigned long endTime = 0;
//volatile boolean BurnerState_idle = true;
//
//// interrupt service routine
//void check_burner_isr_int0()
//{
//  BurnerState_idle = (digitalRead(BrennerSensorPin) == HIGH);      // LOW=false   Brenner hat eingeschaltet
//  if (!BurnerState_idle) {       // Brenner hat eingeschaltet
//	  startTime = millis ();
//	  Serial.println("Brenner Ein");
//	  digitalWrite(BurnIndicatorLEDPin, HIGH);
//  }
//  else {   // Brenner hat ausgeschaltet
//	  endTime = millis ();
//	  Serial.println("Brenner Aus");
//	  digitalWrite(BurnIndicatorLEDPin, LOW);
//  }
//} // check_burner_isr_int0()
//



// Camera shutter speed example  http://gammon.com.au/interrupts

//void check_burnerx()
//{
//	return;
//
//	  if (endTime)  // Burning completed
//	  {
//	    Serial.print ("Bun completed: ms: ");
//
//	    Serial.println (endTime - startTime);
//	    db_px("startTime ", startTime);
//	    db_px("endTime ", endTime);
//
//	    unsigned long current_burntime = (endTime - startTime) ;
//	    endTime = 0;
//	    startTime = 0;
//
//	    if (current_burntime < 200) return;  // jitter - real values are more than 200 ms
//
//		current_burntime = round( (float)current_burntime / 100) ; // burntime in unit 0.1s
//		Serial.print("current_burntime: "); Serial.println(current_burntime);
//		gesamt_brenndauer = gesamt_brenndauer +  round( (float)current_burntime / 10) ;    // wert in s
//		Serial.print ("gesamt_brenndauer: ");  Serial.println (gesamt_brenndauer);
//		EEPROM.put(addr_gesamt_brenndauer, gesamt_brenndauer);
//
//
//		// create data structure
//		CurrentBurntime.bad_transmit_count 	= bad_transmit_count;
//		CurrentBurntime.transmitted_flag 	= 0;		// 0 not yet transmitted
//		CurrentBurntime.gesamt_brenndauer 	= gesamt_brenndauer;
//		CurrentBurntime.current_burntime 	= current_burntime;     // long to int - but OK
//
//	  }
//}

void check_burner()
{
//return;
	// Check Burner State Change ****************************************************

	BurnerState_idle = (digitalRead(BrennerSensorPin) == HIGH);      // LOW=false   Brenner hat eingeschaltet
	// compare the BurnerState to its previous state

	if (!(BurnerState_idle == lastBurnerState_idle)) {
		// Serial.println("state change ");
		if (! BurnerState_idle) {
			// Brenner hat eingeschaltet
			Serial.println("Brenner Ein"); digitalWrite(BurnIndicatorLEDPin, HIGH);
			// RTC.read(tm_on);
			timenow = millis();
			last_time_Burn_On = timenow;


		} else {
			// Brenner hat ausgeschaltet
			Serial.println("Brenner Aus"); digitalWrite(BurnIndicatorLEDPin, LOW);

			timenow = millis();
			unsigned long current_burntime = (timenow - last_time_Burn_On)   ;  // burntime in unit 0.1s

			if (current_burntime > 200)    {       // otherwise jitter on input

				// RTC.read(tm);
				// unsigned int brenndauer = round( (float)current_burntime / 10) ; //  makeTime(tm) - makeTime(tm_on);

				current_burntime = round( (float)current_burntime / 100) ; // burntime in unit 0.1s
				Serial.print("current_burntime: "); Serial.println(current_burntime);
				gesamt_brenndauer = gesamt_brenndauer +  round( (float)current_burntime / 10) ;;
				Serial.print ("gesamt_brenndauer: ");  Serial.println (gesamt_brenndauer);
				EEPROM.put(addr_gesamt_brenndauer, gesamt_brenndauer);


				//write detailed logline to SD card
				//db_p("*write logline"); db_m;
				// Write_Logline_detailed(tm, Temperature10, current_burntime, gesamt_brenndauer);



				//        struct CurrentBurntimeStruct       // declaration
				//        {
				//        	byte receiver = 1;				//An    1: master, 2: brennercontrol
				//        	byte sender = 2;				//Von   1: master, 2: brennercontrol
				//        	byte command = 3;               // 0:empty answer,  1: current sensor values, 2: day average, 3: current burntime
				//        	byte bad_transmit_count = 0;
				//        	byte transmitted_flag = 0;		// 0 not yet transmitted
				//
				//        	unsigned long gesamt_brenndauer = 0;
				//        	unsigned int current_burntime 	= 0;   // burntime in 0.1s units
				//        };
				//        struct CurrentBurntimeStruct CurrentBurntime;

				// create data structure
				CurrentBurntime.bad_transmit_count 	= bad_transmit_count;
				CurrentBurntime.transmitted_flag 	= 0;		// 0 not yet transmitted
				CurrentBurntime.gesamt_brenndauer 	= gesamt_brenndauer;
				CurrentBurntime.current_burntime 	= current_burntime;     // long to int - but OK

			}
		}
	}
	// save the current state as the last state,
	lastBurnerState_idle = BurnerState_idle;
}



//
//void evaluate_joystick()
//{
//	byte joystick_value = joystick();
//	// 1 push  3 up  4 down   5 left  6 right
//
//	if (joystick_value == 3) {          // Up - RF Test
//		Serial.println("RF Testmode");
//		// Transmit to Master
//		struct BurnDataStruct burnlog;
//		// burnlog.average_temp = 11; // average_temp;
//		burnlog.current_temperature = Temperature10;         // new
//		// burnlog.burn_day = 0; // gesamt_brenndauer - gesamt_brenndauer_last_day;
//		// burnlog.temp_meas_count = 0; // temp_meas_count;
//		burnlog.gesamt_brenndauer = gesamt_brenndauer;     //new
//		burnlog.bad_transmit_count = bad_transmit_count;                   //new
//		burnlog.command = 0;                        // 0 test transmission
//		// burnlog.current_burntime = 11;
//
//		for (int index = 1; index <= 8; index++) {
//			if ( transmit(burnlog) ) {
//				Serial.println("SendOK");
//				bad_transmit_count = 0;
//			}
//			else {
//				bad_transmit_count++;
//			}
//			delay(1000);
//		}
//	}
//
//	if (joystick_value == 1) {    // switch on LCD Backlight
//		digitalWrite(7, HIGH);
//		delay(5000);
//		digitalWrite(7, LOW);
//	}
//}
//
//
//
//



void update_display(unsigned long  gesamt_brenndauer, int Temperature10  )
{

	//  char strBuffer20[20] = "";
	//  // date
	//  sprintf(strBuffer20, "%02d.%02d.%04d " , tm.Day, tm.Month, tmYearToCalendar(tm.Year)  ); // Date  10.05.2020
	//  lcd.LCD_write_string(0, 0, strBuffer20, MENU_NORMAL );
	//
	//  // time
	//  sprintf(strBuffer20, "%02d:%02d:%02d " , tm.Hour, tm.Minute, tm.Second  );  //  07:58:26
	//  lcd.LCD_write_string(0, 1, strBuffer20, MENU_NORMAL );
	//  //Serial.print("Time: "); Serial.println(strBuffer20);
	//
	//  // burntime
	//  sprintf(strBuffer20, "%ld BT" , gesamt_brenndauer );
	//  lcd.LCD_write_string(0, 2, strBuffer20, MENU_NORMAL );
	//
	//  // temperature
	//  dtostrf(Temperature10 / 10, 1, 1, strBuffer20);
	//  lcd.LCD_write_string(0, 4, strBuffer20, MENU_NORMAL );
	//  lcd.LCD_write_string(42, 4, "Temp", MENU_NORMAL );
}

//void Date2Buffer (const tmElements_t  &tm1) {
//  sprintf(strBuffer20, "%02d.%02d.%04d " , tm1.Day, tm1.Month, tmYearToCalendar(tm1.Year)  ); // Date  10.05.2020
//};
//
//void Time2Buffer (const tmElements_t  &tm1) {
//  sprintf(strBuffer20, "%02d:%02d:%02d " , tm1.Hour, tm1.Minute, tm1.Second  );  //  07:58:26
//};
