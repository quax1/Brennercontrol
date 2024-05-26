#include "Brennercontrol.h"

// Functions ********************************************




bool getTemp10_index(int &temperature1, byte index) {      //0 sensor 1,
  bool ReadOK = true;
  sensors.begin();
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  pinMode(PIN_ONE_WIRE_BUS, OUTPUT);

  // Get Sensor
  digitalWrite(PIN_ONE_WIRE_BUS, HIGH);
  delay(1000);
  int temperature10_tmp = round(sensors.getTempCByIndex(index) * 10);      //needs calibrated sensors originals no china shit
  db_pxln("\nindex: ", index);
  db_pxln("temperature10_tmp: ", temperature10_tmp);

  if (temperature10_tmp > -400 && temperature10_tmp < 600) {
    temperature1 = temperature10_tmp;   // sometimes the temp. is -126.9 or +85 °C  sensor wrong
    ReadOK = true;
  } else {
    //temperature1 = -900; // -900 signals invalid result
    Serial.println("T. meas. failed");
    ReadOK = false;
  }

  return ReadOK;
}  // getTemp10_index


//
//
//bool getTemp10xalt(int &temperature1, int &temperature2) {
//  bool ReadOK = true;
//  sensors.begin();
//  sensors.setWaitForConversion(false);
//  sensors.requestTemperatures();
//  pinMode(ONE_WIRE_BUS, OUTPUT);
//
//  // Get Sensor 1  Aussenfühler
//  digitalWrite(ONE_WIRE_BUS, HIGH);
//  delay(1000);
//  int temperature10_tmp = round(sensors.getTempCByIndex(0) * 10);
//  Serial.print("\n** T1: "); Serial.println(temperature10_tmp);
//
//  if (temperature10_tmp > -400 && temperature10_tmp < 600) {
//    temperature1 = temperature10_tmp;   // sometimes the temp. is -126.9 or +85 °C  sensor wrong
//  } else {
//    temperature1 = -900; // -900 signals invalid result
//    Serial.println("T. meas. failed");
//    ReadOK = false;
//  }
//
//
//
//  // Get Sensor 2  Vorlauf
//  digitalWrite(ONE_WIRE_BUS, HIGH);
//  delay(1000);
//  temperature2 = round(sensors.getTempCByIndex(1) * 10);
//
//  sensors.setWaitForConversion(true);
//
//  Serial.print("\n** T1: "); Serial.println(temperature10_tmp);
//  Serial.print("** T2: "); Serial.println(temperature2);
//
//  if (temperature10_tmp > -400 && temperature10_tmp < 600) {
//    temperature2 = temperature10_tmp;   // sometimes the temp. is -126.9 or +85 °C  sensor wrong
//  } else {
//    temperature2 = -900; // -900 signals invalid result
//    Serial.println("T. meas. failed");
//    ReadOK = false;
//  }
//
//
//  return ReadOK;
//}


 



void write_info() {
  // flash LED while writing
  Serial.println("write info");

  digitalWrite(PIN_LED_BurnIndicator, HIGH);
  for (int n = 1; n < 6; n++) {
    delay(100);  // blink to signal write process
    digitalWrite(PIN_LED_BurnIndicator, LOW);
    delay(100);
    digitalWrite(PIN_LED_BurnIndicator, HIGH);
  }
}  // end write_info


void Write_Logline_day(tmElements_t  tm1, int average_temp, unsigned int tages_brenndauer, unsigned long gesamt_brenndauer, int temp_meas_count) {
  /*   */
  // write to SD  *********************************
  #ifdef SDCard    //#endif

  write_info();
  File dataFile = SD.open("log-day.txt", FILE_WRITE);    // Filenamen nicht länger als 8
  if (!dataFile) {
    Serial.println("error opening log-day.txt");
    lcd.LCD_write_string(65, 0, "W?", MENU_NORMAL );
  }
  else {
    Serial.println("write SD: log-day.txt");
    Date2Buffer(tm1); dataFile.print(strBuffer20); dataFile.print("\t"); // Serial.print(strBuffer20); Serial.print("\t");
    Time2Buffer(tm1); dataFile.print(strBuffer20); dataFile.print("\t"); // Serial.print(strBuffer20); Serial.print("\t");

    dataFile.print((float)average_temp / 10);       dataFile.print("\t");
    dataFile.print(tages_brenndauer);   dataFile.print("\t");
    dataFile.print(gesamt_brenndauer);  dataFile.print("\t");
    dataFile.println(temp_meas_count);
    dataFile.close();

#ifdef DEBUG    //#endif
    // same for debug output:
    Serial.print((float)average_temp / 10);       Serial.print("\t");
    Serial.print(tages_brenndauer);   Serial.print("\t");
    Serial.print(gesamt_brenndauer);  Serial.print("\t");
    Serial.println(temp_meas_count);
#endif

    lcd.LCD_write_string(65, 0, "WOK", MENU_NORMAL );
  }
  digitalWrite(BurnIndicatorLEDPin, LOW); // switch off indicator light
  #endif
} // end of Write_Logline_day



void Write_Logline_detailed(tmElements_t  tm1, int temp2, unsigned long brenndauer, unsigned long gesamt_brenndauer) {

#ifdef SDCard    //#endif


  // write to SD  *********************************
  write_info();
  File dataFile = SD.open("log-all.txt", FILE_WRITE);    // Filenamen nicht länger als 8
  if (!dataFile) {
    Serial.println("error opening: log-all.txt");
    lcd.LCD_write_string(65, 0, "W?", MENU_NORMAL );
  }
  else {
    Serial.println("write SD detailed: log-all.txt");
    Date2Buffer(tm1); dataFile.print(strBuffer20); dataFile.print("\t"); // Serial.print(strBuffer20); Serial.print("\t");
    Time2Buffer(tm1); dataFile.print(strBuffer20); dataFile.print("\t"); // Serial.print(strBuffer20); Serial.print("\t");

    dataFile.print((float)temp2 / 10);       dataFile.print("\t");
    dataFile.print((float)brenndauer / 10);     dataFile.print("\t");
    dataFile.println(gesamt_brenndauer);
    dataFile.close();


#ifdef DEBUG    //#endif
    // same for debug output:
    Serial.print(temp2);       Serial.print("\t");
    Serial.print(brenndauer);   Serial.print("\t");
    Serial.println(gesamt_brenndauer);
#endif

    lcd.LCD_write_string(65, 0, "WOK", MENU_NORMAL );
  }
  digitalWrite(BurnIndicatorLEDPin, LOW); // switch off indicator light

  #endif


} // end of Write_Logline_detailed




// success_info(); // fail_info();
void success_info() {

  Serial.println("Success");
  digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(800);                       // wait for a second
  digitalWrite(7, LOW);    // turn the LED off by making the voltage LOW
}

void fail_info() {
  for (int i = 0; i <= 2; i++) {
    digitalWrite(7, HIGH);
    delay(100);
    digitalWrite(7, LOW);
    delay(100);
  };
}
byte joystick()
{

  int switchVoltage ;   // From Analog read of the button resistors
  int switchDelay  = 100;  // Switch scanning delay
  byte joystick_value = 0;


  switchVoltage = analogRead(0);
  //Serial.println(switchVoltage);

  if (switchVoltage == 0)
  {
    //lcd.LCD_write_string(2, 2, "LEFT ", MENU_NORMAL);
    delay(switchDelay);
    joystick_value = 5;
  }
  else if (switchVoltage > 0 && switchVoltage < 180)
  {
    //lcd.LCD_write_string(2, 2, "PUSH IN", MENU_NORMAL);
    delay(switchDelay);
    joystick_value = 1;
  }
  else if (switchVoltage > 180 && switchVoltage < 400)
  {
    //lcd.LCD_write_string(2, 2, "DOWN ", MENU_NORMAL);
    delay(switchDelay);
    joystick_value = 4;
  }
  else if (switchVoltage > 400 && switchVoltage < 600)
  {
    //lcd.LCD_write_string(2, 2, "RIGHT", MENU_NORMAL);
    delay(switchDelay);
    joystick_value = 6;
  }
  else if (switchVoltage > 600 && switchVoltage < 800)
  {
    //lcd.LCD_write_string(2, 2, "UP   ", MENU_NORMAL);
    delay(switchDelay);
    joystick_value = 3;
  }
  // else if (switchVoltage > 800)              {
  // lcd.LCD_write_string(2, 2, "NONE    ", MENU_NORMAL);
  //  delay(switchDelay);
  // }

  return joystick_value;
}  //--(end joystick )---
