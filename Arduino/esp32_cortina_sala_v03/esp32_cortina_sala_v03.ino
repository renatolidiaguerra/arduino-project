#include <PubSubClient.h>
#include <WiFi.h>
#include <Stepper.h>

#include <iostream>
#include <cstring>

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";
const char* topicControl = "homeassistant/esp32/cortinasala/control";
const char* topicState   = "homeassistant/esp32/cortinasala/state";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Defina os pinos para o controle do motor de passo
const int stepsPerRevolution = 2048; // O motor de passo 28BYJ-48 tem 2048 passos por revolução

const int motorPin1 = 13;    // Pino IN1 conectado ao D5
const int motorPin2 = 12;    // Pino IN2 conectado ao D6
const int motorPin3 = 14;    // Pino IN3 conectado ao D7
const int motorPin4 = 27;    // Pino IN4 conectado ao D8

int qtde = 4;
int i = 0;
int valor = 2048;
int controle = 0;
int velocidade = 12;
int delay_motor = 5;

int controle_voltas = 0;
int controle_voltas_max = 20;
int controle_posicao = 0;

int delay_resposta_mqtt = 25000;

char direcao_menos = 'd';
char direcao_mais  = 'u';

bool first_run = true;

// Crie um objeto stepper
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  Serial.begin(115200);

  // Configure o motor para uma velocidade média
  myStepper.setSpeed(velocidade); // Ajuste conforme necessário para a velocidade desejada 
  // 10: minimo
  // 12: maior torque
  // 15: menor torque
  
  WiFi.begin(WIFISSID, WIFIPASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mqttClient.setServer(mqttServer, mqttPort);

  mqttClient.setCallback(callback);

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.subscribe(topicControl); // Subscrever ao tópico para receber comandos

  //Configura Sensores através do MQTT Discovery do Home Assistant
  String payloadConfig = "{\"name\": \"CortinaSala\", \"command_topic\": \"homeassistant/esp32/cortinasala/control\", \"state_topic\": \"homeassistant/esp32/cortinasala/state\"}";
  mqttClient.publish("homeassistant/esp32/cortinasala/config", payloadConfig.c_str(), true);

}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  // Serial.print(".");
  mqttClient.loop(); // Manter a conexão MQTT ativa

  delay(100); // Pequeno atraso para evitar rebotes no botão
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("CortinaSala", mqttUser, mqttPass)) {
      Serial.println("Connected to MQTT");
      mqttClient.publish(topicState, "online");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  //	0: mover somando parametro
  //	1: mover para posicao
	//	2: mover 1 posicao para ajuste fino (+200/-200)
	//	3:
	//	4:
	//	5:
  //	6: velocidade
  // 	7: seta valor atual / posicao atual
  //  8: valor maximo
  //  9: comunicação servidor


  int qtde_voltas = 0;

  // Converter os caracteres ASCII para números inteiros e compor o valor final
  for (int i = 1; i < 4; i++) {
      qtde_voltas = qtde_voltas * 10 + (payload[i] - '0');
  }

  // comunicacao com servidor
  if (qtde_voltas == 991) {
    mqttClient.publish(topicState, "online");
    Serial.print("%");
    return;
  }

  if (qtde_voltas == 992) {
    if (first_run) {
      mqttClient.publish(topicState, "online: first_run");
    } else { 
      mqttClient.publish(topicState, "online");
    }
    return;
  }

  // recebe valor maximo via topico
  if (qtde_voltas >= 800  && qtde_voltas <= 899) {
    controle_voltas_max = qtde_voltas - 800;
    mqttClient.publish(topicState, "done:x899");
    Serial.print("  -  controle_voltas_max:");
    Serial.print(controle_voltas_max);
    Serial.println("  -  Sending: +899");
    return;
  }

  // recebe valor ATUAL! via topico
  if (qtde_voltas >= 700  && qtde_voltas <= 799) {
    controle_voltas = qtde_voltas - 700;
    mqttClient.publish(topicState, "done:x799");
    Serial.print("  -  controle_voltas: ");
    Serial.print(controle_voltas);
    Serial.println("  -  Sending: +799");
    first_run = false;
    return;
  }

  // recebe valor ATUAL! via topico
  if (qtde_voltas >= 600  && qtde_voltas <= 699) {
    velocidade = qtde_voltas - 600;
    mqttClient.publish(topicState, "done:x699");
    Serial.print("  -  velocidade: ");
    Serial.print(velocidade);
    Serial.println("  -  Sending: +699");
    first_run = false;
    return;
  }

  // ajuste fino via 1 volta (+200 / -200)
  if (qtde_voltas == 200) {
    if (payload[0] == direcao_menos) {
      voltas(1, -1);
    } else {
      voltas(1, +1);
    }
    mqttClient.publish(topicState, "done:x200");
    Serial.println("...ajustando...");
    return;
  }

  if (first_run) {
    Serial.println("Necessita configurar posição atual!!");
    mqttClient.publish(topicState, "error:@001");
    return;
  }

  // recebe valor POSICIONAL via topico
  if (qtde_voltas >= 100 && qtde_voltas <= 199) {
    controle_posicao = qtde_voltas - 100;
    Serial.print("controle_posicao:");
    Serial.println(controle_posicao);

    int posicao_desejada = 0;

    if (controle_posicao > controle_voltas) {
      if (controle_posicao > controle_voltas_max) {
        Serial.println("excesso");
        return;
      }
      posicao_desejada = controle_posicao - controle_voltas;
      Serial.print("posicao desejada:");
      Serial.println(posicao_desejada);

      voltas(posicao_desejada, +1);
      controle_voltas = controle_posicao;
      Serial.print(controle_voltas);
    } else {
      if (controle_posicao < controle_voltas) {

        posicao_desejada = controle_voltas - controle_posicao;
        Serial.print("posicao desejada:");
        Serial.println(posicao_desejada);


        voltas((controle_voltas - controle_posicao), -1);
        controle_voltas = controle_posicao;
        Serial.print(controle_voltas);
      } else {
        Serial.print("igual");
      }
    }
    mqttClient.publish(topicState, "done:x100");
    return;
  }

  if (qtde_voltas >= 0 && qtde_voltas <= 99) {
    if (payload[0] == direcao_menos) {
      if (qtde_voltas > controle_voltas) {
        voltas(controle_voltas, -1);  
        controle_voltas = 0;
        Serial.println(" !!! <");
      } else {
        controle_voltas -= qtde_voltas;
        voltas(qtde_voltas, -1);
      }
    } else {
      if ((controle_voltas + qtde_voltas) > controle_voltas_max) {
        voltas(controle_voltas_max, +1);  
        controle_voltas = controle_voltas_max;
        Serial.println(" !!! >");
      } else {
        controle_voltas += qtde_voltas;
        voltas(qtde_voltas, +1);
      }
    }
    mqttClient.publish(topicState, "done:x000");
    return;
  }

  Serial.println("Parametro não reconhecido!");

}

void voltas(int qtde, int direcao)  // 1 = direita; -1 = esquerda
{
  int i = 0;
  int sentido = -1;

  // liga motor
  Serial.print(" ... motor ");
  
  digitalWrite(motorPin1,HIGH);
  digitalWrite(motorPin2,HIGH);
  digitalWrite(motorPin3,HIGH);
  digitalWrite(motorPin4,HIGH);

  for (i = 0; i < qtde; i++) {
    myStepper.step(stepsPerRevolution * direcao);
    delay(delay_motor);
    mqttClient.loop();
  }
  // desliga motor
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,LOW);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);

  Serial.print("[ok]");
}