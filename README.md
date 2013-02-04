LEDMatrixScroller
=================

The wall at Dim Sum Labs has a scrolling LED matrix, called "Chef",
showing the Hang Seng Index pulled from Google Finance.

Please hack it to do something more interesting! Inside it's just an Arduino Uno, ethernet shield, and LED matrix module.
To reprogram, use this code as base, put the 2 libraries Timer1 and HT1632 into your arduino/libraries directory, 
compile with Arduino-1.0, and plug into the USB on Chef to program the Arduino.

Currently Chef is plugged into USB which just provides power, and ethernet.

How about Twitter feeds, news items, random insults, clock ...
