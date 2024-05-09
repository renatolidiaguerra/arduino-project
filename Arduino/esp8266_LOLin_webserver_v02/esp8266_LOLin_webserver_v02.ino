#include <ESP8266WiFi.h>      // <WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h> // <WebServer.h>

const char* ssid = "warbrito";      // Nome da rede Wi-Fi
const char* password = "gb240820"; // Senha da rede Wi-Fi

ESP8266WebServer server(80);
// WebServer server(80);

String mensagem1 = "Mensagem 1";
String mensagem2 = "Mensagem 2";
int i = 0;


unsigned long previousMillis = 0;    // Armazena o valor do tempo da última atualização
const unsigned long interval = 5000; // Intervalo de atualização de 30 segundos (em milissegundos)

void handleRoot() {
  String content = "<html><head><meta http-equiv='refresh' content='4'/></head>\<body>";
  content += "<h1>Log Tomada Controladora v1.0</h1>";
  content += "<ul>";
  content += "<li>" + mensagem1 + "</li>";
  content += "<li>" + mensagem2 + "</li>";
  content += "</ul>";
  content += "</body></html>";

  server.send(200, "text/html", content);
}

void setup() {
  Serial.begin(115200);

  // Conectar-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado à rede Wi-Fi");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();

  i = i + 1;
  // Verificar se passou o intervalo de atualização
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Atualizar as mensagens aqui
    mensagem1 = "Nova Mensagem 1 alternativa";
    mensagem2 = "Nova Mensagem 2";

    // Salvar o tempo atual como a última atualização
    previousMillis = currentMillis;
  }
}