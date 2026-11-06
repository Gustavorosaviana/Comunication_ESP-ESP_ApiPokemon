
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

void tratarJson(String mensagem);
void aoReceber(char*, byte*, unsigned int);
void telaInicial();
void statusConexao();
void conexao();

void setup() {
    Serial.begin(115200);

    lcd.init();
    lcd.backlight();

    conexao();

    btnCima.iniciar();
    btnBaixo.iniciar();
    btnEnter.iniciar();

    lcd.clear();
    telaInicial();
}

void loop() {
    mqtt.loop();
    statusConexao();
}

void aoReceber(char* topico, byte* payload, unsigned int tamanho) {

    String mensagem;
    mensagem.reserve(tamanho);

    for (unsigned int i = 0; i < tamanho; i++) {
        mensagem += (char)payload[i];
    }

    Serial.printf("Mensagem recebida no tópico: %s\n", topico);
    Serial.println(mensagem);

    rgbLedWrite(48, 0, 0, 0);
    delay(500);

    rgbLedWrite(48, 0, 0, 255);
    delay(200);

    tratarJson(mensagem);
}

void tratarJson(String mensagem) {

    JsonDocument doc;

    DeserializationError codigoErro = deserializeJson(doc, mensagem);

    if (codigoErro) {
        Serial.println("Erro ao interpretar JSON!");
        return;
    }

    bool estadoLed = doc["estadoLed"].as<bool>();

    if (estadoLed) {
        Serial.println("LED LIGADO");
        rgbLedWrite(48, 0, 0, 255);
    }
    else {
        Serial.println("LED DESLIGADO");
        rgbLedWrite(48, 0, 0, 0);
    }
}

void statusConexao() {

    const unsigned long tempoEntreEnvios = 5000;
    static unsigned long ultimoEnvio = 0;

    bool conectado = mqtt.connected();

    const char* usuario = "GustavoRosa";

    IPAddress IP = WiFi.localIP();

    unsigned int timestamp = 1793796662;

    String mensagemErro = "Sem erro";

    if (millis() - ultimoEnvio >= tempoEntreEnvios) {

        ultimoEnvio = millis();

        JsonDocument doc;

        doc["conectado"] = conectado;
        doc["usuario"] = usuario;
        doc["IP"] = IP.toString();
        doc["timestamp"] = timestamp;
        doc["mensagemErro"] = mensagemErro;

        String mensagem;

        serializeJson(doc, mensagem);

        if (mqtt.connected()) {
            mqtt.publish(TOPICO_PUB, mensagem.c_str());
        }

        Serial.printf(
            "Mensagem enviada no tópico %s:\n%s\n",
            TOPICO_PUB,
            mensagem.c_str()
        );
    }
}

void telaInicial() {

    lcd.setCursor(0, 0);
    lcd.print("SELECIONE O ID:");

    lcd.setCursor(0, 1);
    lcd.print("NOME:");

    lcd.setCursor(0, 2);
    lcd.print("ENTER = CONFIRMAR");
}

void conexao() {

    String id = "esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando WiFi...");

    Serial.printf("Conectando na rede %s", SSID);

    WiFi.begin(SSID, SENHA);

    rgbLedWrite(48, 255, 0, 0);

    while (WiFi.status() != WL_CONNECTED) {

        Serial.print(".");

        delay(500);

        rgbLedWrite(48, 0, 0, 0);
        delay(500);

        rgbLedWrite(48, 255, 255, 0);
        delay(500);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi conectado!");

    Serial.printf("\nWiFi conectado com sucesso!\n");

    rgbLedWrite(48, 255, 255, 0);

    delay(1500);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando Broker:");
    lcd.setCursor(0, 1);
    lcd.print(BROKER);

    Serial.printf("Conectando ao Broker %s\n", BROKER);

    mqtt.setServer(BROKER, PORTA);
    mqtt.setCallback(aoReceber);

    bool conectadoAoBroker = mqtt.connect(id.c_str());

    if (conectadoAoBroker) {

        mqtt.subscribe(TOPICO_SUB);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Broker conectado!");

        lcd.setCursor(0, 1);
        lcd.print("MQTT conectado!");

        Serial.printf("Conectado ao broker %s com sucesso!\n", BROKER);
        Serial.printf("Inscrito no tópico: %s\n", TOPICO_SUB);

        rgbLedWrite(48, 0, 255, 0);

    }
    else {

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Falha no Broker!");

        Serial.printf("Falha ao conectar ao broker %s\n", BROKER);
        Serial.print("Falha: ");
        Serial.println(mqtt.state());

        rgbLedWrite(48, 255, 0, 0);

        delay(2000);
    }

    delay(1500);
}
