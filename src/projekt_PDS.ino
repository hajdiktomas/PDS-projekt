#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define RST_PIN 1  // D4 // RST-PIN for RC522 - RFID - SPI - Modul GPIO15 // pre blikanie modrej LED vymeniť 2 a 15 
#define SS_PIN  15 // D8 // SDA-PIN for RC522 - RFID - SPI - Modul GPIO2
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

#define cnt_filtering_val 10

#define ESP_CONECTED_LED 2  // D2 - pouzita
#define CLEAN_PIN 4 // SD3 - pouzity
#define DOCOFFE_PIN 5   // D1 - pouzity
#define POWER_PIN 16    // D0 -pouzity

//#define COFFE_PIN   10   // D3
//#define WATER_PIN   0  // SD3

//#define ERROR_COFFE_LED 0     // NEPOUZIVA SA
#define ERROR_WATER_LED 3     // D3
//#define ERROR_RUN_WARNING 10     // D2

#define wifi_ssid "wifi"
#define wifi_password "heslo"

WiFiClient espClient;
PubSubClient client(espClient);
long last = 0;


char msg[100];
int filtering_indexCOFFE = cnt_filtering_val;
int filtering_indexWATER = cnt_filtering_val;

int filtering_indexGERRNLED = 25;
int indexGERRNLED = 0;


int Watter = 0; // 0
int BussyState = 0;
int FirstStart = 1;


int oncePrintWaterErr = 0;
int oncePrintGreenLED = 1;

char* topic_stat = "coffee/stat";
char* topic_cmd = "coffee/cmd" ;

#define mqtt_server "mqttserver"
#define mqtt_password "password"
#define mqtt_user "user"

const size_t SIZE = JSON_OBJECT_SIZE(4) + 125;
StaticJsonBuffer<SIZE> jsonBuffer;

unsigned int NFC_id = 0;

int filter(int *index, int act_val){
  if(act_val){
    if(*index != 0){
      *index -= 1;
      if(*index <= 0) *index = 0;
      return 0;
    }
    else return 1;
  }   
  else if(!act_val){
    *index = cnt_filtering_val;
    return 0;
  }  
}

int filterGreenLed(int *val){    
    int act_val = *val;
    // nsvieti svieti led => act_val = 0
    if(!act_val){
      indexGERRNLED++;
      if(indexGERRNLED > 6){
        indexGERRNLED = 6;
        return 1; // off nesvieti
      }
    }
    else if(act_val){
      indexGERRNLED = 0;
      return 0; // on svieti
    };
    return 0;
}



// function set amount of water and coffe and starts knock coffee
void startDoCoffe(){
  delay(200);
  digitalWrite(DOCOFFE_PIN, LOW);
  delay(900);
  digitalWrite(DOCOFFE_PIN, HIGH);
 
  
  // cas vyroby kavy
  // delay(200);
}

// function starts cleaning
void startClean(){
  digitalWrite(CLEAN_PIN, LOW);
  delay(900);
  digitalWrite(CLEAN_PIN, HIGH);

  // cas cistenia
  // delay(200);
}

// function power off or power on
void startPowerSwitching(){    
  digitalWrite(POWER_PIN, LOW);
  delay(900);
  digitalWrite(POWER_PIN, HIGH);
  
  // cas zapnutia / vypnutia
  // delay(200);
}

/* konvertuje bajty identifikačnej karty na unsigned int */
void convertCardByte(byte *uidByte, unsigned int *card) {
  *card =  uidByte[0] << 24;
  *card += uidByte[1] << 16;
  *card += uidByte[2] <<  8;
  *card += uidByte[3];
  return;
}

/* kontrola prilozenej karty */
int readCard(){
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return (1);
  }
  /* nacitanie prilozenej karty */
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return (1);
  }  
  /* konverzia uidByte na unsigned int a obsluha prilozenej karty */
  convertCardByte(mfrc522.uid.uidByte, &NFC_id);
  //  Serial.println("Load UID tag :"); 
  //  Serial.println(NFC_id,DEC);
  //  Serial.println(NFC_id,HEX);

  return 0;
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
     digitalWrite(ESP_CONECTED_LED, !digitalRead(ESP_CONECTED_LED));
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void cmd_clean() {
      if (!BussyState){
        startClean();
        snprintf (msg, 50, "{\"Action\":\"Cleaning\"\"State\":\"On\"}}");
      }
      else {
        snprintf (msg, 50, "{\"Action\":\"Cleaning\",\"State\":\"Off\"}");        
      }
      client.publish(topic_stat, msg,true);    
}
void cmd_turnOn() {
       if (FirstStart){
         startPowerSwitching();    
         snprintf (msg, 50, "{\"Action\":\"TurningOn\"\"State\":\"On\"}}");
         FirstStart = 0;
      }
      if (!BussyState){
        startPowerSwitching();    
        snprintf (msg, 50, "{\"Action\":\"TurningOn\"\"State\":\"On\"}}");
      }
      else {
        snprintf (msg, 50, "{\"Action\":\"TurningOn\",\"State\":\"Off\"}");        
      }
      client.publish(topic_stat, msg,true);
}

void cmd_turnOff() {
      if (!BussyState){
        startPowerSwitching();      
        snprintf (msg, 50, "{\"Action\":\"TurningOff\",\"State\":\"On\"}}");
      }
      else {
        snprintf (msg, 50, "{\"Action\":\"TurningOff\",\"State\":\"Off\"}");        
      }
      client.publish(topic_stat, msg,true);
}

void cmd_makeCoffe() {
      if (!BussyState){
        startDoCoffe();      
        snprintf (msg, 50, "{\"Action\": \"MakingCoffee\",\"State\":\"On\"}");
      }
            else {
        snprintf (msg, 50, "{\"Action\":\"MakingCoffee\",\"State\":\"Off\"}");        
      }
      client.publish(topic_stat, msg,true);
}

void callback(char* topic, byte* payload, unsigned int length) {

    JsonObject& root = jsonBuffer.parseObject((char *)payload);
  
    int Clean = root["Clean"];
    int TurnOn = root["TurnOn"];
    int TurnOff = root["TurnOff"];
    int Coffe = root["MakeCoffe"];

    if(Clean)
      cmd_clean();
      
    else if (TurnOn)
      cmd_turnOn();
      
    else if (TurnOff)
      cmd_turnOff();
      
    else if (Coffe)
      cmd_makeCoffe();
      
    jsonBuffer.clear();  
}

void reconnect() {
  if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
    client.subscribe(topic_cmd);
    
    digitalWrite(ESP_CONECTED_LED, LOW);
  }
  else {
    for(int i = 0; i < 12; i++){
      digitalWrite(ESP_CONECTED_LED, !digitalRead(ESP_CONECTED_LED));
      delay(250);
    }
  }
}

void initPins(){
  pinMode(ESP_CONECTED_LED, OUTPUT);
  digitalWrite(CLEAN_PIN, HIGH);
  
  pinMode(CLEAN_PIN, OUTPUT);
  digitalWrite(CLEAN_PIN, HIGH);
  
  pinMode(DOCOFFE_PIN, OUTPUT);
  digitalWrite(DOCOFFE_PIN, HIGH); 
  
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
 
  pinMode(ERROR_WATER_LED, INPUT);
}

void handleErrorWater(){
    int val = filter(&filtering_indexWATER, digitalRead(ERROR_WATER_LED));  

    delay(10);
    
    if (val == 1){
      if(oncePrintWaterErr == 0){
        Watter = 0; // FAIL 1
        sendStat();
        oncePrintWaterErr = 1;
      }
    }
    else{
      if(oncePrintWaterErr == 1){
        Watter = 1; // OK 0
        sendStat();
        oncePrintWaterErr = 0;
      }
    }
}

void handleOkGreenLed(){
    int act_val = analogRead(A0);
    int diodeLight = !filterGreenLed(&act_val); // 1- svieti 0-nesvieti
   

    if(diodeLight){     
               
    }      
    else{ 
       
    }
    snprintf (msg, 50, "{\":%u \tfVal %u \tStatled  }", act_val, diodeLight);
    client.publish(topic_stat, msg,true);
}

void sendStat(){
   if((!Watter)&&(!BussyState)) 
      client.publish(topic_stat, "{\"EmptyWatter\":\"true\",\"Ready\":\"true\"}",true);
      
   else if((!Watter)&&(BussyState))
      client.publish(topic_stat, "{\"EmptyWatter\":\"true\",\"Ready\":\"false\"}",true);
      
   else if((Watter)&&(!BussyState))
      client.publish(topic_stat, "{\"EmptyWatter\":\"false\",\"Ready\":\"true\"}",true);

   else
      client.publish(topic_stat, "{\"EmptyWatter\":\"false\",\"Ready\":\"false\"}",true);
}

void setup() {
  
  Serial.begin(9600);
  Serial.println("");  
  
  initPins();

  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  client.setCallback(callback);
  
  client.subscribe(topic_cmd);
}

void loop() {
  /* checking wifi connection */
  if (WiFi.status() != WL_CONNECTED) {      
      delay(100);
      //Serial.print(".");
      digitalWrite(ESP_CONECTED_LED, !digitalRead(ESP_CONECTED_LED));
      return;
  }
  ArduinoOTA.handle();
  /* checking MQTT connection */
  if (!client.connected()) {
    reconnect(); 
    return;   
  }
  client.loop();  


  handleOkGreenLed();
  handleErrorWater();

  

  long now = millis();
  if((now - last > 3000) && (readCard() == 0)){
    last = now;
      snprintf (msg, 50, "{\"Action\": \"ReadCard\",\"CardID\":%u}", NFC_id);
      client.publish(topic_stat, msg,true);
  }


  
}
