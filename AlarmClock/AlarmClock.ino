#include <LiquidCrystal.h>
#include <DHT.h>
#include<TimeLib.h> // look at options with RTC
#include <TimeAlarms.h>

#define SETBUTTON 8
#define INCBUTTON 9
#define OKBUTTON 10
#define DHTPIN 7
#define DHTTYPE DHT22
#define BUZZPIN 13
#define LCDCONTRAST 155
#include "notes.h"
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//DISABLING ALARM, LOOK AT DELETING ALARM OPTION, AND CHANGING VALUE AND DONE!!
uint8_t setButton = 0;//state of three buttons
uint8_t incButton = 0;
uint8_t okButton = 0;

int setHH, setMM, setSS = 0;//values used to set alarms and timers
int settingAlarmClock = 0;////setting alarm time: 1 hours, 2 minutes
int settingAlarmType = 0;// 1 once, 2 daily, 3 weekly //setting type of alarm

bool inMenu = false;
int selMenu = 0; //NOt in menu, 1 SET ALARM, 2 SHOW ALARMS, 3 CHOOSE A SONG

AlarmID_t alarmId[10]; //maximum 10 alarms
int alarms = 0;//current number of alarms
int showingAlarm = 0;

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
  setTime(1584404453);
  //  Alarm.timerRepeat(500, DHTMeasure);//idealno povekje sekundi

  alarmId[alarms++] = Alarm.alarmOnce(14, 58, 0, playAlarm); //DAILY dtDailyAlarm = 3
  alarmId[alarms++] = Alarm.alarmRepeat(23, 58, 0, playAlarm); //DAILY dtDailyAlarm = 3

}

void chooseMenu(int inc) {
  selMenu += inc;
  if (selMenu == 4) {
    selMenu = 1;// show time if we want nothing, or set to 1 if we want to go from first
  }
  Serial.println("selMenu is");
  Serial.println(selMenu);

  lcd.clear();
  lcd.setCursor(0, 0);
  switch (selMenu) {
    case 1:
      lcd.print("SET ALARM");
      break;
    case 2:
      lcd.print("SHOW ALARMS");
      break;
    case 3:
      lcd.print("Change melody");
      break;
    default:
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
  } else if (selMenu == 0 && !inMenu) {
    digitalClockDisplay();
  }
  Alarm.delay(600);
}
void displayDateTime(int h, int m, int s = -1, int d = -1, int mon = -1, int y = -1) {
  lcd.setCursor(0, 0);
  if (h < 10) {
    lcd.print("0");
    lcd.print(h);
  } else {
    lcd.print(h);
  }
  lcd.print(":");
  if (m < 10) {
    lcd.print("0");
    lcd.print(m);
  } else {
    lcd.print(m);
  }
  if (s == -1) {
    return;
  }
  lcd.print(":");
  if (s < 10) {
    lcd.print("0");
    lcd.print(s);
  } else {
    lcd.print(s);
  }


  lcd.setCursor(0, 1);
  if (d < 10) {
    lcd.print("0");
    lcd.print(d);
  } else {
    lcd.print(d);
  }
  lcd.print(".");
  if (mon < 10) {
    lcd.print("0");
    lcd.print(mon);
  } else {
    lcd.print(mon);
  }
  lcd.print(".");
  lcd.print(y % 100);
}

void digitalClockDisplay() {
  displayDateTime(hour(), minute(), second(), day(), month(), year());
}
void DHTMeasure() {
  char temp[6];
  char hum[6];
  float t = dht.readTemperature();
  float h = dht.readHumidity();// TODO test negative temps, and invail values
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
        Serial.println("SET ALARM MENU: SET");// TODO CHECK IF SET ALARM TYPE bla bla
        Serial.println(settingAlarmType);
        if (settingAlarmClock == 0) {
          settingAlarmClock = 1;
          showSetAlarmMenu(0);
        } else if (settingAlarmType == 0) {
          settingAlarmType = 1;
          showSetAlarmMenu(0);
        } else {
          setAlarm(false);
        }
      }
      break;
    case 2: // SHOW ALARMS//TODO Shit
      static time_t alarmVal = Alarm.read(alarmId[showingAlarm]);
      if (button == 1) {// OK
        if (settingAlarmClock == 0) {
          showingAlarm++;
          if (showingAlarm == alarms) {
            showingAlarm = 0;
          }
          alarmVal = Alarm.read(alarmId[showingAlarm]);
          lcd.clear();
          showAlarm(alarmId[showingAlarm]);
        } else if (settingAlarmType == 0) {
          settingAlarmClock++;
          if (settingAlarmClock == 3) {
            settingAlarmClock = 1;
          }
          showSetAlarmMenu(0);
        } else {
          settingAlarmType++;
          if (settingAlarmType == 5) {
            settingAlarmType = 1;
          }
          showSetAlarmMenu(0);
        }

      } else if (button == 2) {//INC
        if (settingAlarmClock != 0) {
          showSetAlarmMenu(1);
        }
      } else if (button == 3) {// SET
        lcd.clear();
        static int first = 0;

        if (!first) {
          first = 1;
          showAlarm(alarmId[showingAlarm]);
        } else if (settingAlarmClock == 0) {
          settingAlarmClock = 1;
          setHH = alarmVal / 3600;
          setMM = alarmVal % 3600 / 60;
          showSetAlarmMenu(0);
        } else if (settingAlarmType == 0) {
          settingAlarmType = 1;
          showSetAlarmMenu(0);
        } else {
          setAlarm(true);
          first = 0;
        }

        //        else if (settingAlarmClock == 1 && settingAlarmType == 0 ) {
        //          setHH = alarmVal / 3600;
        //          setMM = alarmVal % 3600 / 60;
        //          showSetAlarmMenu(0);
        //          settingAlarmType = 1;
        //        }  else if (settingAlarmType == 1) {
        //          settingAlarmType = 1;
        //          showSetAlarmMenu(0);
        //        } else {
        //          setAlarm(true);
        //        }

      }
      break;
    case 3: // CHOOSE SONG
      static int choosingSong = currentMelody;
      lcd.clear();
      if (button == 1) {
        choosingSong++;
        if (choosingSong == MELODIES) {
          choosingSong = 0;
        }
        displaySong(choosingSong);
      } else if (button == 2) {
        //todoPlaySong
        int prevSong = currentMelody;
        currentMelody = choosingSong;
        playAlarm();
        currentMelody = prevSong;
        displaySong(choosingSong);
      } else if (button == 3) {

        if (choosingSong == currentMelody) {
          displaySong(choosingSong);
        } else {
          currentMelody = choosingSong;
          choosingSong = 0;
          inMenu = false;
          selMenu = 0;
          lcd.setCursor(0, 0);
          lcd.print("NEW SONG SET!");
          lcd.clear();
        }
      }
      break;
    default:
      break;
  }
}

void showAlarm(int id) {
  time_t al = Alarm.read(id);
  int hh = hour(al);
  int mm = minute(al);
  int dow = weekday(al); //(1 sunday);
  bool rep = Alarm.isRepeating(id);
  bool en = Alarm.isEnabled(id);
  Serial.println("ENN");
  Serial.println(en);
  dtAlarmPeriod_t alarmType = Alarm.readType(id);
  lcd.setCursor(0, 0);
  displayDateTime(hh, mm);
  printDOW(dow);
  lcd.setCursor(0, 1);
  if (!rep) {
    lcd.print("Once");
  } else if (alarmType == dtDailyAlarm) {
    lcd.print("Every day");
  } else if (alarmType == dtTimer) {
    lcd.print("Timer");
  } else {
    lcd.print("Every week");
  }
  if (!en) {
    lcd.print(" disabled!");
  }

}
void printDOW(int d) {
  switch (d) {
    case 1:
      lcd.print(" SUN");
      break;
    case 2:
      lcd.print(" MON");
      break;
    case 3:
      lcd.print(" TUE");
      break;
    case 4:
      lcd.print(" WED");
      break;
    case 5:
      lcd.print(" THU");
      break;
    case 6:
      lcd.print(" FRI");
      break;
    case 7:
      lcd.print(" SAT");
      break;
    default:
      lcd.print("ERR");
      break;
  }
}

void displaySong(int song) {
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println(melodyNames[song]);
  lcd.print(melodyNames[song]);
  delay(1000);
}

void showSetAlarmMenu(int inc) {
  lcd.clear();
  lcd.setCursor(0, 1);
  if (settingAlarmType == 0) {
    switch (settingAlarmClock) {
      case 1:
        setHH += inc;
        lcd.print("Set hours");
        break;
      case 2:
        setMM += inc;
        lcd.print("Set minutes");
        break;
      default:
        break;
    }
    lcd.setCursor(0, 0);
    displayDateTime(setHH, setMM);
  } else {
    lcd.clear();
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
      case 4:
        if (Alarm.isEnabled(alarmId[showingAlarm])) {
          lcd.print("Disable");
        } else {
          lcd.print("Enable");
        }
        break;
      default: break;
    }
  }
}

void setAlarm(bool changeAlarm) {
  if (alarms == 9) {
    lcd.print("limit reached!");
    return;
  }
  if (changeAlarm && settingAlarmType != 4) {
    Alarm.write(alarmId[showingAlarm], setHH * 3600 + setMM * 60);
    return;
  }
  switch (settingAlarmType) {
    case 1:
      alarmId[alarms++] = Alarm.alarmOnce(setHH, setMM, 0, playAlarm); //once
      break;
    case 2:
      alarmId[alarms++] = Alarm.alarmRepeat(setHH, setMM, 0, playAlarm); //daily
      break;
    case 3:
      alarmId[alarms++] = Alarm.alarmRepeat(day(), setHH, setMM, 0, playAlarm); //weekly
      break;
    case 4://disable
      if (Alarm.isEnabled(alarmId[showingAlarm])) {
        Alarm.disable(alarmId[showingAlarm]);
      } else {
        Alarm.enable(alarmId[showingAlarm]);
      }
      break;
    default:
      alarmId[alarms++] = Alarm.alarmOnce(setHH, setMM, 0, playAlarm); //once
      break;
  }


  lcd.clear();
  lcd.print("Alarm set!");
  Alarm.delay(1500);
  settingAlarmType = 0;
  settingAlarmClock = 0;
  setHH = 0;
  setMM = 0;
  inMenu = false;
  selMenu = 0;
}
