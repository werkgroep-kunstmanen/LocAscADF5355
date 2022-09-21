// Local oscillator met de ADF5355 voor 8GHz satellieten

#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <LCDKeypad.h> // https://github.com/dzindra/LCDKeypad

LCDKeypad lcd;

// Connector on the LCD-shield
// D13 D12 D11 D3 D2 D1 D0

int nrbits = 31; //Always one lower: 24 bits -> 23
const int slaveSelect = 2;
const int dat = 12;
const int clk = 13;

boolean aanuit = true;

int setsat0 = 0;
int setsat1 = 0;
int selectsat = 0;
int div8 = 0;
long register0, register1, register2, register6;
String message1;
String message2;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // For the ADF5355
  pinMode(slaveSelect, OUTPUT);
  pinMode(dat, OUTPUT);
  pinMode(clk, OUTPUT);
  digitalWrite(slaveSelect,HIGH); //deselect slave

  // Power for the level converter
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);

  delay(1000);
  
  // setsat1 = EEPROM.read(0);
  
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("ADF5355");
  delay(2000);
}


void sendCommand(long value)
{
  digitalWrite(slaveSelect,LOW); //chip select is active low
  delay(1); 
  
  for (int i = nrbits; i >= 0; i--)
  {
    aanuit = bitRead(value, i);

    digitalWrite(dat,aanuit);
    //delay(1);
    digitalWrite(clk,HIGH);
    //delay(1);
    digitalWrite(clk,LOW);
  }

  digitalWrite(slaveSelect,HIGH); //release chip, signal end transfer
  delay(1);
}

void loop()
{

  switch(lcd.buttonBlocking())
  {
    case KEYPAD_UP:
      if (setsat1 < 6)
        setsat1++;
      break;
    case KEYPAD_DOWN:
      if(setsat1 > 0)
        setsat1--;
      break;
    case KEYPAD_SELECT:
      selectsat = 1;
      break;
    case KEYPAD_LEFT:
      if (div8 == 1)
      {
        register6 = 0x3560E446;
        lcd.setCursor(12,0);
        lcd.print("DIV8");
        div8 = 0;
      }
      else
      {
        register6 = 0x3500E446;
        lcd.setCursor(12,0);
        lcd.print("    ");
        div8 = 1;
      }
      break;
  }
      
  if (setsat1 != setsat0)
  {
    switch(setsat1)
    {
      case 1:
        register0 = 0x002007D0;
        register1 = 0x0B333331;
        register2 = 0x3337FFF2;
                 // 1234567890123456
        message1 = "Syracuse    ";
        message2 = "LO 6285 MHz     ";
        break;
      case 2:
        register0 = 0x002007F0;
        register1 = 0x0D70A3D1;
        register2 = 0x258D5552;
        message1 = "NOAA 20     ";
        message2 = "LO 6392 MHz     ";
        break;
      case 3:
        register0 = 0x00200800;
        register1 = 0x00000001;
        register2 = 0x00000012;
        message1 = "FengYun 3D  ";
        message2 = "LO 6400 MHz     ";
        break;
      case 4:
        register0 = 0x00200860;
        register1 = 0x028F5C21;
        register2 = 0x2FC95552;
        message1 = "Meteor-M2 2 ";
        message2 = "LO 6708 MHz     ";
        break;
      case 5:
        register0 = 0x00200860;
        register1 = 0xCCCCCC1;
        register2 = 0xCCCBFFF2;
        message1 = "Aqua        ";
        message2 = "LO 6740 MHz     ";
        break;
      case 6:
        register0 = 0x00200870;
        register1 = 0x0D999991;
        register2 = 0x999BFFF2;
        message1 = "Terra       ";
        message2 = "LO 6792,5 MHz   ";
        break;
    }

    lcd.setCursor(0,0);
    lcd.print(message1);
    lcd.setCursor(0,1);
    lcd.print(message2);

    setsat0 = setsat1;
  }
  
  if (selectsat == 1)
  {
    // 1. Write R12
    sendCommand(0x0001041C);

    // 1. Write R11
    sendCommand(0x0061300B);

    // 1. Write R10
    sendCommand(0x00C01F7A);

    // 1. Write R9
    sendCommand(0x15153CC9);

    // 1. Write R8
    sendCommand(0x102D0428);

    // 1. Write R7
    sendCommand(0x120000E7);

    // 1. Write R6
    sendCommand(register6);

    // 1. Write R5
    sendCommand(0x00800025);
    
    // 2. Write R4
    sendCommand(0x32008B84); 
    
    // 3. Write R3
    sendCommand(0x00000003);

    // 4. Write R2
    sendCommand(register2);
    
    // 5. Write R1
    sendCommand(register1);

    // 6. Write R0
    sendCommand(register0);

    EEPROM.write(0, setsat0);
    
    selectsat = 0;
  }

}
