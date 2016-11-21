#include <stdio.h>
#include <string.h>
#include <DS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


/*
	This is a program attempting to interact with a serial clock module
	and to display the time on a LCD display using an Arduino.
*/

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2); 

uint8_t CE_PIN   = 5;  //RST pin attach to
uint8_t IO_PIN   = 6;  //
uint8_t SCLK_PIN = 7;

char buf[50];
char day[10];
String comdata = "";
int numdata[7] = {0}, j = 0, mark = 0;

/* DS1302 */
DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);


void print_time()
{
  /* DS1302 */
  Time t = rtc.time();
  /* Set memory size for day of the week */
  memset(day, 0, sizeof(day));
  switch (t.day)
  {
    case 1: strcpy(day, "SUN"); break;
    case 2: strcpy(day, "MON"); break;
    case 3: strcpy(day, "TUE"); break;
    case 4: strcpy(day, "WED"); break;
    case 5: strcpy(day, "THU"); break;
    case 6: strcpy(day, "FRI"); break;
    case 7: strcpy(day, "SAT"); break;
  }
  /* Shows format, however just sets size of the buffer */
  snprintf(buf, sizeof(buf), 
  	"%04d-%02d-%02d %s %02d:%02d:%02d", t.yr, 
  	t.mon, t.date, day, t.hr, t.min, t.sec);
  /* Prints out the display */
  Serial.println(buf);

  /* Print out all of display */
  lcd.setCursor(1, 0);

  lcd.print(t.date / 10);
  lcd.print(t.date % 10);
  lcd.print("-");
  lcd.print(t.mon / 10);
  lcd.print(t.mon % 10);
  lcd.print("-");
  lcd.print(t.yr);

  lcd.print(" ");
  lcd.print(day);

  lcd.setCursor(4, 1);

  lcd.print(t.hr);
  lcd.print(":");
  lcd.print(t.min / 10);
  lcd.print(t.min % 10);
  lcd.print(":");
  lcd.print(t.sec / 10);
  lcd.print(t.sec % 10);
}


void setup()
{
  Serial.begin(9600);
  rtc.write_protect(false);
  rtc.halt(false);

  /*
    // takes 2 seconds to boot and set time
    Time t2(2016, 11, 14, 1, 4, 20, 2);
    rtc.time(t2);
  */

  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
}

void loop()
{

  /* comdata */
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
    mark = 1;
  }
  /* comdata, numdata */
  if (mark == 1)
  {
    Serial.print("You input: ");
    Serial.println(comdata);
    for (int i = 0; i < comdata.length() ; i++)
    {
      if (comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13)
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
      }
    }
    /* numdata - DS1302 */
    Time t(numdata[0], numdata[1], numdata[2], numdata[3], 
    	numdata[4], numdata[5], numdata[6]);
    rtc.time(t);
    mark = 0; j = 0;
    /* comdata */
    comdata = String("");
    /* numdata */
    for (int i = 0; i < 7 ; i++) numdata[i] = 0;
  }

  /* prints time every second */
  print_time();
  delay(1000);
}

