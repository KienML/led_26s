#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>      // a basic DS1307 library that return time as a time_t
// Add comment
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define SIM_TIME_HEADER 'H' // Header tag for simulation serial time message

#define TEST_PIN LED_BUILTIN

//Simulation time mesage format "Hmm,dd,hh,minmin"
const int NUMBER_OF_FIELDS = 4; //how many comma separated fields we expect
int simTimeValues[NUMBER_OF_FIELDS];  //array holding values for all the fields

//define 26 pins of 26 stations
const byte d11_Pin = 22;
const byte d12_Pin = 23;
const byte d13_Pin = 24;
const byte d14_Pin = 25;
const byte d15_Pin = 26;
const byte d16_Pin = 27;
const byte d17_Pin = 28;
const byte d18_Pin = 29;

const byte e11_Pin = 30;
const byte e12_Pin = 31;
const byte e13_Pin = 32;
const byte e14_Pin = 33;
const byte e15_Pin = 34;

const byte e21_Pin = 35;
const byte e22_Pin = 36;
const byte e23_Pin = 37;
const byte e24_Pin = 38;

const byte e31_Pin = 39;
const byte e32_Pin = 40;

const byte d21_Pin = 41;
const byte d22_Pin = 42;
const byte d23_Pin = 43;
const byte d24_Pin = 44;
const byte d25_Pin = 45;
const byte d26_Pin = 46;
const byte d27_Pin = 47;

const byte SIM_MODE_PIN = 50;

//Method of switching RADAs on/off
int g_simMonth = 0;
int g_simDay = 0;
int g_simHour = 0;
int g_simMinute = 0;

//Time table for 24 hours
const byte d11[48] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};
const byte d12[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
const byte d13[48] = {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
const byte d14[48] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};
const byte d15[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
const byte d16[48] = {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
const byte d17[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
const byte d18[48] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};

const byte e11[48] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0};
const byte e12[48] = {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1};
const byte e13[48] = {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
const byte e14[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
const byte e15[48] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};

const byte e21[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};
const byte e22[48] = {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
const byte e23[48] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0};
const byte e24[48] = {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1};

const byte e31[48] = {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
const byte e32[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};

const byte d21[48] = {1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1};
const byte d22[48] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0};
const byte d23[48] = {1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1};
const byte d24[48] = {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0};
const byte d25[48] = {0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0};
const byte d26[48] = {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0};
const byte d27[48] = {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1};

byte g_Index = 0;
byte g_calibIndex = 0;

bool g_SimDataStatus = false;

void setup()
{
  pinMode(TEST_PIN, OUTPUT);

  pinMode(d11_Pin, OUTPUT);
  pinMode(d12_Pin, OUTPUT);
  pinMode(d13_Pin, OUTPUT);
  pinMode(d14_Pin, OUTPUT);
  pinMode(d15_Pin, OUTPUT);
  pinMode(d16_Pin, OUTPUT);
  pinMode(d17_Pin, OUTPUT);
  pinMode(d18_Pin, OUTPUT);
  pinMode(e11_Pin, OUTPUT);
  pinMode(e12_Pin, OUTPUT);
  pinMode(e13_Pin, OUTPUT);
  pinMode(e14_Pin, OUTPUT);
  pinMode(e15_Pin, OUTPUT);
  pinMode(e21_Pin, OUTPUT);
  pinMode(e22_Pin, OUTPUT);
  pinMode(e23_Pin, OUTPUT);
  pinMode(e24_Pin, OUTPUT);
  pinMode(e31_Pin, OUTPUT);
  pinMode(e32_Pin, OUTPUT);
  pinMode(d21_Pin, OUTPUT);
  pinMode(d22_Pin, OUTPUT);
  pinMode(d23_Pin, OUTPUT);
  pinMode(d24_Pin, OUTPUT);
  pinMode(d25_Pin, OUTPUT);
  pinMode(d26_Pin, OUTPUT);
  pinMode(d27_Pin, OUTPUT);

  pinMode(SIM_MODE_PIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop()
{
  if (Serial.available() >= 12)
  {
    ProcessSimMessage();
  }

  //InitIndex(g_SimDataStatus);

  //DisplaySimTime();

  // Limit the Index in a range of [0:47]
  if (g_Index > 47)
  {
    g_Index = 0;
  }

  // Display time table
  if ((g_simMonth % 2) != 0)      // g_simMonth is odd (1, 3, 5, 7, 9, 11)
  {
    if ((g_simDay >= 1) && (g_simDay <= 10))
    {
      g_calibIndex = Calib(0, g_Index);

      WritePin(d11_Pin, d11[g_calibIndex]);
      WritePin(d12_Pin, d12[g_calibIndex]);
      WritePin(d13_Pin, d13[g_calibIndex]);
      WritePin(d14_Pin, d14[g_calibIndex]);
      WritePin(d15_Pin, d15[g_calibIndex]);
      WritePin(d16_Pin, d16[g_calibIndex]);
      WritePin(d17_Pin, d17[g_calibIndex]);
      WritePin(d18_Pin, d18[g_calibIndex]);

      WritePin(e11_Pin, e11[g_calibIndex]);
      WritePin(e12_Pin, e12[g_calibIndex]);
      WritePin(e13_Pin, e13[g_calibIndex]);
      WritePin(e14_Pin, e14[g_calibIndex]);
      WritePin(e15_Pin, e15[g_calibIndex]);

      WritePin(e21_Pin, e21[g_calibIndex]);
      WritePin(e22_Pin, e22[g_calibIndex]);
      WritePin(e23_Pin, e23[g_calibIndex]);
      WritePin(e24_Pin, e24[g_calibIndex]);

      WritePin(e31_Pin, e31[g_calibIndex]);
      WritePin(e32_Pin, e32[g_calibIndex]);

      WritePin(d21_Pin, d21[g_calibIndex]);
      WritePin(d22_Pin, d22[g_calibIndex]);
      WritePin(d23_Pin, d23[g_calibIndex]);
      WritePin(d24_Pin, d24[g_calibIndex]);
      WritePin(d25_Pin, d25[g_calibIndex]);
      WritePin(d26_Pin, d26[g_calibIndex]);
      WritePin(d27_Pin, d27[g_calibIndex]);

      g_Index++;
    }

    if (g_simDay == 11)
    {
      if ((g_simHour >= 0) && (g_simHour < 7))
      {
        g_calibIndex = Calib(0, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
      else if (g_simHour >= 7)
      {
        g_calibIndex = Calib(1, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
    }

    if ((g_simDay >= 12) && (g_simDay <= 20))
    {
      g_calibIndex = Calib(1, g_Index);

      WritePin(d11_Pin, d11[g_calibIndex]);
      WritePin(d12_Pin, d12[g_calibIndex]);
      WritePin(d13_Pin, d13[g_calibIndex]);
      WritePin(d14_Pin, d14[g_calibIndex]);
      WritePin(d15_Pin, d15[g_calibIndex]);
      WritePin(d16_Pin, d16[g_calibIndex]);
      WritePin(d17_Pin, d17[g_calibIndex]);
      WritePin(d18_Pin, d18[g_calibIndex]);

      WritePin(e11_Pin, e11[g_calibIndex]);
      WritePin(e12_Pin, e12[g_calibIndex]);
      WritePin(e13_Pin, e13[g_calibIndex]);
      WritePin(e14_Pin, e14[g_calibIndex]);
      WritePin(e15_Pin, e15[g_calibIndex]);

      WritePin(e21_Pin, e21[g_calibIndex]);
      WritePin(e22_Pin, e22[g_calibIndex]);
      WritePin(e23_Pin, e23[g_calibIndex]);
      WritePin(e24_Pin, e24[g_calibIndex]);

      WritePin(e31_Pin, e31[g_calibIndex]);
      WritePin(e32_Pin, e32[g_calibIndex]);

      WritePin(d21_Pin, d21[g_calibIndex]);
      WritePin(d22_Pin, d22[g_calibIndex]);
      WritePin(d23_Pin, d23[g_calibIndex]);
      WritePin(d24_Pin, d24[g_calibIndex]);
      WritePin(d25_Pin, d25[g_calibIndex]);
      WritePin(d26_Pin, d26[g_calibIndex]);
      WritePin(d27_Pin, d27[g_calibIndex]);

      g_Index++;
    }

    if (g_simDay == 21)
    {
      if ((g_simHour >= 0) && (g_simHour < 7))
      {
        g_calibIndex = Calib(1, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
      else if (g_simHour >= 7)
      {
        g_calibIndex = Calib(2, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
    }

    if ((g_simDay >= 22) && (g_simDay <= 31))
    {
      g_calibIndex = Calib(2, g_Index);

      WritePin(d11_Pin, d11[g_calibIndex]);
      WritePin(d12_Pin, d12[g_calibIndex]);
      WritePin(d13_Pin, d13[g_calibIndex]);
      WritePin(d14_Pin, d14[g_calibIndex]);
      WritePin(d15_Pin, d15[g_calibIndex]);
      WritePin(d16_Pin, d16[g_calibIndex]);
      WritePin(d17_Pin, d17[g_calibIndex]);
      WritePin(d18_Pin, d18[g_calibIndex]);

      WritePin(e11_Pin, e11[g_calibIndex]);
      WritePin(e12_Pin, e12[g_calibIndex]);
      WritePin(e13_Pin, e13[g_calibIndex]);
      WritePin(e14_Pin, e14[g_calibIndex]);
      WritePin(e15_Pin, e15[g_calibIndex]);

      WritePin(e21_Pin, e21[g_calibIndex]);
      WritePin(e22_Pin, e22[g_calibIndex]);
      WritePin(e23_Pin, e23[g_calibIndex]);
      WritePin(e24_Pin, e24[g_calibIndex]);

      WritePin(e31_Pin, e31[g_calibIndex]);
      WritePin(e32_Pin, e32[g_calibIndex]);

      WritePin(d21_Pin, d21[g_calibIndex]);
      WritePin(d22_Pin, d22[g_calibIndex]);
      WritePin(d23_Pin, d23[g_calibIndex]);
      WritePin(d24_Pin, d24[g_calibIndex]);
      WritePin(d25_Pin, d25[g_calibIndex]);
      WritePin(d26_Pin, d26[g_calibIndex]);
      WritePin(d27_Pin, d27[g_calibIndex]);

      g_Index++;
    }
  }

  else    // g_simMonth is even (2, 4, 6, 8, 10, 12)
  {
    if ((g_simDay >= 1) && (g_simDay <= 10))
    {
      g_calibIndex = Calib(2, g_Index);

      WritePin(d11_Pin, d11[g_calibIndex]);
      WritePin(d12_Pin, d12[g_calibIndex]);
      WritePin(d13_Pin, d13[g_calibIndex]);
      WritePin(d14_Pin, d14[g_calibIndex]);
      WritePin(d15_Pin, d15[g_calibIndex]);
      WritePin(d16_Pin, d16[g_calibIndex]);
      WritePin(d17_Pin, d17[g_calibIndex]);
      WritePin(d18_Pin, d18[g_calibIndex]);

      WritePin(e11_Pin, e11[g_calibIndex]);
      WritePin(e12_Pin, e12[g_calibIndex]);
      WritePin(e13_Pin, e13[g_calibIndex]);
      WritePin(e14_Pin, e14[g_calibIndex]);
      WritePin(e15_Pin, e15[g_calibIndex]);

      WritePin(e21_Pin, e21[g_calibIndex]);
      WritePin(e22_Pin, e22[g_calibIndex]);
      WritePin(e23_Pin, e23[g_calibIndex]);
      WritePin(e24_Pin, e24[g_calibIndex]);

      WritePin(e31_Pin, e31[g_calibIndex]);
      WritePin(e32_Pin, e32[g_calibIndex]);

      WritePin(d21_Pin, d21[g_calibIndex]);
      WritePin(d22_Pin, d22[g_calibIndex]);
      WritePin(d23_Pin, d23[g_calibIndex]);
      WritePin(d24_Pin, d24[g_calibIndex]);
      WritePin(d25_Pin, d25[g_calibIndex]);
      WritePin(d26_Pin, d26[g_calibIndex]);
      WritePin(d27_Pin, d27[g_calibIndex]);

      g_Index++;
    }

    if (g_simDay == 11)
    {
      if ((g_simHour >= 0) && (g_simHour < 7))
      {
        g_calibIndex = Calib(2, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
      else if (g_simHour >= 7)
      {
        g_calibIndex = Calib(1, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
    }

    if ((g_simDay >= 12) && (g_simDay <= 20))
    {
      g_calibIndex = Calib(1, g_Index);

      WritePin(d11_Pin, d11[g_calibIndex]);
      WritePin(d12_Pin, d12[g_calibIndex]);
      WritePin(d13_Pin, d13[g_calibIndex]);
      WritePin(d14_Pin, d14[g_calibIndex]);
      WritePin(d15_Pin, d15[g_calibIndex]);
      WritePin(d16_Pin, d16[g_calibIndex]);
      WritePin(d17_Pin, d17[g_calibIndex]);
      WritePin(d18_Pin, d18[g_calibIndex]);

      WritePin(e11_Pin, e11[g_calibIndex]);
      WritePin(e12_Pin, e12[g_calibIndex]);
      WritePin(e13_Pin, e13[g_calibIndex]);
      WritePin(e14_Pin, e14[g_calibIndex]);
      WritePin(e15_Pin, e15[g_calibIndex]);

      WritePin(e21_Pin, e21[g_calibIndex]);
      WritePin(e22_Pin, e22[g_calibIndex]);
      WritePin(e23_Pin, e23[g_calibIndex]);
      WritePin(e24_Pin, e24[g_calibIndex]);

      WritePin(e31_Pin, e31[g_calibIndex]);
      WritePin(e32_Pin, e32[g_calibIndex]);

      WritePin(d21_Pin, d21[g_calibIndex]);
      WritePin(d22_Pin, d22[g_calibIndex]);
      WritePin(d23_Pin, d23[g_calibIndex]);
      WritePin(d24_Pin, d24[g_calibIndex]);
      WritePin(d25_Pin, d25[g_calibIndex]);
      WritePin(d26_Pin, d26[g_calibIndex]);
      WritePin(d27_Pin, d27[g_calibIndex]);

      g_Index++;
    }

    if (g_simDay == 21)
    {
      if ((g_simHour >= 0) && (g_simHour < 7))
      {
        g_calibIndex = Calib(1, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
      else if (g_simHour >= 7)
      {
        g_calibIndex = Calib(0, g_Index);

        WritePin(d11_Pin, d11[g_calibIndex]);
        WritePin(d12_Pin, d12[g_calibIndex]);
        WritePin(d13_Pin, d13[g_calibIndex]);
        WritePin(d14_Pin, d14[g_calibIndex]);
        WritePin(d15_Pin, d15[g_calibIndex]);
        WritePin(d16_Pin, d16[g_calibIndex]);
        WritePin(d17_Pin, d17[g_calibIndex]);
        WritePin(d18_Pin, d18[g_calibIndex]);

        WritePin(e11_Pin, e11[g_calibIndex]);
        WritePin(e12_Pin, e12[g_calibIndex]);
        WritePin(e13_Pin, e13[g_calibIndex]);
        WritePin(e14_Pin, e14[g_calibIndex]);
        WritePin(e15_Pin, e15[g_calibIndex]);

        WritePin(e21_Pin, e21[g_calibIndex]);
        WritePin(e22_Pin, e22[g_calibIndex]);
        WritePin(e23_Pin, e23[g_calibIndex]);
        WritePin(e24_Pin, e24[g_calibIndex]);

        WritePin(e31_Pin, e31[g_calibIndex]);
        WritePin(e32_Pin, e32[g_calibIndex]);

        WritePin(d21_Pin, d21[g_calibIndex]);
        WritePin(d22_Pin, d22[g_calibIndex]);
        WritePin(d23_Pin, d23[g_calibIndex]);
        WritePin(d24_Pin, d24[g_calibIndex]);
        WritePin(d25_Pin, d25[g_calibIndex]);
        WritePin(d26_Pin, d26[g_calibIndex]);
        WritePin(d27_Pin, d27[g_calibIndex]);

        g_Index++;
      }
    }

    if (g_simDay >= 22)
    {
      g_calibIndex = Calib(0, g_Index);

      WritePin(d11_Pin, d11[g_calibIndex]);
      WritePin(d12_Pin, d12[g_calibIndex]);
      WritePin(d13_Pin, d13[g_calibIndex]);
      WritePin(d14_Pin, d14[g_calibIndex]);
      WritePin(d15_Pin, d15[g_calibIndex]);
      WritePin(d16_Pin, d16[g_calibIndex]);
      WritePin(d17_Pin, d17[g_calibIndex]);
      WritePin(d18_Pin, d18[g_calibIndex]);

      WritePin(e11_Pin, e11[g_calibIndex]);
      WritePin(e12_Pin, e12[g_calibIndex]);
      WritePin(e13_Pin, e13[g_calibIndex]);
      WritePin(e14_Pin, e14[g_calibIndex]);
      WritePin(e15_Pin, e15[g_calibIndex]);

      WritePin(e21_Pin, e21[g_calibIndex]);
      WritePin(e22_Pin, e22[g_calibIndex]);
      WritePin(e23_Pin, e23[g_calibIndex]);
      WritePin(e24_Pin, e24[g_calibIndex]);

      WritePin(e31_Pin, e31[g_calibIndex]);
      WritePin(e32_Pin, e32[g_calibIndex]);

      WritePin(d21_Pin, d21[g_calibIndex]);
      WritePin(d22_Pin, d22[g_calibIndex]);
      WritePin(d23_Pin, d23[g_calibIndex]);
      WritePin(d24_Pin, d24[g_calibIndex]);
      WritePin(d25_Pin, d25[g_calibIndex]);
      WritePin(d26_Pin, d26[g_calibIndex]);
      WritePin(d27_Pin, d27[g_calibIndex]);

      g_Index++;
    }
  }

  bool SIM_MODE = digitalRead(SIM_MODE_PIN);
  if (SIM_MODE) {
    DisplayOutput();
  }
}

// Determine one of three time modes including A:nomarl/B:early/C:late time
byte Calib(const byte modeChange, byte g_Index)
{
  switch (modeChange)
  {
    case 0:                  // Normal time (A)
      return g_Index;
      break;
    case 1:                  // Early time (B)
      if (g_Index == 0)
      {
        return (47);
      }
      else
        return (g_Index - 1);
      break;
    case 2:                  // Late time (C)
      if (g_Index == 47)
      {
        return 0;
      }
      else
        return (g_Index + 1);
      break;
    default:
      return g_Index;
      break;
  }
}

// Parse the simulation time message (Hmm,dd,hh,minmin) from serial port
void ProcessSimMessage()
{
  if (Serial.find(SIM_TIME_HEADER))   //or can use Serial.read() == 'H'
  {
    // Read the values
    for (int i = 0; i < NUMBER_OF_FIELDS; i++)
    {
      simTimeValues[i] = Serial.parseInt();
    }
    g_simMonth = simTimeValues[0];
    g_simDay = simTimeValues[1];
    g_simHour = simTimeValues[2];
    g_simMinute = simTimeValues[3];

    // Check if the g_simMinute is in a range of [0:30] or [31:59]
    // and initialize g_Index
    if ((g_simMinute >= 0) && (g_simMinute <= 30))
    {
      g_Index = (byte)(g_simHour << 1);     // g_Index = g_simHour*2
    }
    else if ((g_simMinute >= 31) && (g_simMinute <= 59))
    {
      g_Index = (byte)(g_simHour << 1) + 1;  // g_Index = g_simHour*2 + 1
    }

    g_SimDataStatus = true;
  }
  else
  {
    g_SimDataStatus = false;
  }
}

// Turn on/off each Pin based on time tables
void WritePin(const byte Pin, byte Index)
{
  digitalWrite(Pin, Index);
}

//
void InitIndex(bool messageStatus)
{
  if (messageStatus)
  {
    // Check if the g_simMinute is in a range of [0:30] or [31:59]
    // and initialize g_Index
    if ((g_simMinute >= 0) && (g_simMinute <= 30))
    {
      g_Index = (byte)(g_simHour << 1);     // g_Index = g_simHour*2
    }
    else if ((g_simMinute >= 31) && (g_simMinute <= 59))
    {
      g_Index = (byte)(g_simHour << 1) + 1;  // g_Index = g_simHour*2 + 1
    }
  }
}

// Diplay Time via the serial port
void DisplaySimTime()
{
  Serial.println("The simulation time values: ");
  Serial.print("Sim_Month: "); Serial.println(g_simMonth);
  Serial.print("Sim_Day: "); Serial.println(g_simDay);
  Serial.print("Sim_Hour: "); Serial.println(g_simHour);
  Serial.print("Sim_Minute: "); Serial.println(g_simMinute);
  Serial.println();
}

// Display Outputs via the serial port
void DisplayOutput()
{
  Serial.print("Month: "); Serial.print(g_simMonth); Serial.print(", ");
  Serial.print("Day: "); Serial.print(g_simDay); Serial.print(", ");
  Serial.print("Hour: "); Serial.print(g_simHour); Serial.print(", ");
  Serial.print("Minute: "); Serial.println(g_simMinute);
  Serial.print("g_Index: "); Serial.print(g_Index); Serial.print(", "); 
  Serial.print("g_calibIndex: "); Serial.println(g_calibIndex);
  
  // The states of 26 stations
  Serial.print(d11[g_calibIndex]);
  Serial.print(d12[g_calibIndex]);
  Serial.print(d13[g_calibIndex]);
  Serial.print(d14[g_calibIndex]);
  Serial.print(d15[g_calibIndex]);
  Serial.print(d16[g_calibIndex]);
  Serial.print(d17[g_calibIndex]);
  Serial.print(d18[g_calibIndex]);
  Serial.print(" ");

  Serial.print(e11[g_calibIndex]);
  Serial.print(e12[g_calibIndex]);
  Serial.print(e13[g_calibIndex]);
  Serial.print(e14[g_calibIndex]);
  Serial.print(e15[g_calibIndex]);
  Serial.print(" ");

  Serial.print(e21[g_calibIndex]);
  Serial.print(e22[g_calibIndex]);
  Serial.print(e23[g_calibIndex]);
  Serial.print(e24[g_calibIndex]);
  Serial.print(" ");

  Serial.print(e31[g_calibIndex]);
  Serial.print(e32[g_calibIndex]);
  Serial.print(" ");

  Serial.print(d21[g_calibIndex]);
  Serial.print(d22[g_calibIndex]);
  Serial.print(d23[g_calibIndex]);
  Serial.print(d24[g_calibIndex]);
  Serial.print(d25[g_calibIndex]);
  Serial.print(d26[g_calibIndex]);
  Serial.print(d27[g_calibIndex]);
  Serial.println(); Serial.println();

  delay(100);
}
