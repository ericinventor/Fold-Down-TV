/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

int buttonUp = 26;
int buttonDown = 12;
int buttonTop = 27;
int buttonBottom = 14;

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x40, 0x91, 0x51, 0xFB, 0xE4, 0xE8};

// Define variables to store BME280 readings to be sent
int buttonUp_state;
int buttonDown_state;
int buttonTop_state;
int buttonBottom_state;

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

// Create a struct_message called BME280Readings to hold sensor readings
struct_message ButtonReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonTop, INPUT_PULLUP);
  pinMode(buttonBottom, INPUT_PULLUP);  
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  getReadings();
 
  // Set values to send
  ButtonReadings.buttonUp = buttonUp_state;
  ButtonReadings.buttonDown = buttonDown_state;
  ButtonReadings.buttonTop = buttonTop_state;
  ButtonReadings.buttonBottom = buttonBottom_state;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &ButtonReadings, sizeof(ButtonReadings));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(100);
}

void getReadings(){
  
   // digitalRead function stores the Push button state in variable push_button_state
  buttonUp_state = digitalRead(buttonUp);
  buttonDown_state = digitalRead(buttonDown);
  buttonTop_state = digitalRead(buttonTop);
  buttonBottom_state = digitalRead(buttonBottom);
  
  //Check Buttons
  if (buttonUp_state == LOW){
    Serial.println("Button Up");
  }else if (buttonDown_state == LOW){
    Serial.println("Button Down");
  }else if (buttonTop_state == LOW){
    Serial.println("Button Top ---------- TOP");
  }else if (buttonBottom_state == LOW){
    Serial.println("Button Bottom -----------  BOTTOM");
  }else{
    true;
  }

}
