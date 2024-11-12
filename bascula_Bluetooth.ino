#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); // RX, TX

unsigned char ReadMulti[10] = {0XAA, 0X00, 0X27, 0X00, 0X03, 0X22, 0XFF, 0XFF, 0X4A, 0XDD};
unsigned int timeSec = 0;
unsigned int timemin = 0;
unsigned int dataAdd = 0;
unsigned int incomedate = 0;
unsigned int parState = 0;
unsigned int codeState = 0;
unsigned int lastIncomedate = 0; // Variable para almacenar el último valor enviado

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hola mundo.");
  Serial.write(ReadMulti, 10);
  BTSerial.begin(115200); // Configuración del HC-05 (verifica el baud rate de tu módulo)
}

void loop() {
  timeSec++;
  if (timeSec >= 50000) {
    timemin++;
    timeSec = 0;
    if (timemin >= 20) {
      timemin = 0;
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.write(ReadMulti, 10);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  if (Serial.available() > 0) {
    incomedate = Serial.read();
    if ((incomedate == 0x02) && (parState == 0)) {
      parState = 1;
    } else if ((parState == 1) && (incomedate == 0x22) && (codeState == 0)) {
      codeState = 1;
      dataAdd = 3;
    } else if (codeState == 1) {
      dataAdd++;
      if (dataAdd == 6) {
        Serial.print("RSSI: ");
        Serial.println(incomedate);
      } else if ((dataAdd == 7) || (dataAdd == 8)) {
        if (dataAdd == 7) {
          Serial.print("PC: ");
          Serial.print(incomedate);
        } else {
          Serial.println(incomedate);
        }
      } else if ((dataAdd >= 9) && (dataAdd <= 20)) {
        if (dataAdd == 9) {
          Serial.print("EPC: ");
        }
        Serial.print(incomedate);
        enviar_bt(incomedate); // Enviar `incomedate` por Bluetooth en formato hexadecimal
      } else if (dataAdd >= 21) {
        Serial.println();
        dataAdd = 0;
        parState = 0;
        codeState = 0;
      }
    } else {
      dataAdd = 0;
      parState = 0;
      codeState = 0;
    }
  }
}



void enviar_bt(unsigned int incomedate) {
  // Verificar si el valor es diferente al anterior
  if (incomedate != lastIncomedate) {
    // Convertir incomedate a cadena hexadecimal antes de enviar
    //char hexString[3]; // Espacio para 2 caracteres hexadecimales + terminador null
    //snprintf(hexString, sizeof(hexString), "%02X", incomedate);
    
    // Enviar el valor convertido como texto al Bluetooth
    BTSerial.print(incomedate);
    
    // Actualizar el último valor enviado
    lastIncomedate = incomedate;
    
    // Imprimir en el monitor serie para verificar
 //   Serial.print("Enviando por Bluetooth: ");
//    Serial.println(hexString);
  }
}
