/// @dir sht11demo
/// Ports demo, reads out a SHT11 sensor connected via "something like I2C".
// 2009-02-16 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>
#include <PortsSHT11.h>

SHT11 sht11 (1);
const byte LED = 9;

#define RF12_GROUPID 100    // all nodes must be a member of the same group
#define RF12_NODEID  3         // Each node within a group must have a unique ID
                                                   

// turn the on-board LED on or off
static void led (bool on) {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, on ? 0 : 1); // inverted logic
}

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

byte need_to_send;

void setup() {
    Serial.begin(57600);
    Serial.print("\nNode with id ");
    Serial.print(RF12_NODEID);
    
    // omit this call to avoid linking in the CRC calculation code
    sht11.enableCRC();
    
    // setup the wireless transmission
    // this is node 1 in net group 100 on the 868 MHz band
    rf12_initialize(RF12_NODEID, RF12_868MHZ, RF12_GROUPID);
}

void loop() {
    
    //Serial.println("Starting the loop\r\n");
 
    if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof (Request)) {
        // got a request, treat its contents
        led(true);
        Serial.println("Recieved request");
        
        const Request* request = (const Request*) rf12_data;
        Serial.print("Desired boiler status = ");
        Serial.print(request->boiler);
        Serial.print("\r\n");
        
        Serial.print("Desired pump status = ");
        Serial.print(request->pump);
        Serial.print("\r\n");     
        // take action: set the relays !!
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
        
        Serial.print("\nSHT ");
    
        uint8_t error = sht11.measure(SHT11::HUMI); 
        Serial.print("sht11.meas[SHT11::HUMI]: ");
        Serial.print(sht11.meas[SHT11::HUMI]);
        Serial.print("\n");
        
        error |= sht11.measure(SHT11::TEMP);
        Serial.print("sht11.meas[SHT11::TEMP]: ");
        Serial.print(sht11.meas[SHT11::TEMP]);
        Serial.print("\n");
        
        Serial.print("error, DEC: " );
        Serial.print(error, DEC);
        
        
        float h, t;
        sht11.calculate(h, t);
        answer.humi = h + 0.5;
        answer.temp = 10 * t + 0.5;
        Serial.print(" h:  ");
        Serial.print(h);
        Serial.print(" t: ");
        Serial.print(t);
      
        // omit following code to avoid linking in the log() math code
        float d = sht11.dewpoint(h, t);
        answer.dew = d;
        answer.boiler = 0;
        answer.pump = 1;
        
        Serial.print(" dewpoint: ");
        Serial.print(d);
  
        // actual packet send: broadcast to all
        rf12_sendStart(0, &answer, sizeof answer);
        
        for (int i = 0; i < 3; i++) { 
          led(true);
          delay(20);
          led(false);
          delay(20);
        }
    }
}
    
    
//    Serial.print("\nSHT ");
//
//    uint8_t error = sht11.measure(SHT11::HUMI); 
//    Serial.print("sht11.meas[SHT11::HUMI]: ");
//    Serial.print(sht11.meas[SHT11::HUMI]);
//    Serial.print("\n");
//    
//    error |= sht11.measure(SHT11::TEMP);
//    Serial.print("sht11.meas[SHT11::TEMP]: ");
//    Serial.print(sht11.meas[SHT11::TEMP]);
//    Serial.print("\n");
//    
//    Serial.print("error, DEC: " );
//    Serial.print(error, DEC);



