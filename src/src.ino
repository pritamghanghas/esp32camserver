#include "esp_camera.h"
#include "espselector.h"
#include <WiFi.h>

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"
            

void startCameraServer();



void setupConnection(const String& apName, const String& apPass, const String& selfAPName, const String& selfAPPass)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(apName.c_str(), apPass.c_str());
  int connRes = WiFi.waitForConnectResult();

  if(WiFi.isConnected()) {
    Serial.println("connected");
    return;
  }

  WiFi.mode(WIFI_AP);

  WiFi.softAP(selfAPName.c_str(), selfAPPass.c_str()); 
  delay(1000);
  Serial.println("I think we are ready");
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

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
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif
  // is configuration portal requested? for wifi configuration
  // ondemand config portal can go here but then auto connect doesn't work.
//  if (WiFi.isConnected() != WL_CONNECTED) {
//    WiFiManager wifiManager;
//    //reset saved settings
//    //wifiManager.resetSettings();
   
//    //set custom ip for portal
//    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
//    wifiManager.setPortalPort(8080);

//    //fetches ssid and pass from eeprom and tries to connect
//    //if it does not connect it starts an access point with the specified name
//    //here  "AutoConnectAP"
//    //and goes into a blocking loop awaiting configuration
//    auto ssid = String("COVCAM") + String(ESP_getChipId(), HEX);
//    auto password = String("password");
//    wifiManager.autoConnect(ssid.c_str(), password.c_str());
//    //or use this for auto generated name ESP + ChipID
//    //wifiManager.autoConnect();

   
//    //if you get here you have connected to the WiFi
//    Serial.println("connected...yeey :)");
//  }

auto ssid = String("COVCAM") + String(ESP_getChipId(), HEX);
auto password = String("netdeemak");

setupConnection("uid1", "netdeemak", ssid, password);

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
