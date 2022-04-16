/*
   UPWORK - Makassar (Indonesia), 15/04/2022
   Syahrir Sobirin Mahyuddin / @syahrirmahyuddin
   to
   Vincenzo

*/
#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include <EEPROM.h>
#include <HardwareSerial.h>
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define flashLight 16
#define trig 13
#define EEPROM_SIZE 1
static HardwareSerial sim(1);
String file_Name = "";
char char_buffer;
String string_buffer = "";
int buffer_space = 1000;
int pictureNumber = 0;
Seria
void setup() {
  pinMode(trig,OUTPUT);
  digitalWrite(trig,1);
  delay(2500);
  digitalWrite(trig,0);
  Serial.begin(9600);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  pinMode(flashLight, OUTPUT);
  digitalWrite(flashLight, 1);
  delay(2000);
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  digitalWrite(flashLight, 0);
  sim.begin(9600, SERIAL_8N1, 2, 4); //2
  delay(5000);
  Serial.println("Starting...");
  byte reply = 1;
  int i = 0;
  while (i < 10 && reply == 1) { //Try 10 times...
    reply = sendATcommand(1, "AT+CREG?", "+CREG: 0,1", "ERROR", 1000);
    i++;
    delay(1000);
  }
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;
  file_Name = get_ccid() + "_RX" + String(get_network()) + "_BT" + String(get_battery()) + "_IMAGE" + String(pictureNumber);
  Serial.println(file_Name);
  get_battery();
  if (reply == 0) {
    reply = sendATcommand(1, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", "ERROR", 1000);
    if (reply == 0) {
      reply = sendATcommand(1, "AT+SAPBR=3,1,\"APN\",\"internet\"", "OK", "ERROR", 1000);
      if (reply == 0) {
        //reply = sendATcommand(1,"AT+SAPBR=3,1,\"USER\",\"entelpcs\"", "OK", "ERROR", 1000);
        if (reply == 0) {
          //reply = sendATcommand(1,"AT+SAPBR=3,1,\"PWD\",\"entelpcs\"", "OK", "ERROR", 1000);
          if (reply == 0) {
            reply = 2;
            i = 0;
            while (i < 3 && reply == 2) { //Try 3 times...
              reply = sendATcommand(1, "AT+SAPBR=1,1", "OK", "ERROR", 10000);
              if (reply == 2) {
                sendATcommand(1, "AT+SAPBR=0,1", "OK", "ERROR", 10000);
              }
              i++;
            }
            if (reply == 0) {
              reply = sendATcommand(1, "AT+SAPBR=2,1", "OK", "ERROR", 1000);
              if (reply == 0) {
                reply = sendATcommand(1, "AT+FTPCID=1", "OK", "ERROR", 1000);
                if (reply == 0) {
                  reply = sendATcommand(1, "AT+FTPSERV=\"51.91.98.192\"", "OK", "ERROR", 1000);
                  if (reply == 0) {
                    reply = sendATcommand(1, "AT+FTPPORT=21", "OK", "ERROR", 1000);
                    if (reply == 0) {
                      reply = sendATcommand(1, "AT+FTPUN=\"root\"", "OK", "ERROR", 1000);
                      if (reply == 0) {
                        reply = sendATcommand(1, "AT+FTPPW=\"IVQko1TU\"", "OK", "ERROR", 1000);
                        if (reply == 0) {
                          reply = sendATcommand(1, "AT+FTPPUTNAME=\"" + String(file_Name) + ".jpg\"", "OK", "ERROR", 1000);
                          if (reply == 0) {
                            reply = sendATcommand(1, "AT+FTPPUTPATH=\"/rilevazioni/\"", "OK", "ERROR", 1000);
                            if (reply == 0) {
                              unsigned int ptime = millis();
                              reply = sendATcommand(1, "AT+FTPPUT=1", "+FTPPUT: 1,1", "+FTPPUT: 1,6", 60000);
                              Serial.println("Time: " + String(millis() - ptime));
                              if (reply == 0) {
                                int i = 0;
                                String Encode;
                                for (size_t n = 0; n < fb->len; n++) {
                                  char_buffer = fb->buf[n];
                                  string_buffer.concat(char_buffer);
                                  i++;
                                  if (i == buffer_space) {
                                    sendATcommand(1, "AT+FTPPUT=2," + String(buffer_space), "AT+FTPPUT=2,10", "ERROR", 1000);
                                    sendATcommand(0, string_buffer, "OK", "ERROR", 5000);
                                    string_buffer = "";
                                    i = 0;
                                  }
                                }
                                if (string_buffer != "") {
                                  sendATcommand(1, "AT+FTPPUT=2," + String(i), "AT+FTPPUT=2,10", "ERROR", 1000);
                                  sendATcommand(0, string_buffer, "OK", "ERROR", 5000);
                                  sendATcommand(1, "AT+FTPPUT=2,0", "OK", "ERROR", 1000);
                                  EEPROM.write(0, pictureNumber);
                                  EEPROM.commit();
                                  digitalWrite(flashLight, 1);
                                  delay(50);
                                  digitalWrite(flashLight, 0);
                                  delay(200);
                                  digitalWrite(flashLight, 1);
                                  delay(50);
                                  digitalWrite(flashLight, 0);
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  esp_camera_fb_return(fb);
  Serial.println("The end...");

}

void loop() {
  // put your main code here, to run repeatedly:
}


byte sendATcommand(byte select, String ATcommand, String answer1, String answer2, unsigned int timeout) {
  byte reply = 1;
  String content = "";
  char character;

  //Clean the modem input buffer
  while (sim.available() > 0) sim.read();

  //Send the atcommand to the modem
  if (select == 1) {
    sim.println(ATcommand);
  }
  else {
    sim.print(ATcommand);
  }
  delay(100);
  unsigned int timeprevious = millis();
  while ((reply == 1) && ((millis() - timeprevious) < timeout)) {
    while (sim.available() > 0) {
      character = sim.read();
      content.concat(character);
      Serial.print(character);
      delay(10);
    }
    //Stop reading conditions
    if (content.indexOf(answer1) != -1) {
      reply = 0;
    } else if (content.indexOf(answer2) != -1) {
      reply = 2;
    } else {
      //Nothing to do...
    }
  }
  return reply;
}
int get_network() {
  String buff;
  unsigned int result, index1, index2, Timeout = 0;
  sim.println("AT+CSQ");
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (sim.available() > 0) {
      buff = sim.readString();
      Timeout = 1;
      break;
    }
  }
  if (Timeout == 0) {
    return 0;
  }
  Serial.println(buff);
  index1 = buff.indexOf("\r");
  buff.remove(0, index1 + 2);
  buff.trim();

  index1 = buff.indexOf(":");
  index2 = buff.indexOf(",");
  buff = buff.substring(index1 + 1, index2);
  buff.trim();
  result = buff.toInt();
  return result;

}
int get_battery() {
  String buff;
  unsigned int index1, index2, Timeout = 0;
  sim.println("AT+CBC");
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (sim.available() > 0) {
      buff = sim.readString();
      Timeout = 1;
      break;
    }
  }
  if (Timeout == 0) {
    return 0;
  }
  index1 = buff.indexOf("\r");
  buff.remove(0, index1 + 2);
  buff.trim();

  index1 = buff.indexOf(",");
  index2 = buff.indexOf(",", index1 + 1);
  String result = buff.substring(index1 + 1, index2);
  result.trim();
  return result.toInt();
}
String get_ccid() {
  String buff;
  unsigned int index1, index2, Timeout = 0;
  sim.println("AT+CCID");
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (sim.available() > 0) {
      buff = sim.readString();
      Timeout = 1;
      break;
    }
  }
  if (Timeout == 0) {
    return "";
  }
  index1 = buff.indexOf("\r");
  buff.replace("\n", "|");
  buff.remove(0, index1 + 2);
  buff.trim();
  index1 = buff.indexOf("|");
  index2 = buff.indexOf("|", index1 + 1);
  String result = buff.substring(index1 + 1, index2);
  result.trim();
  return result;
}
