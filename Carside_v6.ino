#include <WiFiS3.h>
#include <Servo.h>
#include <Timer.h>
#include <Arduino.h>

const char ssid[] = "LiamArduino";  // change your network SSID (name)
const char pass[] = "ItsWorking";   // change your network password (use for WPA, or use as key for WEP)

const int serverPort = 4080;

WiFiServer TCPserver(serverPort);
int status = WL_IDLE_STATUS;

char message[5] = {0,0,0,0,0};
int steer = message[0];
int throt = message[1];
int claw = message[2];
int door = message[3];
int arm = message[4];

int lastClaw = 0;
const int clawOpen = 120; //be between 0 and 180 yay it works
const int clawClosed = 75;
int lastDoor = 0;
const int doorClosed = 88;
const int doorOpen = 152;
int lastArm = 0;
const int armMin = 153;
const int armMax = 50;
const int armMid = 90;

//steering stuff
const int steerRightLim = 73;
const int steerLeftLim = 100-steerRightLim;
int steerLocation = ((steerLeftLim + steerRightLim)/2);


bool stopMotor = false;

int lastThrot = 128;//throt;
Timer timer;
int lastTimer = 0;

//esc/motor powering
byte escPin = 10; // signal pin for the ESC.
byte dirPin = 11;
const int throtBottomLimit = 123;
const int throtTopLimit = 128;
const int pwmTopLimit = 1400;
const int pwmBottomLimit = 1100;
const int pwmFwd = 1100;
const int pwmBack = 1900; 
int pwmDirVal = pwmFwd;
int pwmESCVal = 1100;
Servo esc;
Servo dir;

//Servo powering
Servo clawServo;
Servo doorServo;
Servo armServo;
Servo steerServo;


void setup() {
  Serial.begin(115200);
  steerServo.attach(3);
  clawServo.attach(5);
  doorServo.attach(6);
  armServo.attach(9);

  esc.writeMicroseconds(pwmESCVal);
  dir.writeMicroseconds(pwmDirVal);

  Serial.println("ARDUINO #2: TCP SERVER + AN LED");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }


  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  WiFi.config(IPAddress(192,168,0,1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // Print your local IP address:
  Serial.print("TCP Server IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-> Please update the serverAddress in Arduino #1 code");

  // Listening for a TCP client (from Arduino #1)
  TCPserver.begin();

  //setup for esc
  esc.attach(escPin);
  esc.writeMicroseconds(1500); // send "stop" signal to ESC. Also necessary to arm the ESC.

  dir.attach(dirPin);
  dir.writeMicroseconds(1100);

  delay(7000); // delay to allow the ESC to recognize the stopped signal.
  Serial.println("Init");

  steerServo.write(steerLocation);

  
}

void loop() {
  // Wait for a TCP client from Arduino #1:
  WiFiClient client = TCPserver.available();
  if (client) {
    // Read the command from the TCP client:
    //Parse the payload into individual commands
    steer = client.read()-0; //message[0];
    throt = client.read()-0; // message[1];
    claw = client.read()-0; // message[2];
    door = client.read()-0; // message[3];
    arm = client.read()-0; // message[4];
    
    
    Serial.print("-Received message: ");
    Serial.print("steer "); Serial.print(steer);
    Serial.print(", throtttle "); Serial.print(throt);
    Serial.print(", claw "); Serial.print(claw);
    Serial.print(", door "); Serial.print(door);
    Serial.print(", arm "); Serial.println(arm);
    
    
    //Serial.print("last door "); Serial.println(lastDoor);

    //interpretation V
      //claw toggle V
        if(claw == 1){
          if (lastClaw == 0){
            clawServo.write(clawClosed);
            lastClaw = 1;
          } else {
            clawServo.write(clawOpen);
            lastClaw = 0;
          }
        }
        //_________________________________________________________________________

      //door toggle V
        if(door == 1){
          if (lastDoor == 0){
            doorServo.write(doorClosed);
            lastDoor = 1;
          } else {
            doorServo.write(doorOpen);
            lastDoor = 0;
          }
        }
        //_________________________________________________________________________

      //arm controls V
        if (arm != 0) {
          if (lastArm == 0) {  // arm is presently in middle position
            if (arm == 1) {
              armServo.write(armMax);
              Serial.print("upPos "); Serial.println(armMax);
              lastArm = 1;
            }
            else if (arm == 255) {
              armServo.write(armMin);
              Serial.print("dwnPos "); Serial.println(armMin);
              lastArm = -1;
            }
          } 
          else if (lastArm == 1) {  // arm is presntly in up position
            if (arm == 1) {
              armServo.write(armMid);
              lastArm = 0;
            }
            else if (arm ==255){
              armServo.write(armMin);
              lastArm = -1;
            }
          }
          else if (lastArm == -1) {  // arm is presently in down position
            if (arm == 255) {
              armServo.write(armMid);
              lastArm = 0;
            }
            else if (arm == 1){
              armServo.write(armMax);
              lastArm = 1;
            }
          }
        }
        //_________________________________________________________________________
      //steering
      if(steerLocation != map(steer, 0, 255, steerLeftLim, steerRightLim)){
        steerLocation = map(steer, 0, 255, steerLeftLim, steerRightLim);
        steerServo.write(steerLocation);
        //Serial.println("changing steer");
      } 

          
      //Serial.print("steer Location"); Serial.println(steerLocation);
      // Failsafe mechanims, if throttle is "stuck"
      if (timer.read() < (lastTimer+8000)) {
        if ((throt != lastThrot) || (throt <= throtTopLimit && throt >= throtBottomLimit)) {
          lastThrot = throt;
          timer.start();
          lastTimer = timer.read();
        }
      } else if (throt >= throtTopLimit || throt <= throtBottomLimit) {
        stopMotor = true;
      }
      

      //ESC power+direction V  
        if (stopMotor == false){
          if(throt <= throtBottomLimit){
            pwmDirVal = pwmBack;
            pwmESCVal = map(throt,0,throtBottomLimit, pwmTopLimit,pwmBottomLimit);
          }else if(throt >= throtTopLimit){
            pwmDirVal = pwmFwd;
            pwmESCVal = map(throt,throtTopLimit,255, pwmBottomLimit,pwmTopLimit);
          }else{
            pwmESCVal = 1100;
          }  
        } else {
          pwmESCVal = 1100;
          Serial.println("motor stopped");
        }
        esc.writeMicroseconds(pwmESCVal);
        dir.writeMicroseconds(pwmDirVal);
       /* Serial.print(pwmDirVal);
        Serial.print(", ");
        Serial.println(pwmESCVal);
        Serial.print(lastThrot); Serial.print(", "); 
        Serial.println(timer.read() - lastTimer);
        */
    //delay(15);
  }
}
