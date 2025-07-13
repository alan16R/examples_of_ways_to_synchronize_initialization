/**
 * Example of initialization mechanism using counting semaphores.
 * 
 * There are four tasks:-
 * 
 * initTask  -- represents initialization that must be done before any other tasks run.
 * TaskA, TaskB, TaskC - these are 'user' tasks. 
 * 
 * The initTask indicates that it has completed its initialization by 'giving' the initialization
 * semaphore 3 times. Each of the user tasks is unblocked when they can 'take' one of the sempahore
 * counts.
 * 
 * Additionally, the order in which they resume is controlled by the task priority. The highest priority
 * task starts first - in the case of the example this is Task C.
 * 
 * 
 */
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>


#define STACK_SIZE 2048 // seems to be the minimum for ESP32.
SemaphoreHandle_t initSemaphore;

TaskHandle_t initTask;
TaskHandle_t taskA;
TaskHandle_t taskB;
TaskHandle_t taskC;

void xInitTask(void *);
void xTaskA(void *);
void xTaskB(void *);
void xTaskC(void *);

void showCountdown(uint16_t count);




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("Creating tasks.");
  Serial.flush();

  initSemaphore = xSemaphoreCreateCounting(3,0);
  // tasks block in order of execution, but unblock in order of priority
  // A,B,C -> C,B,A 
  BaseType_t a = xTaskCreate(xInitTask, "INIT", STACK_SIZE, NULL, 1, &initTask);
  BaseType_t b1 = xTaskCreate(xTaskA, "TASKA", STACK_SIZE, NULL, 2, &taskA );
  BaseType_t b2 = xTaskCreate(xTaskB, "TASKB", STACK_SIZE, NULL, 3, &taskB );
  BaseType_t b3 = xTaskCreate(xTaskC, "TASKC", STACK_SIZE, NULL, 4, &taskC );
 }

void loop() {}

// ***** Task implementations *****

void xInitTask(void *pvParams) {
  Serial.println("Starting the 'init' task\n");
  Serial.flush();
  showCountdown(5); // equivalent to doing some long running initialization.
  Serial.println("Init complete, giving semaphores.");
  Serial.flush();
  // now give the semaphore 3 times - this will set the count to 3.
  for (int i = 0; i < 3; i++) {
    xSemaphoreGive(initSemaphore);
  }
  vTaskDelete(NULL); // we are done, get rid of initTask.
}

void xTaskA(void *pvParams) {
  Serial.println("Starting task A - blocking for semaphore");
  Serial.flush();
  // until a semaphore 'take' succeeds we block.
  // No error checking because we use portMAX_DELAY.
  // In real code you might want to have a shorter delay 
  // and recheck the semaphore state.
  xSemaphoreTake(initSemaphore, portMAX_DELAY);
  Serial.println("Task A unblocked!");
  Serial.flush();  
  while(true) {
    Serial.println("Task A running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(2500));

  }
}

void xTaskB(void *pvParams) {
  Serial.println("Starting task B - blocking for semaphore");
  Serial.flush();
  xSemaphoreTake(initSemaphore, portMAX_DELAY);
  Serial.println("Task B unblocked!");
  Serial.flush();
  while(true) {
    Serial.println("Task B running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(3000));

  }
}

void xTaskC(void *pvParams) {
  Serial.println("Starting task C - blocking for semaphore");
  Serial.flush();
  xSemaphoreTake(initSemaphore, portMAX_DELAY);
  Serial.println("Task C unblocked!");
  Serial.flush();  
  while(true) {
    Serial.println("Task C running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(5000));
  }
}

// ** utility functions

void showCountdown(uint16_t count){
  while(count > 0) {
    Serial.printf("%02d\b\b",count--);
    vTaskDelay(pdTICKS_TO_MS(1000));
  }
  Serial.println();
}
