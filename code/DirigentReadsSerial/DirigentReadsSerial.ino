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
 
MilliTimer timeout;

#define LED_PIN   9 // activity LED, comment out to disable
static void activityLed (byte on) {
#ifdef LED_PIN
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, !on);
#endif
}

byte command; // command received through the serial port.  It's a single byte, containing all the info we need: the first bit is the on/off signal, the rest is the node id.
byte ids_target[2] = {3,16}; // Array with ids of the individual nodes.

typedef struct {
    byte boiler :1; // 0 or 1, desired boiler status
    byte pump   :1; // 0 or 1, desired pump status
} Request;

typedef struct {
    byte humi   :7;  // humidity: 0..100
    int temp    :10; // temperature: -500..+500 (tenths)
    int dew     :10; // dewpoint temperature
    byte boiler :1; // 0 or 1, boiler status
    byte pump   :1; // 0 or 1, pump status
} Answer;
 
void setup () {
  Serial.begin(57600);
  Serial.println("Dirigent \n\r");
 
  rf12_initialize(RF12_NODEID_GATEWAY, RF12_868MHZ, RF12_GROUPID);
}
 
void loop () {
  
  Serial.println("Starting the loop\r\n");
  // delay(1000);
        
  for (int i = 0; i < sizeof(ids_target); i++) {      
    
      while (!(Serial.available() > 0)) {;} // wait until there's a byte ready for reading in the serial port
          
      command = Serial.read();
      for (int i = 0; i < 2; i++) { 
                activityLed(true);
                delay(100);
                activityLed(false);
                delay(100);
              }
      // treat the command
      // make sure it is for the correct node.  Maybe this is not ok.  I should get the node id from the command instead
    
      // send the request to node ids_target[i]
      while (!rf12_canSend()) {rf12_recvDone();}  // wait for an opportunity to send
      
      activityLed(1);
      delay(500);
      activityLed(0);
      
      Request request;
      request.boiler = 1; // later, get these from my python program
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
      
      //delay(100); // otherwise led blinking isn't visible
      //activityLed(0);
          
  
      timeout.set(1000); // wait 2 seconds for answer
      while (!timeout.poll())
      
          if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Answer)) {
              // got a good packet, treat its contents
              
              // blink led 3 times briefly for receiving from node 
              for (int i = 0; i < 3; i++) { 
                activityLed(true);
                delay(40);
                activityLed(false);
                delay(40);
              }
              
              int sender_id = (RF12_HDR_MASK & rf12_hdr);
              Serial.print("N:");
              Serial.print(sender_id);
              Serial.println();
  
              // Data from RFM12B returns in rf12_data
              const Answer* answer = (const Answer*) rf12_data;
              //Serial.print("Node: ");
              //Serial.print((word) p->node);
              float t,h,d;
              
              t = (answer->temp-0.5)/10;
              Serial.print("T:");
              Serial.println(t);
              h = answer->humi - 0.5;
              Serial.print("RV:");
              Serial.println(h);
              d = (answer->dew - 0.5)/10;
              Serial.print("D:");
              Serial.println(d);
              Serial.print("B:");
              Serial.println(answer->boiler);
              Serial.print("P:");
              Serial.println(answer->pump);
              
              //delay(1000) // wait 1 s for serial port to be filled with next command
             
          }
     }   
}

  
  
  
  
  
  
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
