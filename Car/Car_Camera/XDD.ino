#include "esp_camera.h"
#include "AsyncTCP.h"
#include <WiFi.h>
#include <base64.h>

#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

const char* ssid = "小騷包魚有C. 菌";
const char* password =  "sakun921118";

unsigned long timer1;

const uint16_t port = 9700;

//WiFiClient client;
AsyncClient* aClient = NULL;

//TaskHandle_t Task1;

void setUpPin();
void sendStream();
void wifiConnect();
void socketConnect();
bool cameraInitial();
void hintLed(int, int);
void startCameraServer();
void runSocketStream(void* pvParameters);
void runAsyncSocketClient();

void setup() {
  // Serial initial
  Serial.begin(115200);

  // Camera initial
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 19000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 25;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  
  sensor_t* s = esp_camera_sensor_get();
  s -> set_framesize(s, FRAMESIZE_QVGA);
  s -> set_hmirror(s, 1); 
  s -> set_vflip(s, 1); 

  // Pin initial
  pinMode(4, OUTPUT);
  pinMode(33, OUTPUT);

  digitalWrite(33, 1);

  // Wifi initial
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    hintLed(2, 700);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP: " + WiFi.localIP().toString());
  Serial.println("HOST: " + WiFi.gatewayIP().toString());

  timer1 = millis();

  //xTaskCreatePinnedToCore(runSocketStream, "Task1", 10000, NULL, 1, &Task1, 0);  
}

void loop() {
  runAsyncSocketClient();
  sendStream();
  /*if(client.connected()){
    while (client.available()){
      String line = client.readStringUntil('\n'); 
      Serial.print("recieve data => "); 
      Serial.println(line); 
    }
    camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Frame buffer could not be acquired");
    return;
  }*/

  //size_t base64EncodeLength = base64::getBase64EncodeLength(fb->len);
  //char* encodedImage = base64::encode(fb->buf, fb->len, base64EncodeLength);
  //esp_camera_fb_return(fb);
  //byte charImage[encodeImage.length() + 1];
  //encodeImage.getBytes(charImage, encodeImage.length()+1);
  //Serial.println(base64EncodeLength);
    
  /*client.print("IMAGE$");
  client.print(fb->len);
  client.println("$");

  client.write(fb->buf, fb->len);
  esp_camera_fb_return(fb);

  client.println("IMAGE DONE");
  
  //free(encodedImage);
  
  }
  else{
    socketConnect();
  }*/
}

void runSocketStream(void * pvParameters){
  /*while(true){
    if(client.connected()){
      Serial.println("I am in side!");
      sendStream();
      delay(1000);
    }
    else{
      socketConnect();
    }
  }*/
}


void socketConnect(){
  /*while (!client.connect(WiFi.gatewayIP(), port)) {
    Serial.println("Connection to host failed");
    hintLed(1, 850);
  }*/
}

bool cameraInitial(){
  
  
  return true;
}

void setUpPin(){
  
}

void wifiConnect(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    hintLed(2, 700);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP: " + WiFi.localIP().toString());
  Serial.println("HOST: " + WiFi.gatewayIP().toString());
}

void hintLed(int times, int flashDelay){
  for(int i=0; i<times; i++){
      delay(100);
      digitalWrite(33, 0);
      delay(50);
      digitalWrite(33, 1);
    }
    delay(flashDelay);
}

void sendStream(){
  if(millis() - timer1 < 10){
    Serial.print("DELAY ");
    Serial.println(aClient->space());
    return;
  }

  if(!aClient->canSend()){
    timer1 = millis();
    Serial.print("WAIT ");
    Serial.println(aClient->space());
    return;
  }
  
  if(!aClient){
    return;
  }

  Serial.print("IMAGE IN ");
  Serial.println(aClient->space());

  aClient->write("IMAGE DONE\n");

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Frame buffer could not be acquired");
    return;
  }

  //size_t base64EncodeLength = base64::getBase64EncodeLength(fb->len);
  //char* encodedImage = base64::encode(fb->buf, fb->len, base64EncodeLength);
  
  //byte charImage[encodeImage.length() + 1];
  //encodeImage.getBytes(charImage, encodeImage.length()+1);
  //Serial.println(base64EncodeLength);
  
  char imageCommand[16];
  sprintf(imageCommand, "IMAGE$%06d$", fb->len);
    
  aClient->write(imageCommand);
  aClient->write("\n");
  aClient->write((const char*) fb->buf, fb->len);
  esp_camera_fb_return(fb);
  
  timer1 = millis();
}


void runAsyncSocketClient(){
  if(aClient)//client already exists
    return;

  aClient = new AsyncClient();
  if(!aClient)//could not allocate client
    return;

  aClient->onError([](void * arg, AsyncClient * client, int error){
    Serial.println("Connect Error");
    aClient = NULL;
    delete client;
  }, NULL);

  aClient->onConnect([](void * arg, AsyncClient * client){
    Serial.println("Connected");
    aClient->onError(NULL, NULL);

    client->onDisconnect([](void * arg, AsyncClient * c){
      Serial.println("Disconnected");
      aClient = NULL;
      delete c;
    }, NULL);

    client->onData([](void * arg, AsyncClient * c, void * data, size_t len){
      //Serial.print("\r\nData: ");
      //Serial.println(len);
      uint8_t * d = (uint8_t*)data;
      for(size_t i=0; i<len;i++){
        Serial.write(d[i]);
      }
      //Serial.println((char*) data);
      //c->write((char*) data, len);
      //c->write("\n");
    }, NULL);

    //send the request
    //client->write("Hello\n");
  }, NULL);

  aClient->setAckTimeout(5000);

  if(!aClient->connect(WiFi.gatewayIP(), port)){
    Serial.println("Connect Failed");
    AsyncClient * client = aClient;
    aClient = NULL;
    delete client;
  }
}
