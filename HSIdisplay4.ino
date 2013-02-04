#include <SPI.h>
#include <Ethernet.h>
#include "HT1632.h"
#include "TimerOne.h"

byte mac[] = {  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x06 };
char serverName[] = "www.google.com";
String textToScroll = "";
String nextText;
String tempString = ""; //else setting nexttext will skip some if too slow
String tempString2 = ""; //from detected l_cur" or c : " onwards - so we can detect the next " and hence get the number between "" regardless of its no. of digits
volatile int lengthTextToScroll = 0;
volatile int currentX = 32;

int priceLocChar = 0; int changeLocChar = 0; //location of HSI price and change

EthernetClient client;
HT1632LEDMatrix matrix = HT1632LEDMatrix(2,3,4); //DATA,WR,CS

String returnedHTML;
String uparrow = ""; String downarrow = "";

void setup() {
  if (Ethernet.begin(mac) == 0) {
    nextText = "Failed to config ethernet";
    while(true);
  }
  matrix.begin(HT1632_COMMON_8NMOS);
  matrix.fillScreen();
  matrix.setTextSize(1);
  matrix.setTextColor(1);
  Timer1.initialize(86000);
  uparrow += char(24);
  downarrow += char(25);
  textToScroll = "Hello";
  lengthTextToScroll = (-6*textToScroll.length())+2;
  nextText = textToScroll;
  delay(2000);
  Timer1.attachInterrupt(scroller);
  tryAgain:
  delay(3000);
  if (client.connect(serverName, 80)) {
    client.println("GET /finance/info?infotype=infoquoteall&q=HSI HTTP/1.0");
    client.println();
  } 
  else {
    nextText = "Internet Failed";
    goto tryAgain;
  }

  delay(500);
}

int charsReceived;
char c;

void loop()
{
  
  if (client.connected()) { //ONLY DO IF CONNECTED
  returnedHTML = "";
  charsReceived = 0;
  while (client.available()) {
    c = client.read();
    charsReceived++;
    //Serial.print(c);
    if (charsReceived > 400)
      returnedHTML += c; //disregard useless first part
    if (charsReceived > 860) break;
  }
  } //END ONLY DO IF CONNECTED
  if (!returnedHTML.equals("")) { //ONLY IF WE GOT DATA
  if (!client.connected()) { client.stop(); }
  priceLocChar = returnedHTML.indexOf("l_cur");
  changeLocChar = returnedHTML.indexOf("c\" : \"");
  if ((priceLocChar!=0)&&(priceLocChar!=NULL)) { tempString = "^HSI:" + returnedHTML.substring(priceLocChar+13,priceLocChar+19);
  tempString += " ";
  if ((changeLocChar!=0)&&(changeLocChar!=NULL)) {
  tempString2 = returnedHTML.substring(changeLocChar+6,returnedHTML.length());
  if (tempString2.startsWith("-"))
  {
  tempString += downarrow;
  tempString += tempString2.substring(1,tempString2.indexOf(".")); //0 is first char? or 1.
  } //stock gone down
  else
  {
    tempString += uparrow;
    tempString += tempString2.substring(1,tempString2.indexOf("."));
  }
  nextText = tempString;
  } } //end x and y are not 0 or null.
  
  } //END ONLY IF WE GOT DATA
  
  client.stop(); // NEW AND UNTESTED...
 if (!returnedHTML.equals("")) delay(100000); else { nextText = "Problem."; delay(2000); }
  
  tryAgain2:
  if (client.connect(serverName, 80)) {
    client.println("GET /finance/info?infotype=infoquoteall&q=HSI HTTP/1.0");
    client.println();
    delay(500); 
  } 
  else {
    nextText = "Internet Failed";
    delay(2000);
    goto tryAgain2;
  }
}

void scroller()
{
  matrix.clearScreen();
  matrix.setCursor(currentX,0);
  matrix.print(textToScroll);
  matrix.writeScreen();
  currentX--;
  if ((currentX) < lengthTextToScroll)
    {
      currentX = 32;
      textToScroll = nextText;
      lengthTextToScroll = (-6*textToScroll.length())+2;
    }
}

