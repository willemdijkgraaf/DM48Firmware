#include <OSCMessage.h>
#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
 SLIPEncodedSerial SLIPSerial(Serial1);
#endif

int _holes[12] = {1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10};
int _portAxValues[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int _portAx = 0;
int _buttonsMask;

void setup() 
{                
   //begin SLIPSerial just like Serial
  SLIPSerial.begin(460800);   // set this as high as you can reliably run on your platform

  pinMode(PIN_D2, INPUT_PULLUP); // round button
  pinMode(PIN_D3, INPUT_PULLUP); // up button
  pinMode(PIN_C6, INPUT_PULLUP); // down button
  pinMode(PIN_C7, INPUT_PULLUP); // slide button

  calibrate();
}

void loop()                     
{
  // READ BREATH SENSORS
  int val = analogRead(_portAx);
  if (_portAxValues[_portAx] != val) {
    _portAxValues[_portAx] = val;

    OSCMessage msg("/DM48/hole");
    msg.add(_holes[_portAx]);
    msg.add(val);
  
    SLIPSerial.beginPacket();
    msg.send(SLIPSerial);
    SLIPSerial.endPacket();
    msg.empty(); // free space occupied by message
  }
  
  _portAx = (_portAx + 1) % 12;
  
  // READ BUTTONS
  int buttonsMask = 15;
  // UP BUTTON
  if (digitalRead(PIN_D3)) {
    buttonsMask -= 1;
  }
  // DOWN BUTTON
  if (digitalRead(PIN_C6)) {
     buttonsMask -= 2;
  } 
  // SLIDE BUTTON
  if (digitalRead(PIN_C7)) {
     buttonsMask -= 4;
  } 
  // ROUND BUTTON
  if (digitalRead(PIN_D2)) {
     buttonsMask -= 8;
  } 

  if (_buttonsMask != buttonsMask) {
    _buttonsMask = buttonsMask;

    OSCMessage msg2("/DM48/buttons");
    msg2.add(buttonsMask);
    SLIPSerial.beginPacket();
    msg2.send(SLIPSerial);
    SLIPSerial.endPacket();
    msg2.empty(); // free space occupied by message

    if (buttonsMask == 3) {
      calibrate();      
    }
  }
}

void calibrate() {
    OSCMessage msg("/DM48/calibrate");
    for (int i = 0; i < 12; i++) {
      msg.add(_holes[i]);
      msg.add(analogRead(i));
    } 
    SLIPSerial.beginPacket();
    msg.send(SLIPSerial);
    SLIPSerial.endPacket();
    msg.empty(); // free space occupied by message
}


