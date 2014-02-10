// Code taken from http://blog.wickeddevice.com/?p=440
// Start of Gateway code
/// Based on the polleer code from <jc@wippler.nl>;
// 2011-11-23 <jc@wippler.nl>; http://opensource.org/licenses/mit-license.php
//
//
#include <JeeLib.h>;

 
/*
  RF12 Communications
*/
#define RF12_GROUPID 100    // all nodes must be a member of the same group
                          // to communicate with each other
#define RF12_NODEID_GATEWAY  31      // Each node within a group must have a unique ID, this is the scketch for the jeelink
 
MilliTimer sendTimer;

#define LED_PIN   9 // activity LED, comment out to disable
static void activityLed (byte on) {
#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, !on);
#endif
}

byte needToSend;
byte ids_target[2] = {3,16}; // Array with ids of the individual nodes.

struct {
    byte boiler :1; // 0 or 1, desired boiler status
    byte pump   :1; // 0 or 1, desired pump status
} request;

struct {
    byte humi   :7;  // humidity: 0..100
    int temp    :10; // temperature: -500..+500 (tenths)
    int dew     :10; // dewpoint temperature
    byte boiler :1; // 0 or 1, boiler status
    byte pump   :1; // 0 or 1, pump status
} answer;
 
void setup () {
  Serial.begin(57600);
  Serial.println("Dirigent \n\r");
 
  rf12_initialize(RF12_NODEID_GATEWAY, RF12_868MHZ, RF12_GROUPID);
}
 
void loop () {
  
  Serial.println("Starting the loop\r\n");
  delay(5000);
        
  for (int i = 0; i < sizeof(ids_target); i++) {      
    
    delay(1000); 
    
    if (rf12_recvDone() && rf12_crc == 0) {
        activityLed(1);
        Serial.print("Receiving");
        for (byte i = 0; i < rf12_len; ++i)
            Serial.print(rf12_data[i]);
        Serial.println();
        delay(100); // otherwise led blinking isn't visible
        activityLed(0);
        }
    
    if (rf12_canSend()) {
        activityLed(1);
        
        request.boiler = 1;
        request.pump = 1;
        
        byte header = 0; // no acknowledgement
        header |= RF12_HDR_DST | ids_target[i];  // bitwise operation: add 1 in second byte position to specify that a destination is specified, and add the destination.
        Serial.print("RF12_HDR_DST = ");
        Serial.print(RF12_HDR_DST);
        Serial.print("\r\n");
        Serial.print("Header = ");
        Serial.print(header);
        Serial.print("\r\n");
        
        rf12_sendStart(header, &request, sizeof request);
        
        delay(100); // otherwise led blinking isn't visible
        activityLed(0);
        }
    }   
}
//     if (rf12_recvDone() && rf12_crc == 0) {
//        receiveLed(1);
//        Serial.print("OK ");
//        for (byte i = 0; i < rf12_len; ++i)
//            Serial.print(rf12_data[i]);
//        Serial.println();
//        delay(100); // otherwise led blinking isn't visible
//        receiveLed(0);
// }
    
    
 
  
  
  
  
  
  
  
  
  
//  if (rf12_recvDone()) {
//    Serial.println("rf12_recvDone()");
//    if (rf12_crc == 0) {
//      Serial.println("rf12_crc == 0");
//    }
//    
//    
//    // got a good ACK packet, print out its contents
//    // would be better to get the node id here, and only try to get the values when it's a sensor node
// 
//    // Data from RFM12B returns in rf12_data
//    const payload* p = (const payload*) rf12_data;
//    //Serial.print("Node: ");
//    //Serial.print((word) p->node);
//    float t,h;
//    t = (p->temp-0.5)/10;
//    Serial.print("Temperatuur = ");
//    Serial.print(t);
//    Serial.print("\n\r");
//    h = p->humi - 0.5;
//    Serial.print("Relatieve vochtigheid = ");
//    Serial.print(h);
//    Serial.print("\n\r");
//    
//  }

//End of Gateway code
