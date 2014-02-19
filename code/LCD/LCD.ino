// Code taken from http://blog.wickeddevice.com/?p=440
// Start of Gateway code
/// Based on the polleer code from <jc@wippler.nl>;
// 2011-11-23 <jc@wippler.nl>; http://opensource.org/licenses/mit-license.php
//
//
#include <JeeLib.h>;
#include <PortsLCD.h>

PortI2C myI2C (4);
LiquidCrystalI2C lcd (myI2C);

#define screen_width 16
#define screen_height 2
 
/*
  RF12 Communications
*/
#define RF12_GROUPID 100    // all nodes must be a member of the same group
                          // to communicate with each other
#define RF12_NODEID_GATEWAY  30      // Each node within a group must have a unique ID, 
 
MilliTimer timeout;

#define LED_PIN   9 // activity LED, comment out to disable
static void activityLed (byte on) {
#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, !on);
#endif
}


typedef struct {
    byte humi   :7;  // humidity: 0..100
    int temp    :10; // temperature: -500..+500 (tenths)
    int dew     :10; // dewpoint temperature
    byte boiler :1; // 0 or 1, boiler status
    byte pump   :1; // 0 or 1, pump status
} Answer;
 
void setup () {
    Serial.begin(57600);
    Serial.println("Ontvanger met LCD \n\r");
   
    rf12_initialize(RF12_NODEID_GATEWAY, RF12_868MHZ, RF12_GROUPID);
    
    // set up the LCD's number of rows and columns: 
    lcd.begin(screen_width, screen_height);
    // Print a message to the LCD.
    lcd.print("Dag Roger en Marja");
    lcd.setCursor(0,1);
    lcd.print("Overzicht verwarming:");
    delay(2000);
    lcd.clear();
}
 
void loop () {
  
    Serial.println("Starting the loop\r\n");
        
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Answer)) {
        // got a good packet, treat its contents
        
        // blink led 3 times briefly for receiving from node 
    //      for (int i = 0; i < 3; i++) { 
    //        activityLed(true);
    //        delay(40);
    //        activityLed(false);
    //        delay(40);
    //      }
        
        int sender_id = (RF12_HDR_MASK & rf12_hdr);
        Serial.print("Receiving from node ");
        Serial.print(sender_id);
        Serial.println();
    
        // Data from RFM12B returns in rf12_data
        const Answer* answer = (const Answer*) rf12_data;
        //Serial.print("Node: ");
        //Serial.print((word) p->node);
        float t,h,d;
        
        t = (answer->temp-0.5)/10;
        Serial.print("Temperatuur = ");
        Serial.print(t);
        Serial.print("\n\r");
        h = answer->humi - 0.5;
        Serial.print("Relatieve vochtigheid = ");
        Serial.print(h);
        Serial.print("\n\r");
        d = (answer->dew - 0.5)/10;
        Serial.print("Dauwpunt = ");
        Serial.print(d);
        Serial.print("\n\r");
        Serial.print("Boiler = ");
        Serial.println(answer->boiler);
        Serial.print("Pump = ");
        Serial.println(answer->pump);
        
        //print to lcd
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("N");
        lcd.print(sender_id);
        lcd.print(" ");
        lcd.print("T="); lcd.print(t,0);
        //lcd.print(" C");  
        //lcd.setCursor(0,1);
        lcd.print(" V="); lcd.print(h,0);
        //lcd.print(" perc");
        lcd.print(" ");
        if (answer->pump == 1) {
            lcd.print("P");
        }
        if (answer->boiler == 1) {
            lcd.print("K");
        }
    }
}


