# Sistema de Alarme em ESP32
Sistema de Alarme desenvolvimento com a turma de IOT01 no Senac Lapa Tito

A seguir, as etapas do processo de desenvolvimento.


#### Código botão LED:
```C
#define BOTAO 12
#define LED_AZUL 4

bool ledLigado = false;
int estadoAnteriorBotao = LOW;

void setup() {
  Serial.begin(115200);

  pinMode(BOTAO, INPUT);
  pinMode(LED_AZUL, OUTPUT);
  digitalWrite(LED_AZUL, LOW); // COMEÇA DESLIGADO
}

void loop() {
  int estadoBotao = digitalRead(BOTAO);

  if(estadoAnteriorBotao == LOW && estadoBotao == HIGH) {
    ledLigado = !ledLigado; // INVERTE O BOOLEANO
    digitalWrite(LED_AZUL, ledLigado);

    Serial.print("Mudou o estado para: ");
    Serial.println(ledLigado ? "LIGADO" : "DELISGADO");
  }

  estadoAnteriorBotao = estadoBotao;

  delay(10); 
}
```
#### Diagrama Botão LED:
![Diagrama do Botão LED](https://raw.githubusercontent.com/aeciobrito/alarme-esp32/refs/heads/main/img/alarmeBotao.png)

---
