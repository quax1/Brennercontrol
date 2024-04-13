/*
 * 
 * 
 * burnlog.command = 0  test
 * 1 tägliche Meldung
 * 2  Temperaturmeldung
 * 3  Brenndauer
 * 
 * 
 * 3.0  neue Struktur
Field 1 Watt
Field 2 kWh/Tag
Field 3 Impulse gesamt
Field 4 Brennerdauer
Field 5   T Tagesdurchschnitt
Field 6   T Tag
Field 7  Brenner_Failed_Transmits
Field 8  PowerLogg_Failed_Transmits
 * 
 * 3.1 geänderte Struct für Transmit, RF low power mode
 * to do:  Temperatur -126.9   ausfiltern
 * 
 * 
 * 4.0 7.5.2022 
 *     programmierung ohne delay
 *     brennzeitmessung exakt mit micros funktion
 *     current burntime als variable, wird auch gesendet
 *     
 * 4.1  
 * to do:   pullup Widerstand ändern oder Code ändern wg. Conversion Fehler
 * Zwischen die 5-V-Leitung und die  Datenleitung wird ein 4,7-Kiloohm-Widerstand geschaltet. 
 * Bei sehr langen Verbindungsleitungen kann man auch bis auf 1,8 Kiloohm heruntergehen.
 * 
 * 4.4 fernkonfiguration eingeführt
 * Temperature10 = - 10000  invalid resultr
 * average temp alle 30 min, und wird nicht mehr doppelt gemessen
 * 
 * 4.5 Temperaturmessung jetzt sicherer, niedrigerer  pullup und test auf plausible werte
 * 4.6 
 * gesamt_brenndauer_last_day   unsigned int nach unsigned long geändert und im eeprom gesichert 
 * SD Zugriff macht Fehler
 * 4.7 SD deaktiviert, average temperatur wird nur ausgegebne wenn genügend messungen vorhanden sonst -90°C
 * Durchnittstemp. wird nur ermittelt wenn genügend valide Werte vorhanden
 * 5.0 Min und Max Temp.  geändertes Datenstruct !
 * neue eeprom adresse
 * 5.1 T werte werden jetzt auch ins eeprom geschrieben - war bis dahin ein feler
 * 
 * 
 * 6.0 dynanic paylad  2512.22 installier
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * #fehler
 * 5.2 installiert 
 * 5.3 
  * 
 * 6.0   Brenner zähler auf 0 gesetzt, 25.12.22, zähler = 11976226,  enable dynamic payloads, muss bei allen nodes sein
 */

 
