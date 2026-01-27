# Sistema de Alarme em ESP32
Sistema de Alarme desenvolvimento com a turma de IOT01 no Senac Lapa Tito

A seguir, as etapas do processo de desenvolvimento.


#### Código botão LED:
```C
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
```

#### Diagrama Como Código:
```JSON
{
  "version": 1,
  "author": "Anonymous maker",
  "editor": "wokwi",
  "parts": [
    { "type": "board-esp32-devkit-c-v4", "id": "esp", "top": 0, "left": 0, "attrs": {} },
    {
      "type": "wokwi-led",
      "id": "led1",
      "top": 18,
      "left": 121.8,
      "rotate": 90,
      "attrs": { "color": "red" }
    },
    {
      "type": "wokwi-pushbutton",
      "id": "btn1",
      "top": 54.2,
      "left": -76.8,
      "attrs": { "color": "green", "xray": "1" }
    },
    {
      "type": "wokwi-resistor",
      "id": "r1",
      "top": 138.35,
      "left": -76.8,
      "attrs": { "value": "1000" }
    },
    {
      "type": "wokwi-resistor",
      "id": "r2",
      "top": 89.8,
      "left": 95.15,
      "rotate": 270,
      "attrs": { "value": "1000" }
    },
    { "type": "wokwi-pir-motion-sensor", "id": "pir1", "top": 71.2, "left": -170.58, "attrs": {} }
  ],
  "connections": [
    [ "esp:TX", "$serialMonitor:RX", "", [] ],
    [ "esp:RX", "$serialMonitor:TX", "", [] ],
    [ "r2:1", "esp:4", "green", [ "v0" ] ],
    [ "r2:2", "led1:A", "green", [ "v-19.2", "h-39.6" ] ],
    [ "esp:GND.2", "led1:C", "black", [ "v0", "h33.64" ] ],
    [ "esp:3V3", "btn1:1.r", "red", [ "h-23.81" ] ],
    [ "btn1:2.r", "esp:12", "green", [ "h0.2", "v57.8" ] ],
    [ "esp:12", "r1:2", "limegreen", [ "v0" ] ],
    [ "r1:1", "esp:GND.1", "black", [ "v9.6", "h9.6" ] ],
    [ "pir1:GND", "esp:GND.1", "black", [ "v19.2", "h95.74", "v-9.6" ] ],
    [ "pir1:OUT", "esp:13", "green", [ "v28.8", "h115.06", "v-9.6" ] ],
    [ "esp:5V", "pir1:VCC", "red", [ "h-148.61" ] ]
  ],
  "dependencies": {}
}
```

#### Diagrama Botão LED:
![Diagrama do Botão LED](https://raw.githubusercontent.com/aeciobrito/alarme-esp32/refs/heads/main/img/alarmeBotao.png)

---
