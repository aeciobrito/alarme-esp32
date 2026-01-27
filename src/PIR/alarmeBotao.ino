#define BOTAO 12
#define LED_AZUL 4
#define PIR 13

bool alarmeDisparado = false;
unsigned long ultimoTempoPisca = 0; // "unsigned" é melhor para millis()
const long intervalo = 150; 
int estadoLed = LOW; 

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO, INPUT);
  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, LOW); 
  pinMode(PIR, INPUT);

  Serial.println("Sistema de Alarme - Iniciado");
  Serial.println("Digite 'SENAC' no monitor serial para desativar quando disparar.");
  Serial.println("Aguarde a estabilização do sensor PIR...");
  delay(5000);
  Serial.println("Sistema Pronto para Uso.");
}

void loop() {
  // PARTE 1: Monitoramento (Só se não estiver disparado)
  if(!alarmeDisparado) {
    int estadoPir = digitalRead(PIR);
    if(estadoPir == HIGH) { 
      Serial.println("ALERTA: Movimento Detectado!");
      alarmeDisparado = true;
    }
  }

  // PARTE 2: Ações quando disparado
  if(alarmeDisparado) {
    piscaLed();       // Pisca sem travar (millis)
    verificarBotao(); // Verifica botão físico
    
    // Nova função: Verifica o monitor serial
    if(Serial.available() > 0) {
      String entrada = Serial.readStringUntil('\n');
      entrada.trim();         // Remove espaços e quebras de linha (\n)
      entrada.toUpperCase();  // Transforma tudo em maiúsculo para facilitar
      
      desativarPorSenha(entrada);
    }
  }
}

// --- FUNÇÃO SOLICITADA ---
void desativarPorSenha(String senha) {
  // Verifica se a senha é exatamente "SENAC"
  if (senha == "SENAC") {
    Serial.println("SENHA CORRETA: Alarme desativado via Serial.");
    alarmeDisparado = false;
    digitalWrite(LED_AZUL, LOW);
    
    // Pausa dramática para reinício (igual ao botão)
    Serial.println("Aguarde 6 segundos para o retorno do sistema.");
    delay(6000); 
    Serial.println("Sistema em operação novamente.");
  } else {
    Serial.println("SENHA INCORRETA");
  }
}

void verificarBotao() {
  int estadoBotao = digitalRead(BOTAO);
  if(estadoBotao == HIGH) {
    Serial.println("Alarme interrompido pelo botão físico");
    alarmeDisparado = false;
    digitalWrite(LED_AZUL, LOW);
    Serial.println("Aguarde 6 segundos para o retorno do sistema.");
    delay(6000);
    Serial.println("Sistema em operação novamente.");
  }
}

void piscaLed() {
  unsigned long tempoAtual = millis();
  if(tempoAtual - ultimoTempoPisca >= intervalo) {
    ultimoTempoPisca = tempoAtual;
    
    // Inversão simples do estado (toggle)
    estadoLed = !estadoLed; 
    digitalWrite(LED_AZUL, estadoLed);
  }
}