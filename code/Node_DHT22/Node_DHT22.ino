/// @dir sht11demo
/// Ports demo, reads out a SHT11 sensor connected via "something like I2C".
// 2009-02-16 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>

#define RF12_GROUPID 100    // all nodes must be a member of the same group
#define RF12_NODEID  16         // Each node within a group must have a unique ID

DHTxx dht (4); // connected to DIO1, = arduino pin 4 on port 1 of jeenode
const byte LED = 9; // this is the on-board led

// turn the on-board LED on or off
static void led (bool on) {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, on ? 0 : 1); // inverted logic
}

const byte PORTPUMP = 6; // LED on port 3, arduino pin 6
static void relay_pump(bool on) {
  pinMode(PORTPUMP, OUTPUT);
  digitalWrite(PORTPUMP, on ? 1 : 0);
}

const byte PORTBOILER = 5; // LED on port 2, arduino pin 5
static void relay_boiler(bool on) {
  pinMode(PORTBOILER, OUTPUT);
  digitalWrite(PORTBOILER, on ? 1 : 0);
}

typedef struct {
    byte boiler :1; // 0 or 1, desired boiler status
    byte pump   :1; // 0 or 1, desired pump status
} Request;

typedef struct {
    int humi    :7;  // humidity: 0..100
    int temp    :10; // temperature: -500..+500 (tenths)
    int dew     :10; // dewpoint temperature
    byte boiler :1; // 0 or 1, boiler status
    byte pump   :1; // 0 or 1, pump status
} Answer;

byte need_to_send;

void setup() {
    Serial.begin(57600);
    Serial.print("\nNode with id ");
    Serial.print(RF12_NODEID);
    
    // setup the wireless transmission
    rf12_initialize(RF12_NODEID, RF12_868MHZ, RF12_GROUPID);
}

void loop() {
    
    //Serial.println("Starting the loop\r\n");
 
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Request)) {
        // got a request, treat its contents
        led(true);
        Serial.println("Recieved request");
        Serial.print("\r\n");
        
        const Request* request = (const Request*) rf12_data;
        Serial.print("Desired boiler status = ");
        Serial.print(request->boiler);
        Serial.print("\r\n");
        
        Serial.print("Desired pump status = ");
        Serial.print(request->pump);
        Serial.print("\r\n");     
        
        // take action: set the relays !!
        relay_boiler(request->boiler == 1);
        
        relay_pump(request->pump == 1);
        
        
        delay(100);
        led(false);
        delay(100);
    
        // only send an answer if a request has been received and treated
        need_to_send = 1;
    }
    
    if (need_to_send && rf12_canSend()) {
        need_to_send = 0;
        // omit following code to avoid linking in floating point code
        Answer answer;
        
        Serial.print("\nDHT ");
    
        int t, h;
        if (dht.reading(t, h, true)) {  // This line will do a measurement, and get t and h
          Serial.print("Temperature = ");
          Serial.println(t);
          Serial.print("Relative humidity = ");
          Serial.println(h);
          Serial.println();
        }

        
        answer.humi = int(h / 10);
        answer.temp = t;
        Serial.print("answer.humi = ");
        Serial.println(answer.humi);
        Serial.print("answer.temp = ");
        Serial.println(answer.temp);
      
        answer.dew = 0;
        answer.boiler = 0;
        answer.pump = 1;
        
 
        // actual packet send: broadcast to all
        rf12_sendStart(0, &answer, sizeof answer);
        
        // blink led 3 times briefly for sending from node to dirigent
        for (int i = 0; i < 3; i++) { 
          led(true);
          delay(40);
          led(false);
          delay(40);
        }
    }
}
    


