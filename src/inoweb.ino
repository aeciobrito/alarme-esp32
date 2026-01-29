#include <WiFi.h>
#include <WebServer.h>

// --- CONFIGURAÇÕES DE HARDWARE ---
#define LED_PINO 12   // Seu LED manual (vermelho/azul)
#define TRIG_PIN 13   // Gatilho do Sensor
#define ECHO_PIN 14   // Eco do Sensor (com divisor de tensão)

// --- CONFIGURAÇÕES WI-FI ---
const char* ssid = "esp32senac";
const char* password = "123456789";

// Objeto do Servidor Web na porta 80
WebServer server(80);

// --- VARIÁVEIS DO ALARME ---
bool alarmeDisparado = false;
unsigned long ultimoTempoPisca = 0;
const long intervalo = 150;
int estadoLed = LOW;
const int DISTANCIA_LIMITE = 50; // cm

// --- PÁGINA WEB (HTML EM UMA ÚNICA STRING) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1"><style>body{font-family:Arial;text-align:center;margin-top:50px;}input{padding:10px;font-size:20px;width:80%;max-width:300px;}button{padding:10px 20px;font-size:20px;background-color:#005c9e;color:white;border:none;border-radius:5px;cursor:pointer;margin-top:10px;}.alerta{color:red;font-weight:bold;}</style></head><body><h1>Sistema de Alarme SENAC</h1><p>Digite a senha para desativar:</p><form action="/desativar" method="GET"><input type="text" name="senhaInput" placeholder="Senha..."><br><button type="submit">DESATIVAR ALARME</button></form></body></html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  // 1. Configuração de Pinos
  pinMode(LED_PINO, OUTPUT);
  digitalWrite(LED_PINO, LOW);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 2. Inicia o Ponto de Acesso (Access Point)
  Serial.println("Iniciando Access Point...");
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Endereço IP do AP: ");
  Serial.println(IP); // Geralmente 192.168.4.1

  // 3. Configura as rotas do Servidor Web
  server.on("/", handleRoot);          // Quando acessar a raiz
  server.on("/desativar", handleForm); // Quando clicar no botão
  server.begin();
  Serial.println("Servidor Web iniciado");

  Serial.println("Sistema Pronto. Barreira em 50cm.");
}

void loop() {
  // Mantém o servidor ouvindo requisições (CRÍTICO)
  server.handleClient();

  // 1. VIGILÂNCIA (Só roda se não estiver disparado)
  if(!alarmeDisparado) {
    int distanciaAtual = lerDistancia();
        
    if(distanciaAtual > 0 && distanciaAtual < DISTANCIA_LIMITE) {
      Serial.print("ALERTA! Objeto a ");
      Serial.print(distanciaAtual);
      Serial.println(" cm.");
      alarmeDisparado = true;
    }
    // Delay substituído por millis para não travar o servidor web
    delay(100); 
  }

  // 2. AÇÃO DE ALARME (Disparado)
  if(alarmeDisparado) {
    piscaLed(); // Pisca o LED
    // Nota: O "desativar" agora acontece via server.handleClient() lá em cima
  }
}

// --- FUNÇÕES DO SERVIDOR WEB ---

// 1. Serve a página HTML principal
void handleRoot() {
  server.send(200, "text/html", index_html);
}

// 2. Processa o formulário quando clica no botão
void handleForm() {
  if (server.hasArg("senhaInput")) { // Se veio algo no input "senhaInput"
    String senhaRecebida = server.arg("senhaInput");
    senhaRecebida.trim();
    senhaRecebida.toUpperCase();

    if (senhaRecebida == "SENAC") {
      server.send(200, "text/html", "<h1>SENHA CORRETA!</h1><p>Sistema reiniciando...</p><a href='/'>Voltar</a>");
      resetarSistema();
    } else {
      server.send(200, "text/html", "<h1>SENHA INCORRETA!</h1><a href='/'>Tente Novamente</a>");
    }
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

// --- FUNÇÕES AUXILIARES ---

int lerDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duracao = pulseIn(ECHO_PIN, HIGH);
  return duracao * 0.034 / 2;
}

void resetarSistema() {
  Serial.println("Reset via WEB iniciado...");
  alarmeDisparado = false;
  digitalWrite(LED_PINO, LOW);
  delay(3000); // Pausa para reinício
  Serial.println("Sistema Armado Novamente.");
}

void piscaLed() {
  unsigned long tempoAtual = millis();
  if(tempoAtual - ultimoTempoPisca >= intervalo) {
    ultimoTempoPisca = tempoAtual;
    estadoLed = !estadoLed;
    digitalWrite(LED_PINO, estadoLed);
  }
}
