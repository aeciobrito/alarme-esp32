#define BOTAO 12
#define LED_AZUL 4
// O sensor ultrassônico usa 2 pinos
#define TRIG_PIN 13 
#define ECHO_PIN 14

bool alarmeDisparado = false;
unsigned long ultimoTempoPisca = 0;
const long intervalo = 150;
int estadoLed = LOW;

// Configuração da distância de disparo (em cm)
const int DISTANCIA_LIMITE = 50; 

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO, INPUT);
  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, LOW);
  
  // Configuração do Ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Sistema de Alarme (Ultrassonico) - Iniciado");
  Serial.println("Aguarde estabilizacao...");
  delay(1000);
  Serial.println("Sistema Pronto. Barreira definida em 50cm.");
}

void loop() {
  if(!alarmeDisparado) {
    int distanciaAtual = lerDistancia();
        
    if(distanciaAtual > 0 && distanciaAtual < DISTANCIA_LIMITE) {
      Serial.print("ALERTA: Objeto detectado a ");
      Serial.print(distanciaAtual);
      Serial.println(" cm!");
      alarmeDisparado = true;
    }
    // Delay para não sobrecarregar o sensor ultrassônico
    delay(100); 
  }

  if(alarmeDisparado) {
    piscaLed();
    verificarBotao();
    
    if(Serial.available() > 0) {
      String entrada = Serial.readStringUntil('\n');
      entrada.trim();
      entrada.toUpperCase();
      desativarPorSenha(entrada);
    }
  }
}

int lerDistancia() {
  // 1. Garante que o gatilho está desligado
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // 2. Envia um pulso de 10 microsegundos
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // 3. Lê o tempo que o som demorou para voltar (em microsegundos)
  long duracao = pulseIn(ECHO_PIN, HIGH);
  
  // 4. Calcula a distância (Velocidade do som: 0.034 cm/us)
  int distancia = duracao * 0.034 / 2;
  
  return distancia;
}

void desativarPorSenha(String senha) {
  if (senha == "SENAC") {
    Serial.println("SENHA CORRETA: Alarme desativado.");
    resetarSistema();
  } else {
    Serial.println("SENHA INCORRETA");
  }
}

void verificarBotao() {
  if(digitalRead(BOTAO) == HIGH) {
    Serial.println("Botão físico pressionado.");
    resetarSistema();
  }
}

void resetarSistema() {
  alarmeDisparado = false;
  digitalWrite(LED_AZUL, LOW);
  Serial.println("Reiniciando em 6 segundos...");
  delay(6000);
  Serial.println("Sistema Pronto (Barreira Ativa).");
}

void piscaLed() {
  unsigned long tempoAtual = millis();
  if(tempoAtual - ultimoTempoPisca >= intervalo) {
    ultimoTempoPisca = tempoAtual;
    estadoLed = !estadoLed;
    digitalWrite(LED_AZUL, estadoLed);
  }
}