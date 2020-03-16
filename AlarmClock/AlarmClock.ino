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
#include <LiquidCrystal.h>
#include <DHT.h>
#include<TimeLib.h> //TODO set time using maketime or settime or something
#include <TimeAlarms.h>
#include "notes.h"

#define SETBUTTON 8
#define INCBUTTON 9
#define OKBUTTON 10
#define BUZZPIN 13
#define DHTPIN 7
#define DHTTYPE DHT22
#define CONTRAST 155
#define MINUTES 1
#define HOURS 2
int settingAlarmClock = 0;//1 h, 2 min
int settingAlarmType = 0;// 1 once, 2 daily, 3 weekly
int alarmType = 0; //

bool alarmOn = false;


int val = 0;
int setButton = 0;
int incButton = 0;
int okButton = 0;
int lastButtonState = 0; // 1 set, 2 inc, ok 3
int setTimeState = 0; // 0 HH,1 MM, 2 SS
int menu = 0; // 1 setTime, 2 alarm
bool setting = false;
bool isChosen = false;
int hh = 23;
int mm = 59;
int ss = 50;
int dd = 16;
int mo = 3;
int yy = 2020;
float h, t = 0.0;
bool settingTimeAlarm = false;
// TODO, set time with buttons, set alarm with buttons, make alarm ring
//static uint32_t last_time, now = 0; // RTC
int setHH, setMM, setSS = 0;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#include <TimeLib.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only

  analogWrite(6, CONTRAST);
  pinMode(BUZZPIN, OUTPUT);
  pinMode(SETBUTTON, INPUT);
  pinMode(INCBUTTON, INPUT);
  pinMode(OKBUTTON, INPUT);

  lcd.begin(16, 2);
  dht.begin();

  Alarm.timerRepeat(10, DHTMeasure);//idealno povekje sekundi

  //  playAlarm();
  setTime(1584389894);
  //  setSyncProvider( requestSync);  //set function to call when sync required
  //  Serial.println("Waiting for sync message");
  //  while (!Serial.avfailable()) {
  //    processSyncMessage();
  //  }
  delay(500);
}

void loop() {
  setButton = digitalRead(SETBUTTON);
  incButton = digitalRead(INCBUTTON);
  okButton = digitalRead(OKBUTTON);

  if (!setting) {
    digitalClockDisplay();
  }

  if (setButton == HIGH && !setting) {
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
      alarmType=1;
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
  }
}

void setAlarm() {
  Serial.println("type is");
  Serial.println(alarmType);
  switch (alarmType) {
    case 1:
      Alarm.alarmOnce(setHH, setMM, 0, playAlarm);//once
      break;
    case 2:
      Alarm.alarmRepeat(setHH, setMM, 0, playAlarm);//daily
      break;
    case 3:
      Alarm.alarmRepeat(day(), setHH, setMM, 0, playAlarm); //weekly
      break;
    default: break;
  }
  lcd.clear();
  lcd.print("Alarm set!");
  Alarm.delay(1500);
  lcd.clear();
  setHH = 0;
  setMM = 0;

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

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

void playAlarm() {
  Serial.println("PLAYING!");
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
  alarmOn = false;
}

void DHTMeasure() {
  if (setting)
    return;
  char temp[10];
  char hum[10];
  t = dht.readTemperature();
  h = dht.readHumidity();
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
/*
  void setup() {
  Serial.begin(9600);

  now = millis(); // read RTC initial value
  // initialize the pushbutton pin as an input:
  pinMode(SETBUTTON, INPUT);
  pinMode(INCBUTTON, INPUT);
  pinMode(OKBUTTON, INPUT);

  analogWrite(6, CONTRAST);


  //SETTING TIME!
  // HH:MM 1-24:1-60
  //SET HH, on click-> set min -> on click continue
  //  delay(2000);
  //  for (int i = 0; i < 10; i++) {
  //    buttonState = digitalRead(BUTTONPIN);
  //    if (buttonState == HIGH) {
  //      // turn LED on:
  //      Serial.println("pressed");
  //    } else {
  //      Serial.println("not pressed");
  //    }
  //    delay(4500);
  //  }

  lcd.begin(16, 2);
  dht.begin();
  delay(500);

  lcd.setCursor(0, 0);
  lcd.print(hh);
  lcd.print(": ");
  lcd.print(mm);
  lcd.print(": ");
  lcd.print(ss);
  lcd.setCursor(0, 0);
  lcd.blink();
  }



  void loop() {













  //////////////////////////
  if (!isChosen) {

    lcd.setCursor(0, 0);
    if (hh < 10) {
      lcd.print("0");
      lcd.print(hh);
    } else {
      lcd.print(hh);
    }

    lcd.print(": ");
    if (mm < 10) {
      lcd.print("0");
      lcd.print(mm);
    } else {
      lcd.print(mm);
    }

    lcd.print(": ");
    if (ss < 10) {
      lcd.print("0");
      lcd.print(ss);
    } else {
      lcd.print(ss);
    }
  }
  for (int i = 0; i < 5; i++) {
    while ((now - last_time) < 200) { //delay(200ms);
      now = millis();
    }
    last_time = now;

    // Read buttons
    setButtonState = digitalRead(SETBUTTON);
    incButtonState = digitalRead(INCBUTTON);
    okButtonState = digitalRead(OKBUTTON);

    if (setButtonState == HIGH && !setting) {
      lcd.setCursor(0, 0);
      lcd.print("Set time");
      lcd.setCursor(0, 0);
      menu = 1;
      setting = !setting;
      lastButtonState = 1; //set has been pressed
      goto kur;
    }

    //    if (lastButtonState == 1 && okButtonState == HIGH) {
    //      menu++;
    //      if (menu == 3) {
    //        menu = 1;
    //      }
    //      goto kur;
    //    }

    if (lastButtonState == 1 && setButtonState == HIGH && isChosen == false) {
      isChosen = true;
      Serial.println("what1");
      lastButtonState = 1;
    }

    if (isChosen) {
      switch (menu) {
        case 1: //SET TIME
          lcd.clear();
          lcd.setCursor(0, 0);
          if (setHH < 10) {
            lcd.print("0");
            lcd.print(setHH);
          } else {
            lcd.print(setHH);
          }
          if (setMM < 10) {
            lcd.print(": 0");
            lcd.print(setMM);
          } else {
            lcd.print(": ");
            lcd.print(setMM);
          }
          if (setSS < 10) {
            lcd.print(": 0");
            lcd.print(setSS);
          } else {
            lcd.print(": ");
            lcd.print(setSS);
          }
          switch (setTimeState) {
            case 0:
              lcd.setCursor(0, 1);
              lcd.print("Choose hour");
              break;
            case 1:
              lcd.setCursor(0, 1);
              lcd.print("Choose minutes");
              break;
            case 2:
              lcd.setCursor(0, 1);
              lcd.print("Choose seconds");
              break;
            default:
              break;
          }
          if (incButtonState == HIGH) {
            if (setTimeState == 0) {
              setHH++;
              if (setHH == 24) {
                setHH = 0;
              }
            } else if (setTimeState == 1) {

              setMM++;
              if (setMM == 60) {
                setMM = 0;
              }
            } else if (setTimeState == 2) {
              setSS++;
              if (setSS == 60) {
                setSS = 0;
              }
            }

            lastButtonState = 2;
          }
          if (okButtonState == HIGH) {
            Serial.println("OK");
            setTimeState++;
            if (setTimeState == 3) {
              setTimeState = 0;
            }
            lastButtonState = 3;
          }

          if (setButtonState == HIGH && lastButtonState != 1) {
            Serial.println("whaat2");
            isChosen = false;
            lcd.clear();
            hh = setHH;
            mm = setMM;
            ss = setSS;
            lastButtonState = 1;
          }
          break;
        default:
          goto kur;
          break;
      }
    }

    if (menu == 6) { //setting time
      Serial.println("MENU");
      lcd.setCursor(0, 0);
      if (setHH < 10) {
        lcd.print("0");
        lcd.print(setHH);
      } else {
        lcd.print(setHH);
      }
      if (setMM < 10) {
        lcd.print(": 0");
        lcd.print(setMM);
      } else {
        lcd.print(": ");
        lcd.print(setMM);
      }
      if (setSS < 10) {
        lcd.print(": 0");
        lcd.print(setSS);
      } else {
        lcd.print(": 0");
        lcd.print(setSS);
      }
      //lcd.print("00: 00: 00");//only once
      lcd.setCursor(0, 0);


    }
  kur:

    if (ss == 60) {
      ss = 0;
      mm = mm + 1;
    }
    if (mm == 60)
    {
      mm = 0;
      hh = hh + 1;
    }
    if (hh == 24)
    {
      hh = 0;
    }
  }




  ss = ss + 1; //increment sec. counting
  // ---- manage seconds, minutes, hours am/pm overflow ----
  if (ss == 60) {
    ss = 0;
    mm = mm + 1;
  }
  if (mm == 60)
  {
    mm = 0;
    hh = hh + 1;
  }
  if (hh == 24)
  {
    hh = 0;
  }
  alarmOn = true;
  if (alarmOn) {
    playAlarm();
  }
  }

*/
