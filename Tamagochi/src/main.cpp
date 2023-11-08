#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "freertos/FreeRTOS.h" // Incluye el encabezado de FreeRTOS de esta manera


Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define BUTTON_01 4
#define BUTTON_02 18
#define BUTTON_03 19
#define BUTTON_04 23
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned int Hambre = 100;
unsigned int Sueno = 100;
unsigned int Felicidad = 100;
unsigned int Edad = 1;
int Salud = 100;

int b1=0;
int b2=0;
int b3=0;
int b4=0;

unsigned long lastUpdateTime = 0;
const int hambreUpdateInterval = 5000;  // Intervalo de actualización en milisegundos (5 segundos)

unsigned long lastAgeUpdateTime = 0;
const int edadUpdateInterval = 10000;  // Intervalo de actualización en milisegundos (10 segundos)

unsigned long lastUserInteractionTime = 0;
const int userInteractionInterval = 1000;  // Intervalo de espera en milisegundos (1 segundo)

const int OPCION_TAMAGOCHI = 1;
const int OPCION_JUEGO = 2;

void updateVariables() {
  mpu.getEvent(&a, &g, &temp);

  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= hambreUpdateInterval) {
    Hambre -= 1;
    lastUpdateTime = currentTime;
  }
  Sueno -= 1;
  Felicidad -= 1;
}

void updateSalud() {
  // Calcula la salud basada en las variables Hambre, Sueno y Felicidad
  Salud = (Hambre + Sueno + Felicidad) / 3;
}

void updateEdad() {
  // Incrementa la edad en 1 cada 10 segundos
  unsigned long currentTime = millis();
  if (currentTime - lastAgeUpdateTime >= edadUpdateInterval) {
    Edad++;
    lastAgeUpdateTime = currentTime;
  }
}

void comer() {
  Hambre += 20;
  Felicidad += 10;
}

void dormir() {
  Sueno += 30;
  Felicidad += 10;
}

void drawTamagotchi(int x, int y, int size, int mood) {
  // Dibuja el Tamagotchi en la posición (x, y) con el tamaño especificado
  display.drawCircle(x + size / 2, y + size / 2, size / 2, WHITE);
  // Dibuja los ojos
  display.drawCircle(x + size / 3, y + size / 3, size / 10, WHITE);
  display.drawCircle(x + 2 * size / 3, y + size / 3, size / 10, WHITE);
  // Dibuja la boca
  if (mood > 50) {
    display.fillRect(x + size / 3, y + 2 * size / 3, size / 3, size / 10, WHITE);
  } else {
    display.drawRect(x + size / 3, y + 2 * size / 3, size / 3, size / 10, WHITE);
  }
}
void imprimirMenu(int opcion, int opcionResaltada) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 0);
  display.println("Bienvenido");
  display.setCursor(5, 10);
  display.println("Elige una opcion:");

  for (int i = 0; i < 4; i++) {
    if (i + 1 == opcionResaltada) {
      display.setTextColor(BLACK, WHITE); // Cambiar el color del texto resaltado
    } else {
      display.setTextColor(WHITE);
    }

    switch (i + 1) {
      case 1:
        display.setCursor(10, 20 + i * 10);
        display.println("1. Tamagotchi");
        break;
      case 2:
        display.setCursor(10, 20 + i * 10);
        display.println("2. Patio de juegos");
        break;
      case 3:
        display.setCursor(10, 20 + i * 10);
        display.println("3. Tienda");
        break;
      case 4:
        display.setCursor(10, 20 + i * 10);
        display.println("4. Casa");
        break;
      default:
        break;
    }
  }

  display.display();
}

void tamagotchiLoop() {
  updateVariables();
  updateSalud();
  updateEdad();
  Serial.println("Presiona cualquier tecla para interactuar con el Tamagotchi.");
  while (!Serial.available()) {
    // Espera a que el usuario presione una tecla para interactuar con el Tamagotchi
  }
  Serial.read(); 


  // Dibuja el estado en la pantalla OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Salud: ");
  display.println(Salud);
  display.print("Hambre: ");
  display.println(Hambre);
  display.print("Sueño: ");
  display.println(Sueno);
  display.print("Felicidad: ");
  display.println(Felicidad);
  drawTamagotchi(0, 25, 40, Felicidad);
  display.display();

  // Interacción del usuario
  unsigned long currentTime = millis();
  if (currentTime - lastUserInteractionTime >= userInteractionInterval) {
    Serial.println("Elige una opcion:");
    Serial.println("1. Comer");
    Serial.println("2. Dormir");

    while (!Serial.available()) {
      // Espera a que el usuario ingrese una opción
    }

    int opcion = Serial.parseInt();
    if (opcion == 1) {
      comer();
    } else if (opcion == 2) {
      dormir();
    }

    lastUserInteractionTime = currentTime;
  }
}

void setup() {
  Serial.begin(9600);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  pinMode(BUTTON_01, INPUT);
  pinMode(BUTTON_02, INPUT);
  pinMode(BUTTON_03, INPUT);
  pinMode(BUTTON_04, INPUT);


}


int opcionActual = 1; // Inicializa la opción actual en 1

void loop() {
  b1 = digitalRead(BUTTON_01);
  b2 = digitalRead(BUTTON_02);
  b3 = digitalRead(BUTTON_03);
  b4 = digitalRead(BUTTON_04);

  if (b2) {
    opcionActual = (opcionActual > 1) ? opcionActual - 1 : 4;
  } else if (b3) {
    opcionActual = (opcionActual < 4) ? opcionActual + 1 : 1;
  }

  if (b1) {
    imprimirMenu(opcionActual, opcionActual);
  } else if (b2) {
    imprimirMenu(opcionActual, opcionActual);
  } else if (b3) {
    imprimirMenu(opcionActual, opcionActual);
  } else if (b4) {
    imprimirMenu(opcionActual, opcionActual);
  } else {
    imprimirMenu(0, opcionActual);
  }

  delay(250);
}
