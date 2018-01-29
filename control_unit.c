

#include <Arduino.h>
#include <U8g2lib.h>
#include <TimerOne.h>
#include <Encod_er.h>
#include <avr\pgmspace.h>
#include "main.h"
#include "LangPack.h"



U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

Encod_er encoder( 2, 7, 4);

void setup(void) {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();    // enable UTF8 support for the Arduino print() function
  Timer1.initialize(250); // инициализация таймера 1, период 250 мкс
  Timer1.attachInterrupt(timerInterrupt, 250); // задаем обработчик прерываний
  pinMode(5, INPUT_PULLUP);
  balanceActivated = 1;//check if installed
  selectorActivated = 1;//check if installed
  tembroActivated = 1;//check if installed
  Serial.println("Start");
}

void loop() {

  volume += getEncoder();

  if (counter > PRESS_EVENT)menu_event = 1;

  if (!menu_event)main_screen(VOLUME);
  else menu_screen();
}

void timerInterrupt() {
  encoder.scanState();
  if (!(PIND & (1 << 5)))counter++;
  else counter = 0;

}

void main_screen(bool mode) {

  volumeL = volume + balance;
  volumeR = volume - balance;
  if (volumeL > 128)volumeL = 128;
  if (volumeL < 0)volumeL = 0;
  if (volumeR > 128)volumeR = 128;
  if (volumeR < 0)volumeR = 0;

  //u8g2.setFont(u8g2_font_cu12_t_cyrillic);
  //u8g2.setFont(u8g2_font_unifont_t_cyrillic);
  //u8g2.setFont(u8g2_font_10x20_t_cyrillic);
  u8g2.setFont(u8g2_font_haxrcorp4089_t_cyrillic);
  //u8g2.setFont(u8g2_font_crox2cb_tr);
  u8g2.firstPage();
  do {

    if (!mode)softPrint(30, 7, 1, 3);
    else softPrint(30, 7, 1, 4);
    u8g2.drawHLine(0, 9, 127);
    u8g2.setCursor(0, 30);
    u8g2.print("L:");
    for (int i = 0; i < volumeL / 3; i++) u8g2.print("|");

    u8g2.setCursor(97, 30);
    u8g2.print(volumeL - 128); u8g2.print("dB");
    u8g2.setCursor(0, 45);
    u8g2.print("R:");
    for (int i = 0; i < volumeR / 3; i++) u8g2.print("|");
    u8g2.setCursor(97, 45);
    u8g2.print(volumeR - 128); u8g2.print("dB");

    if (!mode) {
      softPrint(5, 63, 1, 5);
      softPrint(70, 63, 1, 6);
      if (selector == 4)softPrint(35, 63, 1, 17);
      else {
        u8g2.setCursor(35, 63);
        u8g2.print(selector);
      }
    }
    else softPrint(5, 63, 1, 0);
  } while ( u8g2.nextPage() );

}


void menu_screen() {
  while (!(PIND & (1 << 5))) {
    Serial.println(counter);
    if (counter > 4000) {
      menu_event = 0;
      counter = 0;
      last_encoder = encoder.read();
      switchOff();
      return;
    }
  }
  menu_position = 0;
  int add = 0;
  //char buffer[30];
  u8g2.setFont(u8g2_font_haxrcorp4089_t_cyrillic);
  u8g2.setFontMode(0);

  do {
    u8g2.firstPage();
    do {
      u8g2.setCursor(30, 7);
      u8g2.setDrawColor(1);
      u8g2.print("МЕНЮ");
      u8g2.drawHLine(0, 9, 127);

      for (byte i = 0; i < MENU_LINES; i++) {
        if (i + add == menu_position) {
          u8g2.drawBox(9, 10 + 12 * i, 80, 11);
          softPrint(10, 18 + 12 * i, 0, 7 + i + add);
        }

        else softPrint(10, 18 + 12 * i, 1, 7 + i + add);
      }

    } while ( u8g2.nextPage() );

    menu_position += getEncoder();
    if (menu_position < 0)menu_position = 0;
    if (menu_position > MENU_ITEMS - 1)menu_position = MENU_ITEMS - 1;
    if (menu_position > MENU_LINES - 1 + add)add++;
    if (menu_position < add)add--;

    if (counter > PRESS_EVENT) {
      switch (menu_position) {
        case 0:  menu_event = 0;
          counter = 0; last_encoder = encoder.read(); break;
        case 1: selector_event = 1; counter = 0; selectInput(); break;
        case 2: balance_event = 1; counter = 0; balanceScreen(); break;
        case 3: tembro_event = 1; counter = 0; tembroScreen(); break;
        case 4: relay_delay_event = 1; counter = 0; relayDelay(); break;
        case 5: item1_event = 1; counter = 0; notActivated(); break;
        case 6: item2_event = 1; counter = 0; notActivated(); break;
      }

    }
  } while (menu_event);

}

void notActivated() {
  u8g2.firstPage();
  do {
    u8g2.setCursor(30, 25);
    u8g2.setDrawColor(1);
    u8g2.print("ОПЦИЯ");
    u8g2.setCursor(10, 38);
    u8g2.print("НЕ АКТИВИРОВАНА");

  } while ( u8g2.nextPage() );
  delay(2000);
  item1_event = 0;
  item2_event = 0;
  tembro_event = 0;
  balance_event = 0;
}

void balanceScreen() {
  if (!balanceActivated) {
    notActivated();
    balance_event = 0;
    return;
  }

  do {
    balance += getEncoder();

    if (balance > 128) balance = 128;
    if (balance < -127) balance = -127;
    main_screen(BALANCE);
    while (!(PIND & (1 << 5)));
    if (counter > 4000) {
      counter = 0;
      balance = 0;
    }
    if (counter > PRESS_EVENT) {
      counter = 0;
      balance_event = 0;
    }
    /* if (counter > PRESS_EVENT) {
       balance_event = 0;
       counter = 0;*/

  } while (balance_event);
}

void selectInput() {
  byte cursor = selector;
  if (!selectorActivated) {
    notActivated();
    selector_event = 0;
    return;
  }

  do {

    u8g2.firstPage();
    do {
      softPrint(10, 7, 1, 19);
      u8g2.drawHLine(0, 9, 127);

      for (byte i = 0; i < INPUTS_NUMBER; i++) {
        //u8g2.setCursor(10, 18 + 12 * i);

        if (i == cursor - 1) {
          u8g2.drawBox(9, 10 + 12 * i, 80, 11);
          softPrint(10, 18 + 12 * i, 0, 14 + i);
        }
        else softPrint(10, 18 + 12 * i, 1, 14 + i);
      }

    } while ( u8g2.nextPage() );

    cursor += getEncoder();

    if (cursor < 1)cursor = 1;
    if (cursor > INPUTS_NUMBER) cursor = INPUTS_NUMBER;

    if (counter > PRESS_EVENT) {
      selector_event = 0;
      menu_event = 0;
      counter = 0;
      selector = cursor;
      u8g2.setDrawColor(1);
    }
  } while (selector_event);
}

void relayDelay() {

  int del = relayDelayVal;

  do {

    u8g2.firstPage();
    do {
      softPrint(10, 7, 1, 1);
      u8g2.drawHLine(0, 9, 127);
      u8g2.drawFrame(24, 17, 80, 30);
      u8g2.setCursor(40, 38);
      u8g2.print(del);

      softPrint(52, 38, 1, 18);
      softPrint(5, 63, 1, 0);
    } while ( u8g2.nextPage() );

    del += getEncoder();

    if (del < 0)del = 0;
    if (del > MAX_DELAY_RELAY) del = MAX_DELAY_RELAY;

    if (counter > PRESS_EVENT) {
      relay_delay_event = 0;
      //menu_event = 0;
      counter = 0;
      relayDelayVal = del;
    }
  } while (relay_delay_event);
}

int getEncoder() {
  long current_encoder = encoder.read();
  int res = current_encoder - last_encoder;
  last_encoder = current_encoder;
  return res;
}

void switchOff() {
  int j = 128;
  //u8g2.setFont(u8g2_font_cu12_t_cyrillic);
  //u8g2.setFont(u8g2_font_unifont_t_cyrillic);
  u8g2.setFont(u8g2_font_10x20_t_cyrillic);
  
  do {
    u8g2.firstPage();
    do {
      u8g2.setDrawColor(1);
      softPrint(5, 30, 1, 2);

    } while ( u8g2.nextPage() );
    j -= 2;
  } while (j > -80);
  u8g2.clear();

  while (counter < PRESS_EVENT) {
    Serial.println(counter);
  }
  delay(100);
  counter = 0;
}

void softPrint(byte x, byte y, byte color, const int index) {
  u8g2.setCursor(x, y);
  u8g2.setDrawColor(color);
  char textBuffer[50];
  strcpy_P(textBuffer, pgm_read_word((int)(text + index)));
  u8g2.print(textBuffer);

}

void tembroScreen() {
  while (!(PIND & (1 << 5)));
  counter = 0;
  byte cursor = 1;
  bool temp = tembroOn;
  if (!tembroActivated) {
    notActivated();
    return;
  }

  do {
    u8g2.firstPage();
    do {
      softPrint(7, 7, 1, 20);
      tembroOn ? softPrint(75, 7, 1, 25) : softPrint(75, 7, 1, 26);
      u8g2.drawHLine(0, 9, 127);

      for (byte i = 1; i <= TEMBRO_MENU_LINES; i++) {
        //u8g2.setCursor(10, 18 + 12 * i);

        if (i == cursor) {
          u8g2.drawBox(9, 4 + 12 * i, 60, 11);
          softPrint(10, 12 + 12 * i, 0, 20 + i);
        }
        else softPrint(10, 12 + 12 * i, 1, 20 + i);
      }

    } while ( u8g2.nextPage() );

    cursor += getEncoder();

    if (cursor < 1)cursor = 1;
    if (cursor > TEMBRO_MENU_LINES) cursor = TEMBRO_MENU_LINES;

    if (counter > PRESS_EVENT) {
      switch (cursor) {
        case 1: tembro_event = 0; counter = 0; break;
        case 2: temp = 1; if (temp != tembroOn) {
            tembroOn = temp;
            sendDataTembro();
          } counter = 0; break;
        case 3: counter = 0; tune_tembro_event = 1; tembroTune(); break;
        case 4: temp = 0; if (temp != tembroOn) {
            tembroOn = temp;
            sendDataTembro();
          } counter = 0; break;
      }
    }
  } while (tembro_event);

}

void tembroTune() {
  while (!(PIND & (1 << 5)));
  counter = 0;
  byte cursor = 1;

  bool drawing = 0;
  bool lf = 0;
  bool hf = 0;
  byte value = tembro;
  byte temp_lf = value  << 4; temp_lf = temp_lf>>5;
  byte temp_hf = value >>5;
  byte bbb = 0;
  bool lock_flag = 0;

  do {
    u8g2.firstPage();
    do {
      softPrint(0, 8, 1, 30);
      u8g2.drawHLine(0, 10, 127);

      for (byte i = 1; i <= 3; i++) {
        if ( i == cursor)  u8g2.drawFrame(6, 2 + 16 * i, 54, 14);
        softPrint(10, 12 + 16 * i, 1, 26 + i);
        if (!lf && !hf && i == 2) {
          softPrint(85, 12 + 16 * i, 1, temp_lf+30);
        }
        if (!lf && !hf && i == 3) {
          softPrint(85, 12 + 16 * i, 1, temp_hf+30);
        }
      }
      if (lf || hf) {
        if(!lock_flag&&lf){bbb = temp_lf; lock_flag = 1;}
        else if(!lock_flag&&hf){bbb = temp_hf; lock_flag = 1;}
         bbb += getEncoder();
        if (bbb > 7)bbb = 7;
        if (bbb < 1)bbb = 1;      
      u8g2.drawHLine(94, 38, 30);
if (bbb > 4)  u8g2.drawBox(100, 38 - (bbb-4) * 8, 20, (bbb-4) * 8 );
if (bbb < 4)  u8g2.drawFrame(100, 38, 20, (4-bbb) * 8 );
softPrint(70, 63 - 6 * bbb, 1, bbb+30);
    }
  } while ( u8g2.nextPage() );
  if (lf || hf) {
    lf ? temp_lf = bbb : temp_hf = bbb;
    value = temp_hf << 5 | temp_lf << 1 ;
    if (tembro != value) {tembro = value; sendDataTembro(); }
    if (counter > PRESS_EVENT) {
      lock_flag = hf = lf = 0;
      counter = 0;
    }
  }
  if (!lf && !hf) {
    cursor += getEncoder();

    if (cursor < 1)cursor = 1;
    if (cursor > 3) cursor = 3;

    if (counter > PRESS_EVENT) {
      switch (cursor) {
        case 1: tune_tembro_event = 0; counter = 0; break;
        case 2: lf = !lf; counter = 0; break;
        case 3: hf = !hf; counter = 0;  break;
      }
    }
  }

} while (tune_tembro_event);

}

void sendDataTembro() {

}
