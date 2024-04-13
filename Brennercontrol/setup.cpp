#include "Brennercontrol.h"

void setup()
{
  // initialize Serial ***********************************
  Serial.begin(9600);
  delay(300);
  Serial.println(SKETCH_NAME);
  Serial.print("Compiled:  "); Serial.print(__DATE__);  Serial.print(" ");  Serial.println( __TIME__);

  //pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt (digitalPinToInterrupt(interruptPin), check_burner_isr_int0, RISING);

  db_pln("startup rs485 slave");
  softserial.begin(BAUD_RATE);  //  BAUD_RATE 28800
  myChannel.begin();

  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
   digitalWrite (ENABLE_PIN, LOW);  // disable sending

  db_pln("*** Debug enabled");
#ifndef DEBUG    //
  Serial.println(F("*** Debug disabled"));
#endif

  // initialize Radio
#ifdef RADIO    //#endif
  init_radio(); Serial.println(F("*** Radio enabled"));
#else 
Serial.println(F("*** Radio disabled"));
#endif

  // initialize LCD ***********************************
#ifdef LCD_DISPLAY
  lcd.LCD_init();
  lcd.LCD_clear();
  lcd.backlight(OFF);
  lcd.LCD_write_string(0, 0, SKETCH_NAME, MENU_NORMAL );
  lcd.LCD_write_string(0, 1, "xx ", MENU_NORMAL );
  lcd.LCD_write_string(0, 2, "Brenner: A3 ", MENU_NORMAL );
  lcd.LCD_write_string(0, 3, "18B20P: D8", MENU_NORMAL );
  delay(1000);
  lcd.LCD_clear();
#endif

  // initialize temperatur sensors ***********************************
  //  sensors.begin(); // Dallas Senssor,IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  //  // ...and get first reading
  //
  //  sensors.setWaitForConversion(false);
  //  sensors.requestTemperatures();
  //  pinMode(ONE_WIRE_BUS, OUTPUT);
  //  digitalWrite(ONE_WIRE_BUS, HIGH);
  //  delay(750);
  //  Temperature10 = round(sensors.getTempCByIndex(0) * 10);
  //  sensors.setWaitForConversion(true);


  //  initiaize ouput LED
  pinMode(BurnIndicatorLEDPin, OUTPUT);


  //  initialize SD Card  ************************************
#ifdef SDCard   // #endif
  const int Pin_chipSelect = 10;      // SPI Bus, SD Card Memoire Board
  pinMode(Pin_chipSelect, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(Pin_chipSelect)) {
    Serial.println("init SD Card fail");
    //lcd.LCD_write_string(5, 0, "SD FAILED", MENU_NORMAL );
    lcd.LCD_write_string(65, 0, "W?", MENU_NORMAL );
  }
  else {
    Serial.println("init SD card OK.");
    //lcd.LCD_write_string(5, 0, "SD OK", MENU_NORMAL );
    lcd.LCD_write_string(65, 0, "WOK", MENU_NORMAL );
  }
#endif


// reset EEPROM
// 1024 bytes for Uno and Mega

// const byte eeAddress = 0; 
// gesamt_brenndauer = 111; EEPROM.put(eeAddress, gesamt_brenndauer);   // reset EEPROM
//  EEPROM.get( eeAddress, gesamt_brenndauer );
//  db_px("gesamt_brenndauer eeprom ", gesamt_brenndauer);
//
// 
//
// gesamt_brenndauer = 0;  // Zähler ab letztem tanken        
// EEPROM.put( addr_gesamt_brenndauer,  gesamt_brenndauer );
//
//
//  gesamt_brenndauer_last_day = gesamt_brenndauer ;   
//EEPROM.put( addr_gesamt_brenndauer_last_day , gesamt_brenndauer_last_day );


//  temp_meas_count = 0 ;              EEPROM.put( addr_temp_meas_count , temp_meas_count );
//  average_temp = 0 ;                 EEPROM.put( addr_average_temp , average_temp );

   
  EEPROM.get( addr_gesamt_brenndauer,  gesamt_brenndauer );
  EEPROM.get( addr_gesamt_brenndauer_last_day , gesamt_brenndauer_last_day );
  EEPROM.get( addr_temp_meas_count , temp_meas_count );
  EEPROM.get( addr_average_temp , average_temp );
  EEPROM.get( addr_min_T10 ,  min_T10 );
  EEPROM.get( addr_max_T10 ,  max_T10 );

  db_px("gesamt_brenndauer  ", gesamt_brenndauer);
  db_px("addr_gesamt_brenndauer_last_day  ", addr_gesamt_brenndauer_last_day);
  
  db_px("gesamt_brenndauer last daay  ", gesamt_brenndauer_last_day);
  db_pln(temp_meas_count);
  db_pln(average_temp);

  // initialize Brenner Sensor
  pinMode(BrennerSensorPin, INPUT_PULLUP);
  // falls Brenner läuft auf Brenner Ende warten
  digitalWrite(7, HIGH);



#ifdef LCD_DISPLAY
  lcd.LCD_write_string(0, 0, "wait for burner idle", MENU_NORMAL );
#endif

  while (digitalRead(BrennerSensorPin) == LOW) {
    Serial.println ("wait for burn stop");
  }
  digitalWrite(7, LOW);
  Serial.println ("burn stopped");

  // Read current time for average temperature
  RTC.read(tm);



  // initialize for day average
  lastday = tm.Day;
  // gesamt_brenndauer_last_day = gesamt_brenndauer;

  // initialize timer
  timenow = millis();
  last_time_Burn_On = timenow; // Burntime counter
  last_time_display = timenow; //
  last_time_temp_meas = timenow; //
  last_time_transmit_T = timenow; // temperatrue transmit
  last_time_average_Temp = timenow;




}  // end of setup