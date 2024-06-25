#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

int relay1 = 16;
int relay2 = 17;

int buttonUp = 26;
int buttonDown = 25;
int buttonTop = 32;
int buttonBottom = 33;
int limitUp = 14;
int limitDown = 12;

bool atTop = false;
bool atBottom = false;
bool lifting = false;
bool lowering = false;
bool noButtons = true;

//Wireless Communication Stuff

//MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0xE6, 0xAB, 0x10};

// Define variables to store incoming readings
int incoming_buttonUp_state = 1;
int incoming_buttonDown_state = 1;
int incoming_buttonTop_state = 1;
int incoming_buttonBottom_state = 1;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int buttonUp;
    int buttonDown;
    int buttonTop;
    int buttonBottom;
} struct_message;

// Create a struct_message to hold incoming sensor readings
struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
//  Serial.print("Bytes received: ");
//  Serial.println(len);
  incoming_buttonUp_state = incomingReadings.buttonUp;
  incoming_buttonDown_state = incomingReadings.buttonDown;
  incoming_buttonTop_state = incomingReadings.buttonTop;
  incoming_buttonBottom_state = incomingReadings.buttonBottom;
}


void setup() {
   // Init Serial Monitor
  Serial.begin(115200);

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonTop, INPUT_PULLUP);
  pinMode(buttonBottom, INPUT_PULLUP);
  pinMode(limitUp, INPUT_PULLUP);
  pinMode(limitDown, INPUT_PULLUP);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
   // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

}

void goUp(){
  if (atTop == false){
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
  }else{
    Serial.println("Can't go up, at Top");
  }
}

void goDown(){
  if (atBottom == false){
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
  }else{
    Serial.println("Can't go down, at Bottom");
  }
}

void stop(){
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
}

void loop() {

  // Display Readings in Serial Monitor
  Serial.print(" Button Up: ");
  Serial.print(incomingReadings.buttonUp);
  Serial.print(" Button Down: ");
  Serial.print(incomingReadings.buttonDown);
  Serial.print(" Button Top: ");
  Serial.print(incomingReadings.buttonTop);
  Serial.print(" Button Bottom: ");
  Serial.print(incomingReadings.buttonBottom);
  Serial.println();

  // digitalRead function stores the Push button state in variable push_button_state
  int buttonUp_state = digitalRead(buttonUp);
  int buttonDown_state = digitalRead(buttonDown);
  int buttonTop_state = digitalRead(buttonTop);
  int buttonBottom_state = digitalRead(buttonBottom);
  int limitUp_state = digitalRead(limitUp);
  int limitDown_state = digitalRead(limitDown);

  //Check Limit Switches
  if (limitUp_state == LOW){
    Serial.println("Limit Up");
    atTop = true;
    lifting = false;
  }else if (limitDown_state == LOW){
    Serial.println("Limit Down");
    atBottom = true;
    lowering = false;
  }else{
    atTop = false;
    atBottom = false;
  }

  //Check Buttons
  if (buttonUp_state == LOW || incoming_buttonUp_state == LOW){
    Serial.println("Button Up");
    noButtons = false;
    lifting = false;
    lowering = false;
    goUp();
  }else if (buttonDown_state == LOW || incoming_buttonDown_state == LOW){
    Serial.println("Button Down");
    noButtons = false;
    lifting = false;
    lowering = false;
    goDown();
  }else if (buttonTop_state == LOW || incoming_buttonTop_state == LOW){
    Serial.println("Button Top ---------- TOP");
    noButtons = false;
    lifting = true;
    lowering = false;
  }else if (buttonBottom_state == LOW || incoming_buttonBottom_state == LOW){
    Serial.println("Button Bottom -----------  BOTTOM");
    noButtons = false;
    lowering = true;
    lifting = false;
  }else{
    noButtons = true;
  }

  
  //Check Modes
  if (lifting == true){
    goUp();
  }else if(lowering == true){
    goDown(); 
  }else if (noButtons == true){
    stop();
  }

  delay(200);
  

}
