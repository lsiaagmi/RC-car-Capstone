// ARDUINO #1: TCP CLIENT + A BUTTON/SWITCH
#include <ezButton.h>
#include <WiFiS3.h>

const char ssid[] = "LiamArduino";  // change your network SSID (name)
const char pass[] = "ItsWorking";   // change your network password (use for WPA, or use as key for WEP)

const int serverPort = 4080;
//clawBut=claw 2=door 3=arm up 4=arm down
ezButton clawBut(2);   //create ezButton that attach to pin 2;
ezButton doorBut(4);
ezButton armupBut(7);
ezButton armdwnBut(8);

int sidePin = A0;   // select the input pin for the left/right potentiometer
int sideValue = 0;  // variable to store the value coming from the left/right sensor
int vertPin = A1;
int vertValue = 0;
const int deadSideTopLimit = 129;
const int deadSideBottomLimit = 121;
const int deadVertTopLimit = 129; //ADD VALUES WHEN THE NEW JOYSTICK ARRIVES
const int deadVertBottomLimit = 124;

char message[5] = {0,0,0,0,0};  //TCP message: array of 0.steering 1.throttle 2. claw toggle 3. door toggle 4. arm dir


IPAddress serverAddress(192, 168, 0, 1);
WiFiClient TCPclient;
int status = WL_IDLE_STATUS;

//send message function
sendMessage(){
    TCPclient.write(message,5);
    TCPclient.flush();
    Serial.print("steer "); Serial.print(message[0]-0);
    Serial.print(", throtttle "); Serial.print(message[1]-0);
    Serial.print(", claw "); Serial.print(message[2]-0);
    Serial.print(", door "); Serial.print(message[3]-0);
    Serial.print(", arm "); Serial.println(message[4]-0);
}


void setup() {
  Serial.begin(9600);
  clawBut.setDebounceTime(50);  // set debounce time to 50 milliseconds
  doorBut.setDebounceTime(50);
  armupBut.setDebounceTime(50);
  armdwnBut.setDebounceTime(50);


  Serial.println("ARDUINO #1: TCP CLIENT + A BUTTON/SWITCH");

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

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // connect to TCP server (Arduino #2)
  if (TCPclient.connect(serverAddress, serverPort))
    Serial.println("Connected to TCP server");
  else
    Serial.println("Failed to connect to TCP server");
}

void loop() {
    clawBut.loop();  // MUST call the loop() function first
    doorBut.loop();
    armupBut.loop();
    armdwnBut.loop();

    vertValue = map(analogRead(vertPin), 0, 1023, 0, 255);
    sideValue = map(analogRead(sidePin), 0, 1023, 0, 255);

  if (!TCPclient.connected()) {
    Serial.println("Connection is disconnected");
    TCPclient.stop();

    // reconnect to TCP server (Arduino #2)
    if (TCPclient.connect(serverAddress, serverPort))
      Serial.println("Reconnected to TCP server");
    else
      Serial.println("Failed to reconnect to TCP server");
  }

  //BUTTON 1
  if (clawBut.isPressed()) {
    message[2] = 1;
    sendMessage()
  }

  if (clawBut.isReleased()) {
    message[2] = 0;
    //Serial.print("-Claw is released, sending message: ");
    sendMessage()

  //BUTTON 2
  if (doorBut.isPressed()) {
    message[3] = 1;
    //Serial.print("-Door pressed, sending message: ");
    sendMessage()
  }

  if (doorBut.isReleased()) {
    message[3] = 0;
    //Serial.print("-Door is released, sending message: ");
    sendMessage()
  }

  //BUTTON 3
  if (armupBut.isPressed()) {
    message[4] = 1;
    //Serial.print("-Arm up pressed, sending message: ");
    sendMessage()
  }

  if (armupBut.isReleased()) {
    message[4] = 0;
    //Serial.print("-Arm up is released, sending message: ");
    sendMessage()
  }

  //BUTTON 4
  if (armdwnBut.isPressed()) {
    message[4] = -1;
    //Serial.print("-Arm down pressed, sending message: ");
    sendMessage()
  }

  if (armdwnBut.isReleased()) {
    message[4] = 0;
    //Serial.print("-Arm down is released, sending message: ");
    sendMessage()
  }

  /*
  Serial.print("side: ");
  Serial.print(sideValue);
  Serial.print(" vert: ");
  Serial.println(vertValue);
  */
  
  //LEFT/RIGHT STICK or UP/DOWN STICK
  if (((message[0] != sideValue) && ((sideValue <= deadSideBottomLimit) || (sideValue >= deadSideTopLimit))) || 
        ((message[1] != vertValue) && ((vertValue <= deadVertBottomLimit) || (vertValue >= deadVertTopLimit)))){
    Serial.println("I am outside the dead zone");
    
    message[0] = sideValue;
    message[1] = vertValue;
    //Serial.print("-Joystick moved horizontally, sending message: ");
    sendMessage()
  }  
}