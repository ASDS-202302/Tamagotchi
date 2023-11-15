#include <Arduino.h>
#include <Wire.h>
//#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
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

// Prototipos de funciones para tareas
void vUITask(void *pvParameters);
void vStateUpdateTask(void *pvParameters);
void vFeedingTask(void *pvParameters);
void vGameTask(void *pvParameters);
void vSleepTask(void *pvParameters);
void vPowerControlTask(void *pvParameters);

// Definición de colas y semáforos
QueueHandle_t xUserInputQueue;
QueueHandle_t xGameResultQueue;
SemaphoreHandle_t xFoodAvailableSemaphore;
SemaphoreHandle_t xMultipleGamesSemaphore;
SemaphoreHandle_t xUserInputSemaphore;
SemaphoreHandle_t xDataMutex;


void setup() {
  // Inicialización de periféricos y recursos aquí

  // Crear colas y semáforos
  xUserInputQueue = xQueueCreate(5, sizeof(uint8_t));
  xGameResultQueue = xQueueCreate(5, sizeof(uint8_t));
  xFoodAvailableSemaphore = xSemaphoreCreateBinary();
  xMultipleGamesSemaphore = xSemaphoreCreateMutex();
  xUserInputSemaphore = xSemaphoreCreateBinary();
  xDataMutex = xSemaphoreCreateMutex();

  // Crear tareas
  xTaskCreate(vUITask, "UI Task", 1000, NULL, 2, NULL);
  xTaskCreate(vStateUpdateTask, "State Update Task", 1000, NULL, 2, NULL);
  xTaskCreate(vFeedingTask, "Feeding Task", 1000, NULL, 2, NULL);
  xTaskCreate(vGameTask, "Game Task", 1000, NULL, 2, NULL);
  xTaskCreate(vSleepTask, "Sleep Task", 1000, NULL, 2, NULL);
  xTaskCreate(vPowerControlTask, "Power Control Task", 1000, NULL, 2, NULL);

  // Iniciar el planificador de FreeRTOS
  vTaskStartScheduler();
}

void loop(){
  // No hacer nada
}

uint8_t getUserInput(){
  return 0;
}

void vUITask(void *pvParameters){
  while (1) {
    if (xSemaphoreTake(xUserInputSemaphore, portMAX_DELAY) == pdTRUE) {
      uint8_t userInput = getUserInput(); // Obtener entrada del usuario (simulado)
      xQueueSend(xUserInputQueue, &userInput, portMAX_DELAY);
    }
  }
}

void vStateUpdateTask(void *pvParameters) {
// Implementación de la tarea de actualización de estado
  while (1) {
    // Actualizar estado general de la mascota
    // Usar xSemaphoreTake() y xSemaphoreGive() para acceder a datos compartidos
    xSemaphoreTake(xDataMutex, portMAX_DELAY);
    // Actualizar datos co mpartidos
    xSemaphoreGive(xDataMutex);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo
  }
}

void vFeedingTask(void *pvParameters) {
  while (1) {
    // Esperar a que esté disponible la comida
    xSemaphoreTake(xFoodAvailableSemaphore, portMAX_DELAY);
    // Alimentar a la mascota
    vTaskDelay(pdMS_TO_TICKS(500)); // Esperar 0.5 segundos
  }
}

void vGameTask(void *pvParameters) {
  while (1) {
    // Realizar una acción de juego
    xSemaphoreGive(xMultipleGamesSemaphore); // Liberar el semáforo para permitir ot ro juego
    vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos
  }
}

void vSleepTask(void *pvParameters) {
  while (1) {
    // Control de sueño de la mascota
    vTaskDelay(pdMS_TO_TICKS(5000)); // Esperar 5 segundos
  }
}

void
vPowerControlTask(void *pvParameters) {
  while (1) {
    // Control de energía
    vTaskDelay(pdMS_TO_TICKS(10000)); // Esperar 10 segundos
  }
}