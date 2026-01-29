#define LED_NATIVO 12 // LED embutido na placa (azul)
#define TRIG_PIN 13  // Pino de envio do sinal (Gatilho)
#define ECHO_PIN 14  // Pino de retorno (Eco)

bool alarmeDisparado = false;
unsigned long ultimoTempoPisca = 0;
const long intervalo = 150;
int estadoLed = LOW;

// Configuração da distância de disparo (em cm)
const int DISTANCIA_LIMITE = 10; 

void setup() {
  Serial.begin(115200);

  pinMode(LED_NATIVO, OUTPUT);
  digitalWrite(LED_NATIVO, LOW);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("--- ALARME SONAR (SEM BOTAO) ---");
  Serial.println("Aguarde estabilizacao...");
  delay(1000);
  Serial.println("Sistema Pronto. Digite 'SENAC' para desativar se disparar.");
}

void loop() {
  if(!alarmeDisparado) {
    int distanciaAtual = lerDistancia();
        
    if(distanciaAtual > 0 && distanciaAtual < DISTANCIA_LIMITE) {
      Serial.print("ALERTA! Objeto a ");
      Serial.print(distanciaAtual);
      Serial.println(" cm.");
      alarmeDisparado = true;
    }
    delay(100);
  }

  if(alarmeDisparado) {
    piscaLed(); 
    
    if(Serial.available() > 0) {
      String entrada = Serial.readStringUntil('\n');
      entrada.trim();       
      
      if (entrada == "SENAC") {
        resetarSistema();
      } else {
        Serial.println("SENHA INCORRETA");
      }
    }
  }
}

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
  Serial.println("SENHA CORRETA: Reiniciando...");
  alarmeDisparado = false;
  digitalWrite(LED_NATIVO, LOW);
  delay(3000); // Tempo menor de reinício
  Serial.println("Sistema Armado Novamente.");
}

void piscaLed() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempoPisca >= intervalo) {
    ultimoTempoPisca = tempoAtual;
    estadoLed = !estadoLed;
    digitalWrite(LED_NATIVO, estadoLed ? LOW : HIGH);
  }
}
