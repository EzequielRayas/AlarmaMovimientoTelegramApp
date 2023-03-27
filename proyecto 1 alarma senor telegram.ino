#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "Torre C Estudiantes";
const char* password = "1q2w3e4r5t";


// Inicializar BOT Telegram
#define BOTtoken "5517621271:AAGQIjgiiPbdyA7qX6yTbBGLtQCmYMDczXw" 

// hacer click en "start" antes de enviarle mensajes el bot
#define CHAT_ID "5752123114"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int buzPin = 25;
const int motionSensor = 27; 
bool motionDetected = false;

// Indica cuando se detecta movimiento
void IRAM_ATTR detectsMovement() {
  motionDetected = true;
}

void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuario No Autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/encender_alarma") {
      bot.sendMessage(chat_id, "Alarma activada", "");
      digitalWrite(buzPin, HIGH);
    }
    
    if (text == "/apagar_alarma") {
      bot.sendMessage(chat_id, "Alarma desactivada", "");
      digitalWrite(buzPin, LOW);
    }
  }
}

void setup() {
  //Serial.begin(115200);
  Serial.begin(9600);


  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  pinMode(buzPin, OUTPUT);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  
  if(motionDetected){
    bot.sendMessage(CHAT_ID, "¡Movimiento detectado en la bodega!", "");
    motionDetected = false;
  }
 
}