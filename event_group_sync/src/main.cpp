/**
 * Example of initialization mechanism by using an event group.
 * 
 * There are four tasks:-
 * 
 * initTask  -- represents initialization that must be done before any other tasks run.
 * TaskA, TaskB, TaskC - these are 'user' tasks. 
 * 
 * 
 * An event group is created. There needs to be one bit for each task that is awaiting the complete
 * of initialization. Each task awaits its corresponding bit.
 * 
 * When the init sequence is complete, the 'initTask' sets the bit mask for all the Tasks that are waiting and
 * they resume execution determined by their task priority.
 * 
 */
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>


#define STACK_SIZE 2048 // seems to be the minimum for ESP32 - see 'HWM' in initTask.

#define TASK_A_BIT (1 << 1)
#define TASK_B_BIT (1 << 2)
#define TASK_C_BIT (1 << 3)
#define EVENT_BITS (TASK_A_BIT | TASK_B_BIT | TASK_C_BIT)


TaskHandle_t initTask;
TaskHandle_t taskA;
TaskHandle_t taskB;
TaskHandle_t taskC;

EventGroupHandle_t eventGroupHandle;

void showCountdown(uint16_t);

void xTaskA(void *);
void xTaskB(void *);
void xTaskC(void *);
void xInitTask(void *);



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(3000);
  Serial.println("Creating tasks.");
  Serial.flush();
  
  eventGroupHandle = xEventGroupCreate();

  // tasks block in order of execution, but unblock in order of priority
  // A,B,C -> C,B,A 
  BaseType_t b1 = xTaskCreate(xTaskA, "TASKA", STACK_SIZE, eventGroupHandle, 2, &taskA );
  BaseType_t b2 = xTaskCreate(xTaskB, "TASKB", STACK_SIZE, eventGroupHandle, 3, &taskB );
  BaseType_t b3 = xTaskCreate(xTaskC, "TASKC", STACK_SIZE, eventGroupHandle, 4, &taskC );
  BaseType_t a = xTaskCreate(xInitTask, "INIT", STACK_SIZE, eventGroupHandle, 1, &initTask);
}

void loop() {
}

void xTaskA(void * pvParams) {
  EventBits_t uxBits;
  EventGroupHandle_t eventHandle = (EventGroupHandle_t )(pvParams);
  Serial.println("Starting task A.");
  Serial.flush();
  // wait until this task bit is set.
  uxBits = xEventGroupWaitBits(eventHandle, TASK_A_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
  if(uxBits != EVENT_BITS) {
    Serial.print("uxBits: ");Serial.println(uxBits);
  }
  Serial.println("Task A is resuming.");
  while(true) {
    vTaskDelay(pdTICKS_TO_MS(3000));
    Serial.println("Task A running");
    Serial.flush();
  }
}

void xTaskB(void * pvParams) {
  EventBits_t uxBits;
  EventGroupHandle_t eventHandle = (EventGroupHandle_t )(pvParams);
  Serial.println("Starting task B.");
  Serial.flush();
  uxBits = xEventGroupWaitBits(eventHandle, TASK_B_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
  if(uxBits != EVENT_BITS) {
    Serial.print("uxBits: ");Serial.println(uxBits);
  }
  Serial.println("Task B is resuming.");

  while(true) {
    Serial.println("Task B running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(7000));
  }
}

void xTaskC(void * pvParams) {
  EventBits_t uxBits;
  EventGroupHandle_t eventHandle = (EventGroupHandle_t )(pvParams);
  Serial.println("Starting task C.");
  Serial.flush();
  uxBits = xEventGroupWaitBits(eventHandle, TASK_C_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
  if(uxBits != EVENT_BITS) {
    Serial.print("uxBits: ");Serial.println(uxBits);
  }
  Serial.println("Task C is resuming.");
  while(true) {
    Serial.println("Task C running");
    Serial.flush();
    vTaskDelay(pdTICKS_TO_MS(2000));
  }
}

void xInitTask(void * pvParams) {
  EventBits_t uxBits;
  EventGroupHandle_t eventHandle = (EventGroupHandle_t )(pvParams);
  Serial.println("Starting initialization processing - all bits are 0.");
  Serial.flush();
  showCountdown(5); // equivalent to doing something.  
  Serial.println("\n\nInit complete, setting all bits to 1.");
  Serial.flush();
  xEventGroupSetBits(eventHandle, EVENT_BITS);
  Serial.printf("\nHigh watermark (words): %d\n", uxTaskGetStackHighWaterMark( NULL ));
  Serial.flush();
  vTaskDelete(NULL); // we are done, get rid of initTask.
}

void showCountdown(uint16_t count){
  while(count > 0) {
    Serial.printf("%02d\b\b",count--);
    vTaskDelay(pdTICKS_TO_MS(1000));
  }
}