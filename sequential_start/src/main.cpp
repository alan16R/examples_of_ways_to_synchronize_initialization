/**
 * Example of initialization mechanism by starting tasks when initialization complete.
 * 
 * There are four tasks:-
 * 
 * initTask  -- represents initialization that must be done before any other tasks run.
 * TaskA, TaskB, TaskC - these are 'user' tasks. 
 * 
 * When the initialization delay of 5 seconds is complete, the 'initTask' starts the other
 * three 'user' tasks running.
 * 
 * 
 */
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>


#define STACK_SIZE 2048 // seems to be the minimum for ESP32.
//SemaphoreHandle_t initSemaphore;

TaskHandle_t initTask;
TaskHandle_t taskA;
TaskHandle_t taskB;
TaskHandle_t taskC;

void showCountdown(uint16_t);


void xTaskA(void *pvParams);

void xTaskB(void *pvParams);

void xTaskC(void *pvParams);

void xInitTask(void *pvParams);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("Creating tasks.");
  Serial.flush();

  // tasks block in order of execution, but unblock in order of priority
  // A,B,C -> C,B,A 
  BaseType_t a = xTaskCreate(xInitTask, "INIT", STACK_SIZE, NULL, 1, &initTask);

 }

void loop() {
 
}


// **** task implemenations

void xTaskA(void *pvParams) {
  Serial.println("Starting task A - init complete.");
  Serial.flush();
  while(true) {
    Serial.println("Task A running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(3000));

  }
}

void xTaskB(void *pvParams) {
  Serial.println("Starting task B  - init complete.");
  Serial.flush();
  while(true) {
    Serial.println("Task B running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(2000));
  }
}

void xTaskC(void *pvParams) {
  Serial.println("Starting task C  - init complete.");
  Serial.flush();
  while(true) {
    Serial.println("Task C running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(3000));
  }
}

void xInitTask(void *pvParams) {
  Serial.println("Starting initialisation.");
  Serial.flush();
  showCountdown(5);
  // equivalent to doing something.
  Serial.println("Init complete - starting 'user' tasks.");
  Serial.flush();
  BaseType_t b1 = xTaskCreate(xTaskA, "TASKA", STACK_SIZE, NULL, 2, &taskA );
  BaseType_t b2 = xTaskCreate(xTaskB, "TASKB", STACK_SIZE, NULL, 3, &taskB );
  BaseType_t b3 = xTaskCreate(xTaskC, "TASKC", STACK_SIZE, NULL, 4, &taskC );
  Serial.printf("\nStack HWM: %d\n",uxTaskGetStackHighWaterMark( NULL ));
  vTaskDelete(NULL); // we are done, get rid of initTask.
}

void showCountdown(uint16_t count){
  while(count > 0) {
    Serial.printf("%02d\b\b",count--);
    vTaskDelay(pdTICKS_TO_MS(1000));
  }
  Serial.println();
}