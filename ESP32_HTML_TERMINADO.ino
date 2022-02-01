#include "esp_camera.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include "ESP32_FTPClient.h"

#include <NTPClient.h> //For request date and time
#include <WiFiUdp.h>
#include "time.h"

char* ftp_server = "192.168.0.184";
char* ftp_user = "hp";
char* ftp_pass = "Tec3763.";
char* ftp_path = "//";

const char* WIFI_SSID = "Entel";
const char* WIFI_PASS = "92456981";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", (-3600 * 3), 60000);

//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);

//---------------------VARIABLES GLOBALES-------------------------

String header; // Variable para guardar el HTTP request

String estadoSalida = "off";


const int sen_pres = 13;
const int se_temp = 12;

int var_temp=0;


const int dosis1 = 2;
const int  dosis2 = 14;
const int  dosis3 = 15;

int inic_serv_web=0;
int var_terminar=0;
int confirmacion_foto;
///NOS CONECTAMOS AL FTP////

ESP32_FTPClient ftp (ftp_server, ftp_user, ftp_pass, 5000, 2);

//////////////////////////////////////////////////////////////////////////////

// Pin definition for CAMERA_MODEL_AI_THINKER
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

camera_config_t config;


//------------------------CODIGO HTML------------------------------
String pagina = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<meta charset='utf-8' />"
                "<title>Servidor Web ESP32</title>"
                "</head>"
                "<body>"
                "<center>"
                "<h1>Servidor Web ESP32</h1>"
                "<p><a href='/on'><button style='height:50px;width:100px'>ON</button></a></p>"

                "<p><a href='/on2'><button style='height:50px;width:100px'>ON2</button></a></p>"

                "<p><a href='/on3'><button style='height:50px;width:100px'>ON3</button></a></p>"

                "<p><a href='/terminar'><button style='height:50px;width:100px'>TERMINAR</button></a></p>"
                "</center>"
                "</body>"
                "</html>";



////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);
  Serial.println("");
  pinMode(se_temp, INPUT);
  pinMode(sen_pres, INPUT) ;           //botón  como entrada     /
  pinMode(dosis1, OUTPUT) ;            //botón  como entrada
  pinMode(dosis2, OUTPUT) ;            //botón  como entrada
  pinMode(dosis3, OUTPUT) ;            //botón  como entrada


  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  initCamera();

  server.begin(); // iniciamos el servidor

  timeClient.begin();
  timeClient.update();

}

void loop() {
  var_temp = digitalRead(se_temp);
    while (var_temp == HIGH){
    if (digitalRead(sen_pres) == LOW) {
      ftp.OpenConnection();
      timeClient.update();
      takePhoto();
      if (confirmacion_foto > 20){
      ftp.CloseConnection();
      inic_serv_web = 10;// se inicia el servidor web
      
      while (inic_serv_web > 2) {
        WiFiClient client = server.available();
        if (client) {                             // Si se conecta un nuevo cliente
          Serial.println("New Client.");          //
          String currentLine = "";                //
          while (client.connected()) {            // loop mientras el cliente está conectado
            if (client.available()) {             // si hay bytes para leer desde el cliente
              char c = client.read();             // lee un byte
              Serial.write(c);                    // imprime ese byte en el monitor serial
              header += c;
              if (c == '\n') {                    // si el byte es un caracter de salto de linea
                // si la nueva linea está en blanco significa que es el fin del
                // HTTP request del cliente, entonces respondemos:
                if (currentLine.length() == 0) {
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  client.println();

                  // enciende y apaga el GPIO
                  ///DOSIS 1
                  if (header.indexOf("GET /on") >= 0) {
                    Serial.println("GPIO on");
                    estadoSalida = "on";
                    digitalWrite(dosis1, HIGH);
                  }
                  //DOSIS 2
                  if (header.indexOf("GET /on2") >= 0) {
                    Serial.println("GPIO on2");
                    estadoSalida = "on2";
                    digitalWrite(dosis1, HIGH);
                    digitalWrite(dosis2, HIGH);
                  }
                  ///DOSIS3
                  if (header.indexOf("GET /on3") >= 0) {
                    Serial.println("GPIO on3");
                    estadoSalida = "on3";
                    digitalWrite(dosis1, HIGH);
                    digitalWrite(dosis2, HIGH);
                    digitalWrite(dosis3, HIGH);
                  }
                  ////TERMIANR PROCESO DE WEBSERVER
                  if (header.indexOf("GET /terminar") >= 0) {
                    Serial.println("GPIO terminar");
                    estadoSalida = "terminar";
                    digitalWrite(dosis1, LOW);
                    digitalWrite(dosis2, LOW);
                    digitalWrite(dosis3, LOW);
                    var_terminar = 20;
                  }
                  // Muestra la página web
                  client.println(pagina);

                  // la respuesta HTTP temina con una linea en blanco
                  client.println();
                  break;
                } else { // si tenemos una nueva linea limpiamos currentLine
                  currentLine = "";
                }
              } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
                currentLine += c;      // lo agrega al final de currentLine
              }
            }
          }
        }
        // Limpiamos la variable header
        header = "";
        // Cerramos la conexión
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
        if (var_terminar >  10)
        {
          Serial.println("Sale del bucle");
          inic_serv_web = 0;
          var_terminar = 0;
          confirmacion_foto = 10;
        }
      }
    }
    }
    }
  }

////////////////////////////////////////////////////

void initCamera() {
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
    config.frame_size = FRAMESIZE_UXGA;//FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {

    return;
  }
}
void takePhoto() {

  camera_fb_t * fb = NULL;

  // Take Picture with Camera
  ;
  fb = esp_camera_fb_get();

  if (!fb) {

    return;
  }
  /*
     Upload to ftp server
  */
  ftp.ChangeWorkDir(ftp_path);
  ftp.InitFile("Type I");

  String nombreArchivo = timeClient.getFullFormattedTimeForFile() + ".jpg";

  int str_len = nombreArchivo.length() + 1;

  char char_array[str_len];
  nombreArchivo.toCharArray(char_array, str_len);

  ftp.NewFile(char_array);
  ftp.WriteData( fb->buf, fb->len );
  ftp.CloseFile();
  /*
     Free buffer
  */
  esp_camera_fb_return(fb);
  confirmacion_foto=69;
  Serial.print("Foto tomada");
}
//////////////////////
