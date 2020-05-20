//---------------------------------------------------------
/*
  NHD_1_8_128160EF_CTXI_mega.ino
  Program for writing to Newhaven Display 1.8" TFT with ILI9163 controller

  (c)2013 Mike LaVine - Newhaven Display International, LLC.

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.
*/
//---------------------------------------------------------

// The 8 bit data bus is connected to PORTA of the Arduino Mega2560
// 5V voltage regulator on Arduino Mega has been replaced with a 3.3V regulator to provide 3.3V logic

int DC = LCD_DC;      // D/C signal connected to Arduino digital pin 30
int WR = LCD_WR;      // /WR signal connected to Arduino digital pin 31
int RD = LCD_RD;      // /RD signal connected to Arduino digital pin 32
int RST = LCD_RST;     // /RST signal connected to Arduino digital pin 33

// /CS signal tied to ground

void writeDataBus(unsigned char c) {
  digitalWrite(LCD_DB0, bitRead(c, 0));
  digitalWrite(LCD_DB1, bitRead(c, 1));
  digitalWrite(LCD_DB2, bitRead(c, 2));
  digitalWrite(LCD_DB3, bitRead(c, 3));
  digitalWrite(LCD_DB4, bitRead(c, 4));
  digitalWrite(LCD_DB5, bitRead(c, 5));
  digitalWrite(LCD_DB6, bitRead(c, 6));
  digitalWrite(LCD_DB7, bitRead(c, 7));
}

void comm_out(unsigned char c)
{
  digitalWrite(DC, LOW);
  //  PORTA = c;
  writeDataBus(c);
  digitalWrite(WR, LOW);
  delayMicroseconds(5);
  digitalWrite(WR, HIGH);
}

void data_out(unsigned char d)
{
  digitalWrite(DC, HIGH);
  writeDataBus(d);
  digitalWrite(WR, LOW);
  delayMicroseconds(5);
  digitalWrite(WR, HIGH);
}
void disp()
{
  unsigned int i;
  comm_out(0x2C);              //command to begin writing to frame memory
  for (i = 0; i < 20480; i++)  //fill screen with red pixels
  {
    data_out(0xFF);
    data_out(0x00);
    data_out(0x00);

  }
  Serial.println("Filled RED");
  for (i = 0; i < 20480; i++)  //fill screen with green pixels
  {
    data_out(0x00);
    data_out(0xFF);
    data_out(0x00);
  }
   Serial.println("Filled GREEN");
  for (i = 0; i < 20480; i++)  //fill screen with blue pixels
  {
    data_out(0x00);
    data_out(0x00);
    data_out(0xFF);
  }
   Serial.println("Filled BLUE");
}

void initLCD()
{
  //  DDRC = 0xFF;
  //  PORTC = 0x00;
  //  DDRA = 0xFF;
  //  PORTA = 0x00;
  digitalWrite(RD, HIGH);
  digitalWrite(WR, LOW);
  digitalWrite(RST, LOW);
  delay(150);
  digitalWrite(RST, HIGH);
  delay(150);

  comm_out(0x11);              //exit SLEEP mode

  delay(100);

  comm_out(0x28);              //display off

  comm_out(0x26);              //select gamma curve
  data_out(0x04);

  comm_out(0xB1);              //frame rate control
  data_out(0x0A);
  data_out(0x14);

  comm_out(0xC0);              //power control 1
  data_out(0x0A);
  data_out(0x00);

  comm_out(0xC1);              //power control 2
  data_out(0x02);

  comm_out(0xC5);              //VCOM control 1
  data_out(0x2F);
  data_out(0x3E);

  comm_out(0xC7);              //VCOM control 2
  data_out(0x40);

  comm_out(0x2A);              //column address set
  data_out(0x00);
  data_out(0x00);                  //start 0x0000
  data_out(0x00);
  data_out(0x7F);                  //end 0x007F

  comm_out(0x2B);              //page address set
  data_out(0x00);
  data_out(0x00);                  //start 0x0000
  data_out(0x00);
  data_out(0x9F);                  //end 0x009F

  comm_out(0x36);              //memory access control
  data_out(0xC8);

  comm_out(0x3A);              //pixel format = 18 bit per pixel
  data_out(0x06);

  comm_out(0x29);              //display ON

  delay(10);
}