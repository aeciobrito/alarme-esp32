#define BOTAO 12
#define LED_AZUL 4
#define PIR 13

bool alarmeDisparado = false;

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO, INPUT);
  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, LOW); // COMEÇA DESLIGADO
  pinMode(PIR, INPUT);

  Serial.println("Sistema de Alarme com Reset por Botao - Iniciado");
  Serial.println("Aguarde a estabilização do sensor PIR...");
  delay(5000);
  Serial.println("Sistema Pronto para Uso.");
}

void loop() {
  // Se o alarme não estiver disparado, verifica se há movimento
  if(!alarmeDisparado) {
    int estadoPir = digitalRead(PIR);
    if(estadoPir == HIGH) { // tendo movimento
      Serial.println("ALERTA: Movimento Detectado!");
      alarmeDisparado = true;
    }
  }

  // Se o alarme estiver disparado, fica piscando e verifica botão de reset
  if(alarmeDisparado) {
    // 1. Ação do alarme: Piscar o LED
    piscaLed();

    // 2. Verifica o botão 
    int estadoBotao = digitalRead(BOTAO);
    if(estadoBotao == HIGH) {
      Serial.print("Alarme interrompido pelo usuário");
      alarmeDisparado = false;
      digitalWrite(LED_AZUL, LOW);
      delay(1000);
    }
  }

  delay(10); 
}

void piscaLed() {
  digitalWrite(LED_AZUL, HIGH);
  delay(150);
  digitalWrite(LED_AZUL, LOW);
  delay(150);
}