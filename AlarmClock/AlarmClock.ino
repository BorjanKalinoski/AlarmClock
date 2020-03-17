/*
  The  LCD circuit:
     LCD RS pin to digital pin 12
     LCD Enable pin to digital pin 11
     LCD D4 pin to digital pin 5
     LCD D5 pin to digital pin 4
     LCD D6 pin to digital pin 3
     LCD D7 pin to digital pin 2
     LCD R/W pin to ground
     LCD VSS pin to ground
     LCD VCC pin to 5V
     220 [Ohm] resistor from A to 5V,

  The DHT22 Circuit:
     5v, D7, pull-up 5-10 [KOhm], NC, GND
*/
/*RTC
  rtc.getDOWStr(); Wednesday
  rtc.getDateStr(); //17.08.2020
  rtc.getTimeStr(); -> 12:12:12
*/

//TODO CONFIRM MENU

#include <LiquidCrystal.h>
#include <DHT.h>
#include<TimeLib.h> // look at options with RTC
#include <TimeAlarms.h>
#include "notes.h"

#define SETBUTTON 8
#define INCBUTTON 9
#define OKBUTTON 10
#define DHTPIN 7
#define DHTTYPE DHT22
#define BUZZPIN 13
#define LCDCONTRAST 155

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int setButton = 0;//state of three buttons
int incButton = 0;
int okButton = 0;

int hh = 23;//values used to print date
int mm = 59;
int ss = 50;
int dd = 17;
int mo = 3;
int yy = 2020;
int setHH, setMM, setSS = 0;//values used to set alarms and timers

float h, t = 0.0; //values for temperature and humidity

int settingAlarmClock = 0;////setting alarm time: 1 hours, 2 minutes
int settingAlarmType = 0;// 1 once, 2 daily, 3 weekly //setting type of alarm
int alarmType = 0; //same^

int setting = 0; // 0 - set alarm, 1 see alarms, 2 set alarm song
bool inMenu = false;
int selMenu = 0; //NOt in menu, 1 SET ALARM, 2 SHOW ALARMS, 3 CHOOSE A SONG
int action = 0;
AlarmId alarmId[10]; //maximum 10 alarms
int alarms = 0;//current number of alarms

void setAlarm() {
  Serial.println("Alarm type is");
  Serial.println(alarmType);
  switch (settingAlarmType) {
    case 1:
      //alarmId[alarms++] = Alarm.alarmOnce(setHH, setMM, 0, playAlarm); //once
      break;
    case 2:
      // alarmId[alarms++] = Alarm.alarmRepeat(setHH, setMM, 0, playAlarm); //daily
      break;
    case 3:
      // alarmId[alarms++] = Alarm.alarmRepeat(day(), setHH, setMM, 0, playAlarm); //weekly
      break;
    default: break;
  }
  lcd.clear();
  lcd.print("Alarm set!");
  Alarm.delay(1500);
  lcd.clear();
  settingAlarmType = 0;
  settingAlarmClock = 0;
  setHH = 0;
  setMM = 0;
  inMenu = false;
  selMenu = 0;

}
void digitalClockDisplay() {
  hh = hour();
  mm = minute();
  ss = second();
  dd = day();
  mo = month();
  yy = year();

  if (hh < 10) {
    lcd.print("0");
    lcd.print(hh);
  } else {
    lcd.print(hh);
  }
  lcd.print(":");
  if (mm < 10) {
    lcd.print("0");
    lcd.print(mm);
  } else {
    lcd.print(mm);
  }
  lcd.print(":");
  if (ss < 10) {
    lcd.print("0");
    lcd.print(ss);
  } else {
    lcd.print(ss);
  }
  lcd.setCursor(0, 0);

  lcd.setCursor(0, 1);
  if (dd < 10) {
    lcd.print("0");
    lcd.print(dd);
  } else {
    lcd.print(dd);
  }
  lcd.print(".");
  if (mo < 10) {
    lcd.print("0");
    lcd.print(mo);
  } else {
    lcd.print(mo);
  }
  lcd.print(".");
  lcd.print(yy % 100);
  lcd.setCursor(0, 0);
}


void playAlarm() {
  for (int i = 0; i < 3; i++) {
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
      //    Alarm.delay(0);
      divider = melody[thisNote + 1];
      if (divider > 0) {
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }
      tone(BUZZPIN, melody[thisNote], noteDuration * 0.9);
      Alarm.delay(noteDuration);
      noTone(BUZZPIN);
    }
  }
}

void DHTMeasure() {

  if (setting)
    return;

  char temp[10];
  char hum[10];
  t = dht.readTemperature();
  h = dht.readHumidity();// TODO test negative temps, and invail values
  dtostrf(t, 4, 1, temp);
  dtostrf(h, 4, 1, hum);
  lcd.setCursor(9, 0);
  lcd.print("T");

  lcd.print(temp);
  lcd.print("*C");
  lcd.setCursor(9, 1);
  lcd.print("H");
  lcd.print(hum);
  lcd.print("%");
  lcd.setCursor(0, 0);
}
void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only

  analogWrite(6, LCDCONTRAST);
  pinMode(BUZZPIN, OUTPUT);
  pinMode(SETBUTTON, INPUT);
  pinMode(INCBUTTON, INPUT);
  pinMode(OKBUTTON, INPUT);

  lcd.begin(16, 2);
  dht.begin();

  setTime(1584362545);

  //  Alarm.timerRepeat(500, DHTMeasure);//idealno povekje sekundi

  //  playAlarm();

  //  setSyncProvider( requestSync);  //set function to call when sync required
  //  Serial.println("Waiting for sync message");
  //  while (!Serial.avfailable()) {
  //    processSyncMessage();
  //  }
  delay(500);
}

void chooseMenu(int inc) {
  selMenu += inc;
  if (selMenu == 4) {
    selMenu = 1;// show time if we want nothing, or set to 1 if we want to go from first kur
  }
  Serial.println("selMenu is");
  Serial.println(selMenu);
  switch (selMenu) {
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET ALARM");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SHOW ALARMS");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Change melody");
      break;
    default:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET ALARM");
      break;
  }
}
void loop() {
  setButton = digitalRead(SETBUTTON);
  incButton = digitalRead(INCBUTTON);
  okButton = digitalRead(OKBUTTON);

  if (setButton == HIGH && selMenu == 0) {
    Serial.println("GO TO MENU");
    chooseMenu(1);
  } else if (okButton == HIGH && selMenu != 0 && !inMenu) { //change menu
    Serial.println("CHANGE MENU");
    chooseMenu(1);//inMenu?
  } else if (setButton == HIGH && selMenu != 0 && !inMenu) { //select menu
    Serial.println("SELCT MENU");
    selectMenu(selMenu, 3);
    inMenu = true;
    //go inside menu
  } else if (okButton == HIGH && inMenu) {
    Serial.println("IN MENU OK");
    selectMenu(selMenu, 1);
  } else if (incButton == HIGH && inMenu) {
    Serial.println("IN MENU inc");
    selectMenu(selMenu, 2);
  } else if (setButton == HIGH && inMenu) {
    Serial.println("IN MENU SET");
    selectMenu(selMenu, 3);
  }
  Alarm.delay(600);
}

void selectMenu(int menu, int button) {
  switch (menu) {
    case 1: //SET ALARM
      if (button == 1) { //OKBUTTON
        Serial.println("SET ALARM MENU: OK");
        if (settingAlarmType == 0) {
          settingAlarmClock++;
          if (settingAlarmClock == 3) {
            settingAlarmClock = 1;
          }
        } else {
          settingAlarmType++;
          if (settingAlarmType == 4) {
            settingAlarmType = 1;
          }
        }
        showSetAlarmMenu(0);
      } else if (button == 2) { //INCBUTTON
        Serial.println("SET ALARM MENU: INC");
        showSetAlarmMenu(1);
      } else if (button == 3) {//SETBUTTOn
        Serial.println("SET ALARM MENU: SET");
        if (settingAlarmClock == 0) {
          settingAlarmClock = 1;
          showSetAlarmMenu(0);
        } else {
          setAlarm();
        }
      }
      break;
    default:
      showSetAlarmMenu(0);
      break;
  }
}
void showSetAlarmMenu(int inc) {
  lcd.clear();
  if (settingAlarmType == 0) {
    switch (settingAlarmClock) {
      case 1:
        setHH += inc;
        lcd.setCursor(0, 0);
        lcd.print("Set hours");
        break;
      case 2:
        setMM += inc;
        lcd.setCursor(0, 0);
        lcd.print("Set minutes");
        break;
      default:
        break;
    }
    lcd.setCursor(0, 1);
    if (setHH < 10) {
      lcd.print("0");
      lcd.print(setHH);
    } else {
      lcd.print(setHH);
    }
    lcd.print(":");
    if (setMM < 10) {
      lcd.print("0");
      lcd.print(setMM);
    } else {
      lcd.print(setMM);
    }
  } else {
    Serial.println("tuk!!!!a");
    Serial.println(settingAlarmType);
    lcd.clear();
    alarmType = settingAlarmType;
    Serial.println("alarm type ");
    Serial.println(alarmType);
    Serial.println(settingAlarmType);
    switch (settingAlarmType) {
      case 1:
        lcd.print("Set once");
        break;
      case 2:
        lcd.print("Set daily");
        break;
      case 3:
        lcd.print("Set weekly");
        break;
      default: break;

    }
  }
}


/*
  void loop() {
  setButton = digitalRead(SETBUTTON);
  incButton = digitalRead(INCBUTTON);
  okButton = digitalRead(OKBUTTON);

  if (setting == 0) {
    Serial.println("should not be here");
    // digitalClockDisplay();
  }
  if (setting == 0 && setButton == HIGH) { //SHOW MENU
    showMenu(1);
    inMenu = true;
  }

  else if (setting == 1 && !returnToMenu) {
    if (setButton == LOW && incButton == LOW && okButton == LOW) {
      showSetAlarmMenu(0);
    } else if (incButton == HIGH) {
      showSetAlarmMenu(1);
    } else if (okButton == HIGH) {
      settingAlarmClock++;
      if (settingAlarmClock == 3) {
        settingAlarmClock = 1;
      }
      lcd.clear();
      showSetAlarmMenu(0);
    }
  }


  //  if ((setButton == HIGH && setting == 0) || (setting != 0 && okButton == HIGH) || returnToMenu) {
  //    setting++;
  //
  //    if (setting == 4) {
  //      setting = 1;// show time if we want nothing, or set to 1 if we want to go from first kur
  //    }
  //    Serial.println("Setting is");
  //    Serial.println(setting);
  //    switch (setting) {
  //      case 1:
  //        lcd.clear();
  //        lcd.setCursor(0, 0);
  //        lcd.print("SET ALARM");
  //        break;
  //      case 2:
  //        lcd.clear();
  //        lcd.setCursor(0, 0);
  //        lcd.print("SHOW ALARMS");
  //        break;
  //      case 3:
  //        lcd.clear();
  //        lcd.setCursor(0, 0);
  //        lcd.print("Change melody");
  //        break;
  //      default:
  //        //lcdprint default menu
  //        break;
  //    }
  //
  //  } else if (setting == 1 && !returnToMenu) { //SETTING ALARM
  //    //    if (setButton == HIGH) {
  //    //      setAlarm();
  //    //   //   returnToMenu = true;
  //    //      settingAlarmType = 0;
  //    //      alarmType = 0;
  //    //    } else
  //    if (incButton == HIGH) {
  //      showSetAlarmMenu(1);
  //    } else if (okButton == HIGH) {
  //      settingAlarmClock++;
  //      if (settingAlarmClock == 3) {
  //        settingAlarmClock = 1;
  //      }
  //      lcd.clear();
  //      showSetAlarmMenu(0);
  //    }

  //    if (setButton == HIGH) {
  //      settingAlarmClock = 0;
  //      settingAlarmType = 1;
  //      alarmType = 1;
  //      lcd.clear();
  //      lcd.print("Set daily");
  //
  //    }
  //    Serial.println("Setting one");
  //    if (setButton == HIGH) {
  //      lcd.clear();
  //      settingAlarmClock = 1;
  //      lcd.setCursor(0, 0);
  //      lcd.print("Set hour");
  //      lcd.setCursor(0, 1);
  //      lcd.print("00:00");
  //      lcd.blink();
  //    } else if (incButton == HIGH) {
  //      showSetAlarmMenu();
  //    }
  //    else if (okButton == HIGH) {
  //      settingAlarmClock++;
  //      if (settingAlarmClock == 3) {
  //        settingAlarmClock = 1;
  //      }
  //      lcd.clear();
  //      showSetAlarmMenu();
  //
  //    } else if (setButton == HIGH) {
  //      settingAlarmClock = 0;
  //      settingAlarmType = 1;
  //      alarmType = 1;
  //      lcd.clear();
  //      lcd.print("Set daily");
  //    }


  Alarm.delay(500);

  /*
    if (setButton == HIGH && !setting) { //SHOW MENU (first menu is
      setting = true;
      settingAlarmClock = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set hour");
      lcd.setCursor(0, 1);
      lcd.print("00:00");

      lcd.blink();
    } else if (setting && settingAlarmClock != 0) {
      if (incButton == HIGH) {

        switch (settingAlarmClock) {
          case 1:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Set hours");
            setHH++;
            if (setHH == 24) {
              setHH = 0;
            }
            break;
          case 2:
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Set minutes");
            setMM++;
            if (setMM == 60) {
              setMM = 0;
            }
            break;
          default:
            break;
        }

        lcd.setCursor(0, 1);
        if (setHH < 10) {
          lcd.print("0");
          lcd.print(setHH);
        } else {
          lcd.print(setHH);
        }
        lcd.print(":");
        if (setMM < 10) {
          lcd.print("0");
          lcd.print(setMM);
        } else {
          lcd.print(setMM);
        }

      } else if (okButton == HIGH) {
        settingAlarmClock++;
        if (settingAlarmClock == 3) {
          settingAlarmClock = 1;
        }
        lcd.clear();
        switch (settingAlarmClock) {
          case 1:
            lcd.setCursor(0, 0);
            lcd.print("Set hours");

            break;
          case 2:
            lcd.setCursor(0, 0);
            lcd.print("Set minutes");
            break;
          default:
            break;
        }
        lcd.setCursor(0, 1);
        if (setHH < 10) {
          lcd.print("0");
          lcd.print(setHH);
        } else {
          lcd.print(setHH);
        }
        lcd.print(":");
        if (setMM < 10) {
          lcd.print("0");
          lcd.print(setMM);
        } else {
          lcd.print(setMM);
        }
      } else if (setButton == HIGH) {
        settingAlarmClock = 0;
        settingAlarmType = 1;
        alarmType = 1;
        lcd.clear();
        lcd.print("Set daily");
      }
    } else if (setting && settingAlarmType != 0) {
      if (okButton == HIGH) {
        settingAlarmType++;
        if (settingAlarmType == 4) {
          settingAlarmType = 1;
        }
        lcd.clear();
        alarmType = settingAlarmType;
        Serial.println("alarm type ");
        Serial.println(alarmType);
        Serial.println(settingAlarmType);
        switch (settingAlarmType) {

          case 1:
            lcd.print("Set once");

            break;
          case 2:
            lcd.print("Set daily");
            break;
          case 3:
            lcd.print("Set weekly");
            break;
          default: break;
        }
      } else if (setButton == HIGH) {
        setAlarm();
        setting = false;
        settingAlarmType = 0;
        alarmType = 0;
      }
    }
    if (setting) {
      Alarm.delay(200);
    } else {
      Alarm.delay(1000);
    }*/
//}
