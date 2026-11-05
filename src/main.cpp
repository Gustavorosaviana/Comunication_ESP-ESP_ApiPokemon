#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Botao.h>
#include <LiquidCrystal_I2C.h>

const char* SSID = "SENAI EDUCACIONAL";
const char* SENHA = "info@134";

const char* BROKER = "broker.emqx.io"; 
const int PORTA = 1883;
const char* TOPICO_SUB = "senai134/esp32/eduardo-gustavo/comando";
const char* TOPICO_PUB = "senai134/esp32/eduardo-gustavo/status";

LiquidCrystal_I2C lcd(0x27, 20, 4);
Botao btnCima(13);
Botao btnBaixo(14);
Botao btnEnter(15);

WiFiClient wifiClient; 
PubSubClient mqtt(wifiClient);

void tratarJson(String);
void aoReceber(char*, byte*, unsigned int);
void telaInicial();
void statusConexao();
void conexao();


void setup() {
  Serial.begin(115200);
  Serial.println();

  lcd.init();
  lcd.backlight();

  String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool conectadoAoBroker = false;
  conectadoAoBroker = mqtt.connect(id.c_str());

  conexao();


  //* ============================CONEXAO SERIAL============================
  Serial.printf("Conectando na rede %s", SSID);
  rgbLedWrite(48, 255, 0, 0);
  delay(500);


  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    
    rgbLedWrite(48, 0, 0, 0);
    delay(500);
    rgbLedWrite(48, 255, 255, 0);
    delay(500);
  }
  delay(1000);

  Serial.printf("\n\rConectado a rede %s com sucesso", SSID);
  rgbLedWrite(48, 255, 255, 0);

  Serial.printf("\n\rConectando ao Broker %s\n\r", BROKER);

  
  mqtt.setServer(BROKER, PORTA);
  mqtt.setCallback(aoReceber);

  conectadoAoBroker = mqtt.connect(id.c_str());

  if(conectadoAoBroker){

    Serial.printf("Conectado ao broker %S com sucesso!\n\r", BROKER);

    Serial.printf("Inscrito no tópico: %s\n\r", TOPICO_SUB);
    rgbLedWrite(48, 0, 255, 0);
  }
  else{
    Serial.printf("Falha ao conectar ao broker %s\n\r");
    Serial.print("Falha: "); Serial.println(mqtt.state());
    rgbLedWrite(48, 255, 0, 0);
    delay(2000);
    rgbLedWrite(48, 255, 255, 0);
  }

  btnCima.iniciar();
  btnBaixo.iniciar();
  btnEnter.iniciar();

  telaInicial();
}

void loop() {
  mqtt.loop();
  statusConexao();
}




void aoReceber(char* topico, byte* payload, unsigned int tamanho){
  String mensagem;
  mensagem.reserve(tamanho); //* evita realocações enquanto a string cresce.

  for(unsigned int i = 0; i < tamanho; i++) { mensagem += (char)payload[i]; }

  Serial.printf("Mensagem Recebida no tópico: %s \n\r mensagem:\n\r %s\n\n\r", topico, mensagem.c_str());

  rgbLedWrite(48, 0, 0, 0);
  delay(500);
  rgbLedWrite(48, 0, 0, 255);
  delay(200);

  tratarJson(mensagem);
}


void tratarJson(String mensagem){
  JsonDocument doc;

  DeserializationError codigoErro = deserializeJson(doc, mensagem);

  bool estadoLed = doc["estadoLed"].as<bool>();

  if(estadoLed){
    Serial.println("LED LIGADO");
    rgbLedWrite(48,0,0,255);
  }
  else{
    Serial.println("LED DESLIGADO");
    rgbLedWrite(48,0,0,0);
  }
}


void statusConexao(){
  const unsigned long tempoEntreEnvios = 5000;
  static unsigned long ultimoEnvio = 0;
  bool conectado = mqtt.state() == MQTT_CONNECTED ? true : false;
  
  const char* usuario = "GustavoRosa";
  IPAddress IP = WiFi.localIP();
  unsigned int timestamp = 1793796662;
  String mensagemErro = "Sem erro";


  if(millis() - ultimoEnvio > tempoEntreEnvios){

    Serial.println("Formatando as mensagens de erro");

    ultimoEnvio = millis();
    JsonDocument doc;
    doc["conectado"] = conectado;
    doc["usuario"] = usuario;
    doc["IP"] = IP.toString();
    doc["timestamp"] = timestamp;
    doc["mensagemErro"] = mensagemErro;


    String mensagem;
    serializeJson(doc,mensagem);

    mqtt.publish(TOPICO_PUB, mensagem.c_str());
    Serial.printf("Mensagem a ser enviada no topico %s: \n\r%s",TOPICO_PUB, mensagem.c_str());
  }
}

void telaInicial()
{
    
  
  
  
  
  
    lcd.setCursor(0, 0);
    lcd.print("SELECIONE O ID:  ");
    lcd.setCursor(16, 0);

    lcd.setCursor(0, 1);
    lcd.print("NOME:    ");
    lcd.setCursor(6, 2);

    lcd.setCursor(0, 2);
    lcd.print("ENTER = CONFIRMAR");
}


void conexao(){
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi...");

  WiFi.begin(SSID, SENHA);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi conectado!");
  delay(500);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.printf("Conectando ao Broker %s\n\r", BROKER);
  
  mqtt.setServer(BROKER, PORTA);
  mqtt.setCallback(aoReceber);

  String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

  bool conectadoAoBroker = false;
  conectadoAoBroker = mqtt.connect(id.c_str());

  if(conectadoAoBroker){
    lcd.clear();
    
    lcd.printf("Conectado ao broker %S com sucesso!", BROKER);
    rgbLedWrite(48, 0, 255, 0);
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.printf("Falha ao conectar ao broker %s");
    rgbLedWrite(48, 255, 0, 0);
    delay(2000);
    rgbLedWrite(48, 255, 255, 0);
  }
}