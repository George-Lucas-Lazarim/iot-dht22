// --- WIFI ---
#include <ESP8266WiFi.h>
const char* ssid = "NINO1";
const char* password = "ninosom3351";
WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "mqtt.eclipseprojects.io";
const char* mqtt_ClientID = "termometro-glazarim01";
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "labglazarim/temperatura";
const char* topicoUmidade = "labglazarim/umidade";

// --- DHT ---
#include <DHT.h>

#define DHTPIN D3
#define DHTTYPE DHT22

DHT dht(DHTPIN,DHTTYPE);

int umidade;
int temperatura;

// --- DISPLAY ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//--- SETUP ---
void setup() {
  Serial.begin(9600);
  configurarDisplay();
  conectarWifi();
  client.setServer(mqtt_Broker, 1883);
  dht.begin();
}

//--- LOOP ---
void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  medirTemperaturaUmidade();
  publicarTemperaturaUmidadeNoTopico();
  mostrarTemperaturaUmidade();
}

//--- FUNÇÕES AUXILIARES ---

// --- CONFIGURACAO E CONEXAO WIFI ---
void conectarWifi() {
  delay(10);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Conectando");
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
}

// --- RECONECTAR O CLIENTE MQTT ---
void reconectarMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_ClientID);
  }
}

// --- CONFIGURA DISPLAY ---
void configurarDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
}
// --- PUBLICA (MQTT) TEMPERATURA E UMIDADE ---
void publicarTemperaturaUmidadeNoTopico() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);
}

// --- MEDIR TEMPERATURA E UMIDADE ---
void medirTemperaturaUmidade() {
  delay(5000);
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
}

// --- MOSTRA TEMPERATURA E UMIDADE ---

void mostrarTemperaturaUmidade() {
  mostrarMensagemNoDisplay("Temperatura", temperatura, " C");
  mostrarMensagemNoDisplay("Umidade", umidade, " %");
}

// --- EXIBE TEMPERATURA E UMIDADE NO DISPLAY ---
void mostrarMensagemNoDisplay(const char* texto1, int medicao, const char* texto2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(texto1);
  display.setTextSize(5);
  display.setCursor(20, 20);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
}
