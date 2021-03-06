/*****************************************************************************
The MIT License (MIT)

Copyright (c) 2020 Matthew James Bellafaire

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
//starts battery monitor and configures with parameters stated in Declarations.h
void initBatMonitor()
{
  int StatusPOR = readRegister(BAT_MONITOR_ADDR, 0x00) & 0x0002;
  if (StatusPOR == 0)
  {
  }
  else
  {

    while (readRegister(BAT_MONITOR_ADDR, 0x3D) & 1)
    {
      delay(1);
    }
    int    HibCFG = readRegister(BAT_MONITOR_ADDR, 0xBA); //Store original HibCFG value
    sendWrite(BAT_MONITOR_ADDR, 0x60, 0x90);       // Exit Hibernate Mode step 1
    sendWrite(BAT_MONITOR_ADDR, 0xBA, 0x0);        // Exit Hibernate Mode step 2
    sendWrite(BAT_MONITOR_ADDR, 0x60, 0x0);        // Exit Hibernate Mode step 3

    sendWrite(BAT_MONITOR_ADDR, 0x18, designcap); // Write DesignCap
    sendWrite(BAT_MONITOR_ADDR, 0x1E, ichgterm);  // Write IchgTerm
    sendWrite(BAT_MONITOR_ADDR, 0x3A, vempty);    // Write VEmpty

    sendWrite(BAT_MONITOR_ADDR, 0xDB, 0x8000); // Write ModelCFG


    sendWrite(BAT_MONITOR_ADDR, 0x1D,  0x2210); // Write CONFIG1
    sendWrite(BAT_MONITOR_ADDR, 0xBB,  0x365A); // Write CONFIG2

    //Poll ModelCFG.Refresh(highest bit),
    //proceed to Step 3 when ModelCFG.Refresh=0.
    while (readRegister(BAT_MONITOR_ADDR, 0xDB) & 0x8000)
      delay(1);  //do not continue until ModelCFG.Refresh==0
    sendWrite(BAT_MONITOR_ADDR, 0xBA, HibCFG); // Restore Original HibCFG value
  }

  int Status = readRegister(BAT_MONITOR_ADDR, 0x00);   //Read Status
  WriteAndVerifyRegister(0x00, Status & 0xFFFD); //Write and Verify

  StatusPOR = readRegister(BAT_MONITOR_ADDR, 0x00) & 0x0002; //Read POR bit in Status Register

#ifdef DEBUG
  int RepCap = readRegister(BAT_MONITOR_ADDR, 0x05); //Read RepCap
  Serial.print("RepCap 0x");
  Serial.println(RepCap, HEX);

  int RepSOC = readRegister(BAT_MONITOR_ADDR, 0x06); //Read RepSOC
  Serial.print("RepSOC 0x");
  Serial.println(RepSOC, HEX);

  int TTE = readRegister(BAT_MONITOR_ADDR, 0x11); //Read TTE
  Serial.print("TTE 0x");
  Serial.println(TTE, HEX);

  int VCELL = readRegister(BAT_MONITOR_ADDR, 0x09); //Read VCELL
  Serial.print("VCELL 0x");
  Serial.println(VCELL, HEX);
#endif
}

float getBatteryCurrent() {
  return ((short)readRegister(BAT_MONITOR_ADDR, 0x0A)) * 0.00015625; // for 0.010 ohm shunt
}


float getTimeUntilEmpty() {
  int a = readRegister(BAT_MONITOR_ADDR, 0x11);
  float t = (float)a * 5.625; //gives time til empty in seconds
  t = (t) / 3600; //convert to hours
  return t;
}

float getBatteryVoltage() {
  int a = readRegister(BAT_MONITOR_ADDR, 0x19);
  float voltage = (float)a * 0.000078125;
  return voltage;
  //return 3.19; //debug
}

//returns in Ah
float getTotalCapacity() {
  int a = readRegister(BAT_MONITOR_ADDR, 0x10);
  return a * 0.0005;
}

//returns in Ah
float getRemainingCapacity() {
  int a = readRegister(BAT_MONITOR_ADDR, 0x05);
  return a * 0.0005;
}

int getBatteryPercentage() {
  int a = readRegister(BAT_MONITOR_ADDR, 0x06);
  a = a >> 8;
  return a;
}

void WriteAndVerifyRegister(char RegisterAddress, int RegisterValueToWrite)
{
  int Attempt = 0;
  int RegisterValueRead;
  do
  {
    sendWrite(BAT_MONITOR_ADDR, RegisterAddress, RegisterValueToWrite);
    delay(1); //1ms

    RegisterValueRead = readRegister(BAT_MONITOR_ADDR, RegisterAddress);
  } while (RegisterValueToWrite != RegisterValueRead && Attempt++ < 3);
}

void sendWrite(byte deviceAddr, byte location, int d)
{
  Wire.beginTransmission(BAT_MONITOR_ADDR);
  Wire.write(location);
  Wire.write(d & 0x00FF);
  Wire.write(d >> 8);
  Wire.endTransmission();
}

int readRegister(byte deviceAddr, byte location)
{
  Wire.beginTransmission(deviceAddr); // select device with "beginTransmission()"
  Wire.write(location);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddr, 2, true); //request battery percentage

  int  a = Wire.read();
  a |= Wire.read() << 8;

  //  int a = Wire.read() << 8 | Wire.read();
  return a;
}
