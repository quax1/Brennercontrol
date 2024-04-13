/*
 * serialcontrol.cpp
 *
 *  Created on: 18.02.2024
 *      Author: matthias
 */
#include "Brennercontrol.h"


void transmit_buf(void* vptr_buf, unsigned int bufsize){
	delay (5);  // give the master a moment to prepare to receive
	digitalWrite (ENABLE_PIN, HIGH);  // enable sending
	myChannel.sendMsg (vptr_buf, bufsize);
	digitalWrite (ENABLE_PIN, LOW);  // disable sending
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

void checkSerial(){

	if (myChannel.update ())
	{
		Serial.println("incoming msg");

		unsigned int msg_size = myChannel.getLength ();
		byte buf [msg_size];
		memset (&buf, 0, msg_size);
		memcpy(buf, myChannel.getData (), msg_size );

		db_px("receiver buf [0]: ", buf [0]);
		db_px("sender   buf [1]: ", buf [1]);
		db_px("command  buf [2]: ", buf [2]);


		if (buf [0] != 1) {
			Serial.println("not my device");
			return;  // not my device
		}

		//		struct Transmit_Sensors_Struct       // declaration
		//		{
		//			byte receiver = 6;        // An   1: master, 2: brennercontrol
		//			byte sender = 7;          // Von  1: master, 2: brennercontrol
		//			byte command = 8;                 // 0:empty answer,  1: current sensor values, 2: day average
		//			byte bad_transmit_count = 9;
		//			byte transmitted_flag = 0;    // 0 not yet transmitted
		//			int current_temperature = -900;
		//			int vorlauf = -900;
		//			int vorlauf_max = -900;
		//			int vorlauf_min = -900;
		//		};
		//
		//		struct Transmit_Sensors_Struct Transmit_Sensors1;

		//  command byte...
		switch (buf [2]) {
		case 1:    //
		{
			db_pln("** cmd 1 received - request all data");
			// transmitted_flag:  100 sensor data, 101 CurrentBurntime, 102  DayAverage, 250  end message

			if (sensordata.transmitted_flag == 0) {    //not yet transmitted
				db_pln("send sensor data");
				sensordata.transmitted_flag = 100;
				transmit_buf(&sensordata, sizeof(sensordata));
				delay(100); // Ausreichend um buffer im empfänger zu leeren
			}

			if (CurrentBurntime.transmitted_flag == 0) {
				db_pln("send CurrentBurntime");
				CurrentBurntime.transmitted_flag = 101;
				transmit_buf(&CurrentBurntime, sizeof(CurrentBurntime));
				delay(100); // Ausreichend um buffer im empfänger zu leeren
			}

			if (DayAverage.transmitted_flag == 0) {
				db_pln("send DayAverage");
				DayAverage.transmitted_flag = 102;
				transmit_buf(&DayAverage, sizeof(DayAverage));
				delay(100); // Ausreichend um buffer im empfänger zu leeren
			}

			// send end message
			struct NoDataMsg_Struct       // declaration
			{
				byte receiver = 1;        // An   1: master, 2: brennercontrol
				byte sender = 2;          // Von  1: master, 2: brennercontrol
				byte command = 0;                 // 0:empty answer,  1: current sensor values, 2: day average
				byte bad_transmit_count = 0;
				byte transmitted_flag = 250;    // 0 not yet transmitted
			};
			struct NoDataMsg_Struct  NoDataMsg;
			db_pln("last msg - No Data");
			NoDataMsg.transmitted_flag = 250;
			transmit_buf(&NoDataMsg, sizeof(NoDataMsg));
			delay(100); // Ausreichend um buffer im empfänger zu leeren

		}
		break;

		case 2:    //
		{
			db_pln("** cmd 2 received - rxxxx");
		}
		break;





		} // end of switch
	} // mychannel update

} //checkSerial




