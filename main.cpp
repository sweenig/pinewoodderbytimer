// Interrupt Driven Pinewood Derby Timer
// BDub @ Technobly.com 3/19/2014
//
// All inputs are pulled high with internal pullups.
// GND D2 to reset and run the timer for all lanes.
// GND A6, A7, D3 and D4 to stop the timer for each
// lane and display the results.
//
// D0-D3 were initially chosen, but because 
// individual control of dis/en'abling their
// interrupt handlers was not available, A6, A7
// D3 and D4 were chosen instead.  D0, D1, D2
// A0, A1, A3, A4 are all tied to one interrupt
// handler, so for the one remaining input any 
// of these will do.  D2 was chosen to allow
// for the most flexibility of the remaining inputs.
// 
// EXTI_ClearITPendingBit() is necessary to clear
// the interrupt Pending register, or interrupts 
// will fire immediately after enabling the 
// interrupt handlers again.
//
// DWT->CYCCNT was used for the timer instead of 
// micros() which wraps this hardware counter and
// returns the number of microseconds associated
// with it; because it's not clean to handle the 
// case where micros() wraps at 59.652323 seconds.
// This give us 72x more resolution on the timing
// anyway and it's super easy to deal with wrapping.
// It just works out through subtraction of unsigned
// 32-bit variables; as long as you don't time 
// something longer than 59.652323 seconds.
//
//==================================================

#include "application.h"

void startRace(void);
void lane1(void);
void lane2(void);
void lane3(void);
void lane4(void);
bool showStartMsg = false;
bool raceEnded = true; // start off assuming the race has not started.
volatile uint32_t timeStart1;
volatile uint32_t timeStart2;
volatile uint32_t timeStart3;
volatile uint32_t timeStart4;
volatile uint32_t timeEnd1;
volatile uint32_t timeEnd2;
volatile uint32_t timeEnd3;
volatile uint32_t timeEnd4;
volatile uint32_t mph1;
volatile uint32_t mph2;
volatile uint32_t mph3;
volatile uint32_t mph4;




uint32_t startTime;
const uint32_t DISQUALIFIED_TIME = 20 * 1000;  // in milliseconds (20 seconds)

void setup()
{
  Serial.begin(115200);
  while(!Serial.available()) SPARK_WLAN_Loop(); // Waiting for user to open terminal and press ENTER
  // Enter waiting state, waiting for D2 to go low.
  Serial.println("================================");
  Serial.println("Waiting for race to start.");

  //setup pins
  pinMode(D2, INPUT); // startRace trigger
  pinMode(A6, INPUT); // lane1
  pinMode(A7, INPUT); // lane2
  pinMode(D3, INPUT); // lane3
  pinMode(D4, INPUT); // lane4

  //setup actions to perform on interrupts
  attachInterrupt(D2, startRace, FALLING); // startRace
  attachInterrupt(A6, lane1, FALLING); // lane1
  attachInterrupt(A7, lane2, FALLING); // lane2
  attachInterrupt(D3, lane3, FALLING); // lane3
  attachInterrupt(D4, lane4, FALLING); // lane4

  //Stop listening to interrupts on all lanes
  NVIC_DisableIRQ(EXTI0_IRQn); // A6 "Lane 1"
  NVIC_DisableIRQ(EXTI1_IRQn); // A7 "Lane 2"
  NVIC_DisableIRQ(EXTI4_IRQn); // D3 "Lane 3"
  NVIC_DisableIRQ(EXTI3_IRQn); // D4 "Lane 4"
}

void loop()
{
  //wait for race to end
  if(!raceEnded) {
    //wait for all lanes to have times (this includes any empty lanes, which will timeout at 20 seconds)
    if((timeEnd1 != timeStart1 && timeEnd2 != timeStart2 && timeEnd3 != timeStart3 && timeEnd4 != timeStart4) || (millis() - startTime) > DISQUALIFIED_TIME) {

      double tempTime;

      Serial.println("Race Finished!");
      Serial.println("= Lane Times in seconds =");
      
      //output lane times
      Serial.print("Lane 1: "); 
      tempTime = (double)(timeEnd1 - timeStart1)/72000000.0;
      if(tempTime != 0.0) {
          Serial.print(tempTime,8);
          Serial.print(" (");
          mph1 = (double)(31 * 60 * 60 * 24 / 5280 / tempTime);
          Serial.print(mph1,6);
          Serial.println(" mph)");
      } else Serial.println("DNF");
      delay(50);
      Serial.print("Lane 2: "); 
      tempTime = (double)(timeEnd2 - timeStart2)/72000000.0;
      if(tempTime != 0.0) {
          Serial.print(tempTime,8);
          Serial.print(" (");
          mph2 = (double)(31 * 60 * 60 * 24 / 5280 / tempTime);
          Serial.print(mph2,6);
          Serial.println(" mph)");
      } else Serial.println("DNF");
      delay(50);
      Serial.print("Lane 3: "); 
      tempTime = (double)(timeEnd3 - timeStart3)/72000000.0;
      if(tempTime != 0.0) {
          Serial.print(tempTime,8);
          Serial.print(" (");
          mph3 = (double)(31 * 60 * 60 * 24 / 5280 / tempTime);
          Serial.print(mph3,6);
          Serial.println(" mph)");
      } else Serial.println("DNF");
      delay(50);
      Serial.print("Lane 4: "); 
      tempTime = (double)(timeEnd4 - timeStart4)/72000000.0;
      if(tempTime != 0.0) {
          Serial.print(tempTime,8);
          Serial.print(" (");
          mph4 = (double)(31 * 60 * 60 * 24 / 5280 / tempTime);
          Serial.print(mph4,6);
          Serial.println(" mph)");
      } else Serial.println("DNF");
      delay(50);
      
      //cleanup
      raceEnded = true; // prevents results from being displayed over and over  
      EXTI_ClearITPendingBit(EXTI_Line5); // D2 "startRace"
      NVIC_EnableIRQ(EXTI9_5_IRQn); // D2
      
      // Enter waiting state, waiting for D2 to go low.
      Serial.println("================================");
      Serial.println("Waiting for race to start.");
    }
  }

  if(showStartMsg) {
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("================================");
    Serial.println("Race started!");
    showStartMsg = false;
    raceEnded = false;
    startTime = millis(); // Capture the rough start time, for disqualification timer
  }
}

void lane1()
{
  NVIC_DisableIRQ(EXTI0_IRQn); // stop listening for an interrupt on A6
  timeEnd1 = DWT->CYCCNT; //get the lane time (instead of using micros();)
}

void lane2()
{
  NVIC_DisableIRQ(EXTI1_IRQn); // stop listening for an interrupt on A7
  timeEnd2 = DWT->CYCCNT; //get the lane time (instead of using micros();)
}

void lane3()
{
  NVIC_DisableIRQ(EXTI4_IRQn); // stop listening for an interrupt on D3
  timeEnd3 = DWT->CYCCNT; //get the lane time (instead of using micros();)
}

void lane4()
{
  NVIC_DisableIRQ(EXTI3_IRQn); // stop listening for an interrupt on D4
  timeEnd4 = DWT->CYCCNT; //get the lane time (instead of using micros();)
}

void startRace()
{
  NVIC_DisableIRQ(EXTI9_5_IRQn); // stop listening for an interrupt on D2
  timeStart1 = DWT->CYCCNT; // get the start time for each lane
  timeStart2 = timeStart1; // set the start time for each line the same
  timeStart3 = timeStart1; // set the start time for each line the same
  timeStart4 = timeStart1; // set the start time for each line the same
  timeEnd1 = timeStart1; // set the end time to the start time temporarily
  timeEnd2 = timeStart1; // set the end time to the start time temporarily
  timeEnd3 = timeStart1; // set the end time to the start time temporarily
  timeEnd4 = timeStart1; // set the end time to the start time temporarily
  EXTI_ClearITPendingBit(EXTI_Line0); // Reset the interrupt for A6 "Lane 1"
  EXTI_ClearITPendingBit(EXTI_Line1); // Reset the interrupt for A7 "Lane 2"
  EXTI_ClearITPendingBit(EXTI_Line4); // Reset the interrupt for D3 "Lane 3"
  EXTI_ClearITPendingBit(EXTI_Line3); // Reset the interrupt for D4 "Lane 4"
  NVIC_EnableIRQ(EXTI0_IRQn); // Start listening for an interrupt on A6 "Lane 1"
  NVIC_EnableIRQ(EXTI1_IRQn); // Start listening for an interrupt on A7 "Lane 2"
  NVIC_EnableIRQ(EXTI4_IRQn); // Start listening for an interrupt on D3 "Lane 3"
  NVIC_EnableIRQ(EXTI3_IRQn); // Start listening for an interrupt on D4 "Lane 4"
  showStartMsg = true; //indicate that the race has started
}
