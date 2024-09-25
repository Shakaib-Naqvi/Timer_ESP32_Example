/*
   Timer Interrupt in ESP32, Alarm generation in ESP32
   www.electronicwings.com
 */

#define LED_PIN 2  // In some ESP32 board have inbuilt LED

volatile int interruptCounter;    //for counting interrupt
volatile int interruptCounter_2;  //for counting interrupt
int totalInterruptCounter;        //total interrupt counting
int totalInterruptCounter_2;      //total interrupt counting
int LED_STATE = LOW;
hw_timer_t* timer = NULL;    //H/W timer defining (Pointer to the Structure)
hw_timer_t* timer_2 = NULL;  //H/W timer defining (Pointer to the Structure)

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {  //Defining Interrupt function with IRAM_ATTR for faster access
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR onTimer_2() {  //Defining Interrupt function with IRAM_ATTR for faster access
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter_2++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  timer = timerBegin(0, 80, true);                  // timer 0, prescalar: 80, UP counting
  timer_2 = timerBegin(1, 80, true);                // timer 1, prescalar: 80, UP counting
  timerAttachInterrupt(timer, &onTimer, true);      // Attach interrupt
  timerAttachInterrupt(timer_2, &onTimer_2, true);  // Attach interrupt
  timerAlarmWrite(timer, 1000000, true);            // Match value= 1000000 for 1 sec. delay.
  timerAlarmWrite(timer_2, 1000000, true);          // Match value= 1000000 for 1 sec. delay.
  timerAlarmEnable(timer);                          // Enable Timer with interrupt (Alarm Enable)
}

void loop() {
  if (interruptCounter > 0) {

    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;  //counting total interrupt

    LED_STATE = !LED_STATE;            //toggle logic
    digitalWrite(LED_PIN, LED_STATE);  //Toggle LED
    Serial.print("An interrupt has occurred. Total number: ");
    Serial.println(totalInterruptCounter);
  }
  if (totalInterruptCounter > 9) {
    // timer_2 = timerBegin(1, 80, true);  // timer 1, prescalar: 80, UP counting
    // timerAttachInterrupt(timer_2, &onTimer, true);  // Attach interrupt
    // timerAlarmWrite(timer_2, 2000000, true);        // Match value= 2000000 for 2 sec. delay.
    totalInterruptCounter = 0;
    timerAlarmDisable(timer);
    timerWrite(timer, 0);
    timerAlarmEnable(timer_2);  // Enable Timer with interrupt (Alarm Enable)
  }
  if (interruptCounter_2 > 0) {

    portENTER_CRITICAL(&timerMux);
    interruptCounter_2--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter_2++;  //counting total interrupt
    Serial.print("The Second interrupt has occurred. Total number: ");
    Serial.println(totalInterruptCounter_2);
  }
  if (totalInterruptCounter_2 > 19) {
    totalInterruptCounter_2 = 0;
    timerAlarmDisable(timer_2);
    timerWrite(timer_2, 0);
    timerAlarmEnable(timer);  // Enable Timer with interrupt (Alarm Enable)
  }
}