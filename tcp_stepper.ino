#include <Stepper.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h> 

const int stepsPerRevolution = 400; // 5mm
float newSteps;
//stepper
Stepper mystepper(stepsPerRevolution, 2 , 3); //400 pulses = moves 5 mm

int counter = 0; 

int steps = 0;
int stepSize = 0;
int direction = -1;
int bobbinWidth = 30;//lengthof bobbin / width of wire // 12cm / 
int stepCount = 0; 
unsigned long previousMillis; 
long OnTime;     // milliseconds of on-time
long OffTime;    // milliseconds of off-time

  void Update(unsigned long currentMillis)
  {
    if((currentMillis - previousMillis >= OnTime))
    {
      previousMillis = currentMillis;  // Remember the time
    }
    else if ((currentMillis - previousMillis >= OffTime))
    {
      previousMillis = currentMillis;   // Remember the time
    }
  }

 
class Sweeper
{
  Servo servo;
  int pos;              // current servo position 
  int increment;        // increment to move for each interval
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
 
public: 
  Sweeper(int interval)
  {
    updateInterval = interval;
    increment = 1;
  }
  
  void Attach(int pin)
  {
    servo.attach(pin);
  }
  
  void Detach()
  {
    servo.detach();
  }
  
  void Update(unsigned long currentMillis)
  {
    if((currentMillis - lastUpdate) > updateInterval)  // time to update
    {
      lastUpdate = millis();
      pos += increment;
      servo.write(pos);
      if ((pos >= 180) || (pos <= 0)) // end of sweep
      {
        // reverse direction
        increment = -increment;
      }
    }
  }
};
//ETHERNET
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 2, 1);// shield ip
IPAddress server(192, 168, 2, 2); // my computer ip
EthernetClient client;

void setup() {

  Ethernet.begin(mac, ip);// Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 50007)) {
    Serial.println("connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  char values[20];
  memset(values, 0, sizeof(values));
  int count = 0;
  if (client.available()) {
    char wireGaugePixel;
    //float mm = wireGaugePixel;
    while (1){
      wireGaugePixel = client.read();
      if (wireGaugePixel == '\n') {
        break;
      }
      values[count]=wireGaugePixel;
      count++;
    }
    Serial.print(values);
    Serial.println();
    double newvalues; 
    newvalues = atof (values);
    //sscanf(values,"%f",&newvalues);
    Serial.print(newvalues);
    Serial.println();
    mystepper.setSpeed(1000);
    //steps = direction * stepSize * newvalues;
    //Serial.print(steps);
    //Serial.print(steps);
    //mystepper.setSpeed(motorSpeed);// step 1/100 of a revolution:
    //mystepper.step(1600);
    newSteps = ((400 * newvalues)/5);
    Serial.println();
    Serial.print("steps:");
    Serial.println(newSteps);
    
    //stepCount++;
    delay(20);
    //mystepper.step(newSteps);
    //delay(20);
    //for(int stepCount = 0; stepCount < 200; stepCount++) {
      //mystepper.step(-newSteps);
      //delay(2);
     
    //}
    //delay(1000);
    for(int i = 0; i < bobbinWidth; i++){
      for(int stepCount = 0; stepCount < newSteps ; stepCount++) {
        mystepper.step(1);
        delay(20);
       }      
    }

    //delay(1000);
    for(int i = 0; i < bobbinWidth; i++){
      for(int stepCount = newSteps; stepCount > 0 ; stepCount--) {
        mystepper.step(-1);
        delay(20);
       }      
    }
    
    //if (stepCount < 200);{
      //mystepper.step(-newSteps);
      //delay(2);
      //stepCount++;
    //}
    //if (stepCount > 200);{
      //mystepper.step(newSteps);
      //delay(2);
      //stepCount++;
    //}
  }
  

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (client.connected()) {
      client.print(inChar);
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
    setup();
    //while (true);
  }
}
