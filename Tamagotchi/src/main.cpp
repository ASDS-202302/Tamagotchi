#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GFX.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define BUTTON_01 4
#define BUTTON_02 18
#define BUTTON_03 19
#define BUTTON_04 23
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU6050 sensor;

// BITMAPS GENERADOS CON: https://javl.github.io/image2cpp/
// simbolo de vida
const unsigned char health[] PROGMEM = {
    0x00, 0x00, 0x79, 0xe0, 0x7f, 0xe0, 0x7f, 0xe0, 0x7f, 0xe0, 0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80,
    0x0f, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00};
// simbolo de hambre
const unsigned char hunger[] PROGMEM = {
    0x00, 0x70, 0x00, 0x30, 0x00, 0xc0, 0x3f, 0x80, 0x7f, 0x00, 0x7e, 0x00, 0xfe, 0x00, 0xb4, 0x00,
    0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// simbolo de limpio
const unsigned char clean[] PROGMEM = {
    0x00, 0x00, 0x02, 0x00, 0x04, 0x20, 0x24, 0x60, 0x4c, 0xc0, 0x44, 0x80, 0x62, 0x40, 0x33, 0x60,
    0x12, 0x20, 0x32, 0x20, 0x64, 0x40, 0x40, 0x00};
// simbolo de jugar
const unsigned char play[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x19, 0x80, 0x3f, 0xc0, 0x57, 0x60, 0x56, 0xa0, 0x7f, 0x60, 0xff, 0xf0,
    0xf0, 0xf0, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00};
// simbolo de dormir
const unsigned char sleeping[] PROGMEM = {
    0x00, 0x60, 0x08, 0x20, 0x38, 0x70, 0x71, 0x00, 0x71, 0x00, 0xf0, 0x00, 0xf0, 0x40, 0xf8, 0x00,
    0x7c, 0x00, 0x7f, 0xc0, 0x3f, 0x80, 0x1f, 0x00};
// simbolo de edad
const unsigned char age[] PROGMEM = {
    0x00, 0x00, 0x0f, 0x00, 0x1f, 0x80, 0x1f, 0x80, 0x0f, 0x00, 0x0f, 0x00, 0x10, 0x80, 0x3f, 0xc0,
    0x3f, 0xc0, 0x7f, 0xe0, 0x3f, 0xc0, 0x06, 0x00};
// boton comer
const unsigned char eatButton[] PROGMEM = {
    0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
    0xf0, 0x40, 0x31, 0xfc, 0x40, 0x33, 0xfe, 0x40, 0x37, 0xfe, 0x40, 0x17, 0xff, 0x40, 0x17, 0xff,
    0x40, 0x17, 0xff, 0x40, 0x17, 0xfe, 0x40, 0x13, 0xfe, 0x40, 0x13, 0xfc, 0x40, 0x10, 0xf8, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x10};
// boton dormir
const unsigned char sleepButton[] PROGMEM = {
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x60, 0x00, 0x02, 0x60, 0x00, 0x00, 0xe3, 0x00, 0x02, 0xe9, 0x00, 0x0c, 0xe0,
    0x00, 0x00, 0xe0, 0x00, 0x02, 0x72, 0x00, 0x00, 0x38, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x10};
// boton jugar
const unsigned char playButton[] PROGMEM = {
    0x80, 0x00, 0x30, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
    0xfe, 0x00, 0x3f, 0xff, 0x80, 0x3b, 0xfa, 0xc0, 0x61, 0xfa, 0xc0, 0x61, 0xfa, 0xc0, 0x3b, 0xfa,
    0xc0, 0x3f, 0xff, 0x80, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x10};
// boton limpiar
const unsigned char cleanButton[] PROGMEM = {
    0xc0, 0x00, 0x10, 0x80, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x03,
    0xf4, 0x00, 0x07, 0xf8, 0x00, 0x07, 0xe8, 0x00, 0x07, 0xd8, 0x00, 0x07, 0xf8, 0x00, 0x03, 0xf0,
    0x00, 0x03, 0xf0, 0x00, 0x03, 0xf0, 0x00, 0x07, 0xf8, 0x00, 0x07, 0xf8, 0x00, 0x07, 0xf8, 0x00,
    0x07, 0xf8, 0x00, 0x03, 0xf0, 0x00, 0x80, 0x00, 0x10, 0xc0, 0x00, 0x30};
// boton muerte
const unsigned char deathButton[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x01, 0xf8, 0x00, 0x01, 0xfc, 0x00, 0x03,
    0xfc, 0x00, 0x03, 0xfc, 0x00, 0x03, 0x6c, 0x00, 0x03, 0x6c, 0x00, 0x0d, 0xfb, 0x00, 0x0e, 0xf7,
    0x00, 0x0f, 0x1f, 0x00, 0x0f, 0xfc, 0x00, 0x01, 0xf8, 0x00, 0x0f, 0xff, 0x00, 0x0f, 0x0f, 0x00,
    0x0c, 0x03, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// bitmap de estado de animo neutral
const unsigned char neutral_mood[] PROGMEM = {
    0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xcc,
    0x00, 0x00, 0x00, 0x00, 0x9c, 0xfc, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x7c, 0x00, 0x00, 0x00, 0x07,
    0xe0, 0x66, 0x00, 0x00, 0x00, 0x0e, 0x40, 0x46, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x03, 0xe0, 0x00,
    0x01, 0xf8, 0x00, 0x03, 0xfc, 0x00, 0x07, 0xf8, 0x00, 0x01, 0xff, 0x00, 0x0f, 0xf0, 0x00, 0x01,
    0xff, 0x80, 0x1f, 0xf0, 0x00, 0x00, 0xff, 0xc0, 0x3f, 0xe0, 0x00, 0x00, 0x7f, 0xc0, 0x3f, 0xe0,
    0x00, 0x00, 0x7f, 0xe0, 0x7f, 0xc0, 0x00, 0x00, 0x3f, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x1f, 0xf0,
    0x7e, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x7e, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x7e, 0x00, 0x00, 0x00,
    0x07, 0xf0, 0x7e, 0x0c, 0x00, 0x03, 0x83, 0xf0, 0x7c, 0x3f, 0x00, 0x0f, 0xc1, 0xf0, 0x78, 0x7f,
    0x80, 0x0f, 0xe1, 0xf0, 0x78, 0x7f, 0x80, 0x1f, 0xe1, 0xe0, 0x30, 0x7f, 0x80, 0x1f, 0xe0, 0xe0,
    0x30, 0x7f, 0x80, 0x0f, 0xe0, 0xe0, 0x3c, 0x3f, 0x00, 0x0f, 0xc3, 0xc0, 0x38, 0x1e, 0x00, 0x07,
    0x81, 0x80, 0x18, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x07,
    0xff, 0xff, 0x01, 0x80, 0x18, 0x0f, 0xff, 0xff, 0x01, 0x80, 0x18, 0x00, 0x00, 0x00, 0x01, 0x80,
    0x1d, 0xf0, 0x00, 0x00, 0x7d, 0x80, 0x1f, 0xf8, 0x00, 0x01, 0xff, 0x80, 0x3f, 0xfc, 0x00, 0x03,
    0xff, 0xc0, 0x3f, 0xfc, 0x00, 0x03, 0xff, 0xc0, 0x3f, 0xfe, 0x00, 0x03, 0xff, 0xc0, 0x1f, 0xfc,
    0x00, 0x03, 0xff, 0xc0, 0x1f, 0xff, 0xc0, 0x0f, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xff, 0xff, 0x00,
    0x01, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Status variables
unsigned int limpieza;
unsigned int hambre;
unsigned int aburrimiento;
unsigned int sueno;
unsigned int salud;
unsigned int edad;

double k;                     // Factor de crecimiento
int option;                   // Opcion escogida
bool left, right, enter, esc; // Botones

// Variables de MPU6050
int16_t ax, ay, az;
int16_t gx, gy, gz;

// Prototipos de funciones para tareas
void vUITask(void *pvParameters);          // Interfaz grafica
void vUserInputTask(void *pvParameters);   // Recibir el input del usuario
void vStateUpdateTask(void *pvParameters); // Actualizar variables de estado
void vAgeTask(void *pvParameters);         // Envejecer
void vFeedingTask(void *pvParameters);     // Alimentar
void vGameTask(void *pvParameters);        // Jugar
void vSleepTask(void *pvParameters);       // Dormir
void vCleanTask(void *pvParameters);       // Limpiar

// Definición de colas y semáforos
QueueHandle_t xUserInputQueue; // Cola con las entradas de los botones
SemaphoreHandle_t xDataMutex;  // Mutex de los datos de estado

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sensor.initialize();
  pinMode(BUTTON_01, INPUT);
  pinMode(BUTTON_02, INPUT);
  pinMode(BUTTON_03, INPUT);
  pinMode(BUTTON_04, INPUT);
  left = digitalRead(BUTTON_01);
  right = digitalRead(BUTTON_02);
  enter = digitalRead(BUTTON_03);
  esc = digitalRead(BUTTON_04);

  limpieza = 100;
  hambre = 100;
  sueno = 100;
  salud = 100;
  aburrimiento = 100;

  option = 0;
  edad = 0;
  k = 0.2;//0.05;

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  if (sensor.testConnection())
    Serial.println("Sensor iniciado correctamente");
  else
    Serial.println("Error al iniciar el sensor");

  // Crear colas y semáforos
  xUserInputQueue = xQueueCreate(10, sizeof(uint8_t));
  xDataMutex = xSemaphoreCreateMutex();

  // Crear tareas
  xTaskCreate(vUITask, "UI Task", 32000, NULL, 1, NULL);
  xTaskCreate(vUserInputTask, "User Input Task", 1000, NULL, 1, NULL);
  xTaskCreate(vStateUpdateTask, "State Update Task", 1000, NULL, 2, NULL);
  xTaskCreate(vAgeTask, "Feeding Task", 1000, NULL, 2, NULL);
  // xTaskCreate(vFeedingTask, "Feeding Task", 1000, NULL, 2, NULL);
  // xTaskCreate(vGameTask, "Game Task", 2000, NULL, 2, NULL);
  // xTaskCreate(vSleepTask, "Sleep Task", 1000, NULL, 2, NULL);
  // xTaskCreate(vCleanTask, "Clean Task", 1000, NULL, 2, NULL);
}

void loop()
{
  // No hacer nada
}

uint8_t getUserInput()
{
  left = digitalRead(BUTTON_01);
  right = digitalRead(BUTTON_02);
  enter = digitalRead(BUTTON_03);
  esc = digitalRead(BUTTON_04);
  if (left == 1)
  {
    return 1;
  }
  else if (right == 1)
  {
    return 2;
  }
  else if (enter == 1)
  {
    return 3;
  }
  else if (esc == 1)
  {
    return 4;
  }
  return 0;
}

void vUITask(void *pvParameters)
{
  while (1)
  {
    uint32_t rxElement = 0;
    if (xQueueReceive(xUserInputQueue, (void *)&rxElement, (TickType_t)10))
    {
      Serial.println("Elemento recibido.");
      switch (rxElement)
      {
      case 1:
        option = ((option - 1) % 5);
        if (option < 0)
        {
          option += 5;
        }
        break;
      case 2:
        option = ((option + 1) % 5);
        break;
      case 3:
        break;
      case 4:
        break;
      }
      Serial.println(option);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    // Simbolo mascota
    display.drawBitmap(0, 0, neutral_mood, 45, 45, WHITE);
    // Simbolos de estado
    display.drawBitmap(55, 0, clean, 12, 12, WHITE);
    display.setCursor(69, 3); // x+14, y+3
    display.print(limpieza);
    display.drawBitmap(90, 0, hunger, 12, 12, WHITE);
    display.setCursor(104, 3);
    display.print(hambre);
    display.drawBitmap(55, 15, sleeping, 12, 12, WHITE);
    display.setCursor(69, 18);
    display.print(sueno);
    display.drawBitmap(90, 15, play, 12, 12, WHITE);
    display.setCursor(104, 18);
    display.print(aburrimiento);
    display.drawBitmap(55, 30, health, 12, 12, WHITE);
    display.setCursor(69, 33);
    display.print(salud);
    display.drawBitmap(90, 30, age, 12, 12, WHITE);
    display.setCursor(104, 33);
    display.print(edad);
    // Simbolos botones
    switch (option)
    {
    case 0:
      display.drawBitmap(0, 44, eatButton, 20, 20, BLACK, WHITE);
      display.drawBitmap(25, 44, cleanButton, 20, 20, WHITE);
      display.drawBitmap(50, 44, playButton, 20, 20, WHITE);
      display.drawBitmap(75, 44, sleepButton, 20, 20, WHITE);
      display.drawBitmap(100, 44, deathButton, 20, 20, WHITE);
      break;
    case 1:
      display.drawBitmap(0, 44, eatButton, 20, 20, WHITE);
      display.drawBitmap(25, 44, cleanButton, 20, 20, BLACK, WHITE);
      display.drawBitmap(50, 44, playButton, 20, 20, WHITE);
      display.drawBitmap(75, 44, sleepButton, 20, 20, WHITE);
      display.drawBitmap(100, 44, deathButton, 20, 20, WHITE);
      break;
    case 2:
      display.drawBitmap(0, 44, eatButton, 20, 20, WHITE);
      display.drawBitmap(25, 44, cleanButton, 20, 20, WHITE);
      display.drawBitmap(50, 44, playButton, 20, 20, BLACK, WHITE);
      display.drawBitmap(75, 44, sleepButton, 20, 20, WHITE);
      display.drawBitmap(100, 44, deathButton, 20, 20, WHITE);
      break;
    case 3:
      display.drawBitmap(0, 44, eatButton, 20, 20, WHITE);
      display.drawBitmap(25, 44, cleanButton, 20, 20, WHITE);
      display.drawBitmap(50, 44, playButton, 20, 20, WHITE);
      display.drawBitmap(75, 44, sleepButton, 20, 20, BLACK, WHITE);
      display.drawBitmap(100, 44, deathButton, 20, 20, WHITE);
      break;
    case 4:
      display.drawBitmap(0, 44, eatButton, 20, 20, WHITE);
      display.drawBitmap(25, 44, cleanButton, 20, 20, WHITE);
      display.drawBitmap(50, 44, playButton, 20, 20, WHITE);
      display.drawBitmap(75, 44, sleepButton, 20, 20, WHITE);
      display.drawBitmap(100, 44, deathButton, 20, 20, BLACK, WHITE);
      break;
    default:
      display.drawBitmap(0, 44, eatButton, 20, 20, WHITE);
      display.drawBitmap(25, 44, cleanButton, 20, 20, WHITE);
      display.drawBitmap(50, 44, playButton, 20, 20, WHITE);
      display.drawBitmap(75, 44, sleepButton, 20, 20, WHITE);
      display.drawBitmap(100, 44, deathButton, 20, 20, WHITE);
    }
    display.display();
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void vUserInputTask(void *pvParameters)
{
  while (1)
  {
    uint32_t input = getUserInput();
    if (input != 0 && xQueueSend(xUserInputQueue, (void *)&input, (TickType_t)0) == pdTRUE)
    {
      Serial.println("Enviado exitosamente.");
    }
    vTaskDelay(pdMS_TO_TICKS(300));
  }
}

void vAgeTask(void *pvParameters)
{
  while (1)
  {
    edad++;
    vTaskDelay(pdMS_TO_TICKS(2000 / k));
  }
}

void vStateUpdateTask(void *pvParameters)
{
  while (1)
  {
    xSemaphoreTake(xDataMutex, portMAX_DELAY);
    (limpieza > 0) ? limpieza -= 2 : limpieza = limpieza;
    (hambre > 0) ? hambre -= 4 : hambre = hambre;
    (sueno > 0) ? sueno -= 2 : sueno = sueno;
    (aburrimiento > 0) ? aburrimiento -= 4 : aburrimiento = aburrimiento;
    if (hambre<=30){
      (salud > 0) ? salud -= 2 : salud = salud;
    }
    if (sueno<=30){
      (salud > 0) ? salud -= 2 : salud = salud;
    }
    if (aburrimiento<=20){
      (salud > 0) ? salud -= 1 : salud = salud;
    }
    if (limpieza<=15){
      (salud > 0) ? salud -= 1 : salud = salud;
    }
    xSemaphoreGive(xDataMutex);
    vTaskDelay(pdMS_TO_TICKS(400 / k));
  }
}

void vFeedingTask(void *pvParameters)
{
  while (1)
  {
    // Esperar a que esté disponible la comida
    // xSemaphoreTake(xFoodAvailableSemaphore, portMAX_DELAY);
    // Alimentar a la mascota
    // vTaskDelay(pdMS_TO_TICKS(500)); // Esperar 0.5 segundos
  }
}

void vGameTask(void *pvParameters)
{
  while (1)
  {
    // Realizar una acción de juego
    // xSemaphoreGive(xMultipleGamesSemaphore); // Liberar el semáforo para permitir ot ro juego
    // vTaskDelay(pdMS_TO_TICKS(2000));         // Esperar 2 segundos
    sensor.getAcceleration(&ax, &ay, &az);
    sensor.getRotation(&gx, &gy, &gz);
    // Serial.print("a[x y z] g[x y z]:\t");
    // Serial.print(ax);
    // Serial.print("\t");
    // Serial.print(ay);
    // Serial.print("\t");
    // Serial.print(az);
    // Serial.print("\t");
    // Serial.print(gx);
    // Serial.print("\t");
    // Serial.print(gy);
    // Serial.print("\t");
    // Serial.println(gz);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void vSleepTask(void *pvParameters)
{
  while (1)
  {
    // Control de sueño de la mascota
    xSemaphoreTake(xDataMutex, portMAX_DELAY);

    xSemaphoreGive(xDataMutex);
  }
}

void vCleanTask(void *pvParameters)
{
  while (1)
  {
    // Control de sueño de la mascota
    xSemaphoreTake(xDataMutex, portMAX_DELAY);
    xSemaphoreGive(xDataMutex);
  }
}
