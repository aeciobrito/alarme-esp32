#include <WiFi.h>
#include <WebServer.h>

// --- CONFIGURAÇÕES DE HARDWARE ---
#define LED_PINO 12
#define TRIG_PIN 13
#define ECHO_PIN 14

// --- CONFIGURAÇÕES WI-FI ---
const char* ssid = "SEU_SSID"; // Nome do WiFi
const char* password = "SUA_SENHA"; // Em rede aberta, não usar

// Objeto do Servidor Web
WebServer server(80);

// --- VARIÁVEIS DO ALARME ---
bool alarmeDisparado = false;
unsigned long ultimoTempoPisca = 0;
unsigned long ultimoTempoBuzz = 0;
const long intervalo = 150;
int estadoLed = LOW;
int estadoBuzz = LOW;
const int DISTANCIA_LIMITE = 15; // cm

// --- PÁGINA WEB ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{font-family:Arial;text-align:center;margin-top:50px;}
input{padding:10px;font-size:20px;width:80%;max-width:300px;}
button{padding:10px 20px;font-size:20px;background-color:#005c9e;color:white;border:none;border-radius:5px;cursor:pointer;margin-top:10px;}
</style>
</head>
<body>
<h1>Sistema de Alarme SENAC</h1>
<p>Digite a senha para desativar:</p>
<form action="/desativar" method="GET">
<input type="text" name="senhaInput" placeholder="Senha...">

<button type="submit">DESATIVAR ALARME</button>
</form>
</body>
</html>
)rawliteral";

// =======================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --- Pinos ---
  pinMode(LED_PINO, OUTPUT);
  digitalWrite(LED_PINO, LOW);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // --- Wi-Fi STA ---
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(500);

  WiFi.begin(ssid, password); // Em rede aberta, não usar o 'password'

  unsigned long inicio = millis();
  const unsigned long timeout = 15000;

  while (WiFi.status() != WL_CONNECTED && millis() - inicio < timeout) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n Falha ao conectar no Wi-Fi");
    return;
  }

  Serial.println("\n Conectado com sucesso!");
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());

  // --- Servidor Web ---
  server.on("/", handleRoot);
  server.on("/desativar", handleForm); // Endpoint que o HTML chama
  server.begin();

  Serial.println("Servidor Web iniciado");
  Serial.println("Sistema armado. Limite: 50 cm");
}

void loop() {
  server.handleClient();

  if (!alarmeDisparado) {
    int distanciaAtual = lerDistancia();

    if (distanciaAtual > 0 && distanciaAtual < DISTANCIA_LIMITE) {
      Serial.print("ALERTA! Objeto a ");
      Serial.print(distanciaAtual);
      Serial.println(" cm.");
      alarmeDisparado = true;
    }

    delay(100);
  }

  if (alarmeDisparado) {
    piscaLed();
  }
}

// ================= ROTAS =================

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleForm() {
  if (server.hasArg("senhaInput")) {
    String senha = server.arg("senhaInput");
    senha.trim();
    senha.toUpperCase();

    if (senha == "SENAC") {
      server.send(200, "text/html",
        "<h1>SENHA CORRETA</h1><p>Sistema reiniciado.</p><a href='/'>Voltar</a>");
      resetarSistema();
    } else {
      server.send(200, "text/html",
        "<h1>SENHA INCORRETA</h1><a href='/'>Tentar novamente</a>");
    }
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

// ================= FUNÇÕES AUXILIARES =================

int lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) return -1;

  return duracao * 0.034 / 2;
}

void resetarSistema() {
  Serial.println("Reset via WEB...");
  alarmeDisparado = false;
  digitalWrite(LED_PINO, LOW);
  delay(3000);
  Serial.println("Sistema armado novamente.");
}

void piscaLed() {
  unsigned long agora = millis();
  if (agora - ultimoTempoPisca >= intervalo) {
    ultimoTempoPisca = agora;
    estadoLed = !estadoLed;
    digitalWrite(LED_PINO, estadoLed);
  }
}

void alarmeSonoro() {
  unsigned long agora = millis();
  if (agora - ultimoTempoBuzz >= intervalo) {
    ultimoTempoBuzz = agora;
    estadoBuzz = !estadoBuzz;
    digitalWrite(LED_PINO, estadoBuzz);
  }
}