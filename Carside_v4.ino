// ARDUINO #2: TCP SERVER + AN LED
#include <WiFiS3.h>
#include <Servo.h>

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
const int clawMin = 0; //be between 0 and 270
const int clawMax = 90;
int lastDoor = 0;
const int doorMin = 0; //be between 0 and 270
const int doorMax = 90;
int lastArm = 0;
const int armMin = 0; //be between 0 and 270
const int armMax = 180;
const int armMid = 90;

//esc/motor powering
byte escPin = 10; // signal pin for the ESC.
byte dirPin = 11;
int pwmDirVal = 1100;
int pwmESCVal = 1100;
const int throtBottomLimit = 124;
const int throtTopLimit = 128;
const int pwmTopLimit = 1700;
const int pwmBottomLimit = 1300;
Servo esc;
Servo dir;

//Servo powering
Servo clawServo;
Servo doorServo;
Servo armServo;

void setup() {
  Serial.begin(9600);
  clawServo.attach(5);

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
}

void loop() {
  // Wait for a TCP client from Arduino #1:
  WiFiClient client = TCPserver.available();

  if (client) {
    // Read the command from the TCP client:
    //client.read(message, 5);
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


    //interpretation
    //claw toggle
    if (claw == 1) {
      lastClaw = 1;
    } else if (claw == 0) {
      lastClaw = 0;
    }
    if (lastClaw == 0 && claw == 1){
      clawServo.write(clawMin);
    } else if (lastClaw == 1 && claw ==0){
      clawServo.write(clawMax);
    }

    //door toggle
    if (door == 1) {
      lastDoor = 1;
    } else if (door == 0) {
      lastDoor = 0;
    }
    if (lastDoor == 0 && door == 1){
      doorServo.write(doorMin);
    } else if (lastDoor == 1 && door ==0){
      doorServo.write(doorMax);
    }

    //arm controls
    if (arm == 1){
      lastArm = 1;
    } else if (arm == 255){
      lastArm = -1;
    } else if (arm == 0){
      lastArm == 0;
    }
    if (lastArm == 0 && arm == 1){
      armServo.write(armMax);
    } else if (lastArm == 1 && arm ==1){
      armServo.write(armMid);
    } else if (lastArm == 1 && arm ==255){
      armServo.write(armMin);
    }
    if (lastArm == 0 && arm == 255){
      armServo.write(armMin);
    } else if (lastArm == -1 && arm ==255){
      armServo.write(armMid);
    } else if (lastArm == -1 && arm ==1){
      armServo.write(armMax);
    }

    //ESC power+direction  
    if(throt <= throtBottomLimit){
      pwmDirVal = pwmTopLimit;
      pwmESCVal = map(throt,0,throtBottomLimit, pwmTopLimit,pwmBottomLimit);
    }else if(throt >= throtTopLimit){
      pwmDirVal = pwmBottomLimit;
      pwmESCVal = map(throt,throtTopLimit,255, pwmBottomLimit,pwmTopLimit);
    }else{
      pwmESCVal = pwmBottomLimit;
    }  
    esc.writeMicroseconds(pwmESCVal); // Send signal to ESC.
    dir.writeMicroseconds(pwmDirVal);
    Serial.print(pwmDirVal);
    Serial.print(", ");
    Serial.println(pwmESCVal);
  }
}
