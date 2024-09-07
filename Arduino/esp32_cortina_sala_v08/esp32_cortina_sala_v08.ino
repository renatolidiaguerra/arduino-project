
// DOIT ESP32 DEVKIT V1

#include <PubSubClient.h>
#include <WiFi.h>
#include <Stepper.h>

#include <iostream>
#include <cstring>

#include <Preferences.h>

Preferences preferences;

// Informações do WiFi
#define WIFISSID "warbrito"
#define WIFIPASS "gb240820"

#define SLEEP_TIME 10e6  // Tempo de sono em microsegundos (1 minuto)

// Informações do Servidor MQTT
const char* mqttServer = "192.168.15.10";
const int mqttPort = 1883;
const char* mqttUser = "renatobrito";
const char* mqttPass = "gb240820";
const char* topicControl   = "homeassistant/esp32/cortinasala/control";
const char* topicState     = "homeassistant/esp32/cortinasala/state";
const char* topicPosition  = "homeassistant/esp32/cortinasala/position";
const char* topicDebug     = "homeassistant/esp32/cortinasala/debug";


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

const unsigned long delay_resposta_mqtt = 59000;
unsigned long lastMessageTime = 0; 
unsigned long currentTime = 0;

char direcao_menos = 'd';
char direcao_mais  = 'u';

bool first_run = true;
int  contador_reboot = 0;

int cont_debug = 0;
int cont_debug_linha = 0;

int controle_marcopolo = 2;
int controle_loop = 0;

// Crie um objeto stepper
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4);

void setup() {
  Serial.begin(115200);

  Serial.println("Iniciando configuração...");
  Serial.println("Iniciando configuração...");
  Serial.println("Iniciando configuração...");

  // Configure o motor para uma velocidade média
  myStepper.setSpeed(velocidade); // Ajuste conforme necessário para a velocidade desejada 
  // 10: minimo
  // 12: maior torque
  // 15: menor torque
  
  // inicia memoria gravavel
  preferences.begin("memoria", false);
  // le memoria e atualiza controle de voltas atual
  controle_voltas = preferences.getUInt("counter", 0);
  Serial.print("memoria.controle_voltas: ");
  Serial.println(controle_voltas);
  // finaliza gravacao memoria
  // preferences.end();

  WiFi.begin(WIFISSID, WIFIPASS);

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  Serial.println("Connected to Wi-Fi");

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }

  Serial.print("memoria.controle_voltas: ");
  Serial.println(controle_voltas);

  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);

  if (!mqttClient.connected()) {
    reconnect();
  }

  mqttClient.subscribe(topicControl); // Subscrever ao tópico para receber comandos

  //Configura Sensores através do MQTT Discovery do Home Assistant
  String payloadConfig = "{\"name\": \"CortinaSala\", \"command_topic\": \"homeassistant/esp32/cortinasala/control\", \"state_topic\": \"homeassistant/esp32/cortinasala/state\"}";
  mqttClient.publish("homeassistant/esp32/cortinasala/config", payloadConfig.c_str(), true);


  // solicita posicao do controle 
  // mqttClient.publish(topicState, "@000");
  publish_topicPosition(controle_voltas);

}

void loop() {

  if (!mqttClient.connected()) {
    ESP.restart();
  }
  // Serial.print(".");
  mqttClient.loop(); // Manter a conexão MQTT ativa

  currentTime = millis(); 
  if (currentTime - lastMessageTime >= delay_resposta_mqtt) {

    controle_loop += 1;

    if (controle_loop > 100) {
      ESP.restart();
    } 

    mqttClient.publish(topicState, "online");

    publish_topicPosition(controle_voltas);

    lastMessageTime = currentTime; // Atualiza o tempo da última mensagem
    controle_marcopolo -= 1;
    // mqttClient.publish(topicDebug, "acao_marcopolo");
    if (controle_marcopolo <= 0) {
      ESP.restart();
      // mqttClient.publish(topicDebug, "acao_restart");
    }

  }
  // ====================
  //    debug
  // ====================
  // cont_debug += 1;
  // if (cont_debug > 60) {
  //   cont_debug_linha += 1;
  //   cont_debug = 0;
  //   Serial.print("%");
  // }
  // if (cont_debug_linha > 160) {
  //   cont_debug = 0;
  //   cont_debug_linha = 0;
  //   Serial.println("//");
  // }
  // =======================



  delay(100); // Pequeno atraso para evitar rebotes no botão
}

void reconnect() {
  Serial.println("tentando reconectar");
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

      contador_reboot += 1;
      Serial.println("fora do ar");
      if (contador_reboot > 12) {
        Serial.println("< ========================= > restartando < ========================= > ");
        ESP.restart();
      }
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
  //  9: comunicação servidor (991; 992; 993; 999) 


  int qtde_voltas = 0;

  // Converter os caracteres ASCII para números inteiros e compor o valor final
  for (int i = 1; i < 4; i++) {
      qtde_voltas = qtde_voltas * 10 + (payload[i] - '0');
  }

  // comunicacao com servidor
  if (qtde_voltas == 999) {
      Serial.println("< ========================= > restartando < ========================= > ");
      ESP.restart();
    return;
  }
  // comunicacao com servidor
  if (qtde_voltas == 991) {
    mqttClient.publish(topicState, "online");

    publish_topicPosition(controle_voltas);

    Serial.print("!!");
    return;
  }

  // if (qtde_voltas == 992) {
  //   if (first_run) {
  //     mqttClient.publish(topicState, "online: first_run");
  //   } else { 
  //     mqttClient.publish(topicState, "online");
  //   }
  //   publish_topicPosition(controle_voltas);
  //   return;
  // }

  //   marco polo
  if (qtde_voltas == 993) {
    controle_marcopolo = 6;
    Serial.print(".");
    mqttClient.publish(topicState, "done:x993");
    return;
  }

  // // recebe valor maximo via topico
  // if (qtde_voltas >= 800  && qtde_voltas <= 899) {
  //   controle_voltas_max = qtde_voltas - 800;
  //   mqttClient.publish(topicState, "done:x899");
  //   Serial.print("  -  controle_voltas_max:");
  //   Serial.print(controle_voltas_max);
  //   Serial.println("  -  Sending: +899");
  //   return;
  // }

  // recebe valor ATUAL! via topico
  if (qtde_voltas >= 700  && qtde_voltas <= 799) {
    controle_voltas = qtde_voltas - 700;

    preferences.putUInt("counter", controle_voltas);

    mqttClient.publish(topicState, "done:x799");
    publish_topicPosition(controle_voltas);

    Serial.print("  -  controle_voltas: ");
    Serial.print(controle_voltas);
    Serial.println("  -  Sending: +799");
    first_run = false;
    return;
  }

  // // recebe valor ATUAL! via topico
  // if (qtde_voltas >= 600  && qtde_voltas <= 699) {
  //   velocidade = qtde_voltas - 600;
  //   mqttClient.publish(topicState, "done:x699");
  //   Serial.print("  -  velocidade: ");
  //   Serial.print(velocidade);
  //   Serial.println("  -  Sending: +699");
  //   first_run = false;
  //   return;
  // }

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

  // if (first_run) {
  //   Serial.println("Necessita configurar posição atual!!");
  //   mqttClient.publish(topicState, "starting...");
  //   return;
  // }

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

      preferences.putUInt("counter", controle_voltas);

      Serial.print(controle_voltas);
    } else {
      if (controle_posicao < controle_voltas) {

        posicao_desejada = controle_voltas - controle_posicao;
        Serial.print("posicao desejada:");
        Serial.println(posicao_desejada);


        voltas((controle_voltas - controle_posicao), -1);
        controle_voltas = controle_posicao;

        preferences.putUInt("counter", controle_voltas);

        Serial.print(controle_voltas);
      } else {
        Serial.print("igual");
      }
    }
    mqttClient.publish(topicState, "done:x100");
    publish_topicPosition(controle_voltas);
    return;
  }

  // if (qtde_voltas >= 0 && qtde_voltas <= 99) {
  //   if (payload[0] == direcao_menos) {
  //     if (qtde_voltas > controle_voltas) {
  //       voltas(controle_voltas, -1);  
  //       controle_voltas = 0;

  //       preferences.putUInt("counter", controle_voltas);

  //       Serial.println(" !!! <");
  //     } else {
  //       controle_voltas -= qtde_voltas;

  //       preferences.putUInt("counter", controle_voltas);

  //       voltas(qtde_voltas, -1);
  //     }
  //   } else {
  //     if ((controle_voltas + qtde_voltas) > controle_voltas_max) {
  //       voltas(controle_voltas_max, +1);  
  //       controle_voltas = controle_voltas_max;

  //       preferences.putUInt("counter", controle_voltas);

  //       Serial.println(" !!! >");
  //     } else {
  //       controle_voltas += qtde_voltas;

  //       preferences.putUInt("counter", controle_voltas);

  //       voltas(qtde_voltas, +1);
  //     }
  //   }
  //   mqttClient.publish(topicState, "done:x000");
  //   return;
  // }

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

  int aux_posicao = controle_voltas;

  Serial.print("inicial aux_posicao:");
  Serial.println(aux_posicao);
  Serial.print(" ->");

  for (i = 0; i < qtde; i++) {
    myStepper.step(stepsPerRevolution * direcao);

    if (direcao > 0) {
      // soma
      aux_posicao++;
    } else {
      // subtrai
      aux_posicao--;
    }

    // grava posicao temporaria em memoria
    preferences.putUInt("counter", aux_posicao);
    // Serial.print("...");
    // Serial.print(aux_posicao);
    // publish_topicDebug(aux_posicao);
  
    delay(delay_motor);
    mqttClient.loop();
  }
  // desliga motor
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,LOW);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);

  Serial.print("[ok]");

  Serial.print("final   aux_posicao:");
  Serial.println(aux_posicao);

}

void publish_topicPosition(int valor)
{
  Serial.print("Gravando posicao:");
  Serial.println(valor);
  char buffer[10];
  itoa(valor, buffer, 10);
  mqttClient.publish(topicPosition, buffer);
}

void publish_topicDebug(int valor)
{
  char buffer[10];
  itoa(valor, buffer, 10);
  mqttClient.publish(topicDebug, buffer);
}