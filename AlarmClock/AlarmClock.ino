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
#include <LiquidCrystal.h>
#include <DHT.h>

#define BUTTONONE 8
#define BUTTONTWO 9
#define CONTRAST 155
#define DHTPIN 7
#define DHTTYPE DHT22

int val = 0;
int buttonOne = 0;
int buttonTwo = 0;

int hh = 23;
int mm = 59;
int ss = 30;
int dd = 16;
int mm = 3;
int yy = 2020;


static uint32_t last_time, now = 0; // RTC

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);



  now = millis(); // read RTC initial value
  // initialize the pushbutton pin as an input:
  pinMode(BUTTONONE, INPUT);
  pinMode(BUTTONTWO, INPUT);

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
  lcd.print(":");
  lcd.print(mm);
  lcd.print(":");
  lcd.print(ss);
  lcd.setCursor(0, 0);
  lcd.blink();
  //lcd.print(" T:");...

}

void loop() {
  //  delay(2000);
  //
  //  float t = dht.readTemperature();// Gets the values of the temperature
  //  float h = dht.readHumidity(); // Gets the values of the humidity
  //
  //  if (isnan(h) || isnan(t)) { // TODO print wrong values for T || H
  //    Serial.println(F("Failed to read from DHT sensor!"));
  //    return;
  //  }
  //
  //  float hic = dht.computeHeatIndex(t, h, false);
  //  lcd.setCursor(0, 0);
  //  lcd.print("T: ");
  //  lcd.print(t);
  //  lcd.print("*C");
  //  lcd.setCursor(0, 1);
  //  lcd.print("H: ");
  //  lcd.print(h);
  //  lcd.print(" %");
  //  delay(2000);




  for (int i = 0; i < 5; i++) {
    while ((now - last_time) < 200) { //delay(200ms);
      now = millis();
    }
    last_time = now;
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

    lcd.setCursor(0, 0);

    if (hh < 10)lcd.print("0"); // always 2 digits
    lcd.print(hh);
    lcd.print(":");
    if (mm < 10)lcd.print("0");
    lcd.print(mm);
    lcd.print(":");
    if (ss < 10)lcd.print("0");
    lcd.print(ss);
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
}
