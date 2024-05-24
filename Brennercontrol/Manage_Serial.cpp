/*
 * serialcontrol.cpp
 *
 *  Created on: 18.02.2024
 *      Author: matthias
 */
#include "Brennercontrol.h"


void transmit_buf(void* vptr_buf, unsigned int bufsize){
	delay (5);  // give the master a moment to prepare to receive
	digitalWrite (PIN_ENABLE_RS485, HIGH);  // enable sending
	myChannel.sendMsg (vptr_buf, bufsize);
	digitalWrite (PIN_ENABLE_RS485, LOW);  // disable sending
}

//void dump_Transmit_Sensors(struct Transmit_Sensors_Struct Transmit_Sensors){
//    // print all
//    db_px("Transmit_Sensors.receiver ", Transmit_Sensors.receiver);
//    db_px("Transmit_Sensors.sender ", Transmit_Sensors.sender);
//    db_px("Transmit_Sensors.command ", Transmit_Sensors.command);
//    db_px("Transmit_Sensors.bad_transmit_count ", Transmit_Sensors.bad_transmit_count);
//    db_px("Transmit_Sensors.transmitted_flag ", Transmit_Sensors.transmitted_flag);
//    db_px("Transmit_Sensors.current_temperature ", Transmit_Sensors.current_temperature);
//    db_px("Transmit_Sensors.vorlauf ", Transmit_Sensors.vorlauf);
//    db_px("Transmit_Sensors.vorlauf_max ", Transmit_Sensors.vorlauf_max);
//    db_px("Transmit_Sensors.vorlauf_min ", Transmit_Sensors.vorlauf_min);
//}

void LED_serial_received_timer(bool startLED = false){
	// blinks if not stuck
	static unsigned long timer_LED_serial_received =0 ;
	static bool LED_on = 0 ;

	if (startLED){
		digitalWrite(PIN_LED_SERIAL_RECEIVED, HIGH);
		timer_LED_serial_received = millis();
		 LED_on = true ;
		 startLED = false;
	}
	if (((millis() - timer_LED_serial_received) > 5000L) && LED_on) {
		LED_on = false;
		digitalWrite(PIN_LED_SERIAL_RECEIVED, LOW);
	}
}  //LED_serial_received_timer

void checkSerial_incoming_msg(){

	if (myChannel.update ())
	{
		Serial.println("incoming msg");
		 LED_serial_received_timer(true);

		unsigned int msg_size = myChannel.getLength ();
		byte buf [msg_size];
		memset (&buf, 0, msg_size);
		memcpy(buf, myChannel.getData (), msg_size );

		db_px("receiver buf [0]: ", buf [0]);
		db_px("sender   buf [1]: ", buf [1]);
		db_px("command  buf [2]: ", buf [2]);

		byte receiver = buf [0];
		byte sender   = buf [1];
		byte command  = buf [2];

		if (receiver != 2) {
			Serial.println("not my device");
			return;  // not my device
		}


		switch (command) {

		case 1:    //   send all data objects that are new
		{
			db_pln("** cmd 1 received - request all data");
			// transmitted_flag:  100 sensor data, 101 CurrentBurntime, 102  DayAverage, 250  end message

			if (result.sensordata.transmitted_flag == 0) {    //not yet transmitted
				db_pln("send sensor data");
				result.sensordata.transmitted_flag = 100;  // steht im command byte !
				transmit_buf(&result.sensordata, sizeof(result.sensordata));
				delay(100); // Ausreichend um buffer im empfänger zu leeren
			}

			if (result.CurrentBurntime.transmitted_flag == 0) {
				db_pln("********* send CurrentBurntime");
				result.CurrentBurntime.transmitted_flag = 101;
				transmit_buf(&result.CurrentBurntime, sizeof(result.CurrentBurntime));
				delay(100); // Ausreichend um buffer im empfänger zu leeren
			}

			if (result.DayAverage.transmitted_flag == 0) {
				db_pln("**********  send DayAverage ************");
				result.DayAverage.transmitted_flag = 102;
				transmit_buf(&result.DayAverage, sizeof(result.DayAverage));
				delay(100); // Ausreichend um buffer im empfänger zu leeren
			}

			// send end message

			struct Termination_Msg_Struct  EndMsg;
			EndMsg.receiver = 1; 		// An    1: master, 2: brennercontrol
			EndMsg.sender 	= 2; 		// Von  1: master, 2: brennercontrol
			EndMsg.command 	= 0;   		// 0:END message or no Data to send,  1: current sensor values, 2: day average
			db_pln("send END msg");
			//NoDataMsg.transmitted_flag = 250;
			transmit_buf(&EndMsg, sizeof(EndMsg));
			delay(100); // Ausreichend um buffer im empfänger zu leeren

		}
		break;

		case 100:    //
		{
			db_pln("** cmd 100 received - device configuration");
			// Datastructure for device configuration Burner Control
			/*
			struct Rx_device_configuration_burner
			{
			  byte receiver = 1;				// An   1: master, 2: brennercontrol
			  byte sender = 2;					// Von  1: master, 2: brennercontrol
			  byte command = 0;   				// 100: neue konfiguration
			  byte dcVersion = 0;
			  bool  transmit_each_burn = false;
			  unsigned int t_meas_sensors = 20; 	// intervall in s  read all local sensors
			  unsigned int t_publish_sensors = 20;	// intervall in s   provide data at Serial Interface
			} ;
			*/

			// buf is a device configuration - copy buffer into data structure

			struct Rx_device_configuration_burner dc1;
			memcpy(&dc1, &buf, sizeof(dc1));
			dc.transmit_each_burn = 	dc1.transmit_each_burn;
			dc.t_meas_sensors = 		dc1.t_meas_sensors;
			dc.t_publish_sensors = 		dc1.t_publish_sensors;
			dc.dcVersion = 				dc1.dcVersion;

			db_px("dc.transmit_each_burn ", 				dc.transmit_each_burn);
			db_px("dc.measure_sensors_intervall_s ", 	dc.t_meas_sensors);
			db_px("dc.update_sensor_data_intervall_s ", 	dc.t_publish_sensors);
			db_px("dc.dcVersion ", 	  					dc.dcVersion);


			// send end message
			struct Termination_Msg_ConfigOK_Struct  EndMsg;
			EndMsg.receiver = 1;        // An   1: master, 2: brennercontrol
			EndMsg.sender = 2;          // Von  1: master, 2: brennercontrol
			EndMsg.command = 100;                 // 0:empty answer,  1: current sensor values, 2: day average
			EndMsg.version = dc.dcVersion;

			unsigned long  t = now();
			db_px("timet", t);
			EndMsg.unix_time = t;

			db_px("last msg CONFIG- No Data - unix time: ", EndMsg.unix_time);
			transmit_buf(&EndMsg, sizeof(EndMsg));
			delay(100); // Ausreichend um buffer im empfänger zu leeren wichtig !
		}
		break;

		} // end of switch
	} // mychannel update

} //checkSerial




