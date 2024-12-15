#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h> 
LiquidCrystal lcd(14,23,25,27,29,31); //Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
// Global variables for PWM generation
float sinpwm[] = {20,40,60,80,99,119,138,157,176,195,213,231,249,266,282,299,315,330,345,359,372,386,
                  398,410,421,431,441,450,459,466,473,479,484,489,493,496,498,499,500,500,499,497,494,
                  491,487,482,476,470,462,454,446,436,426,415,404,392,379,366,352,337,322,307,291,274,
                  257,240,222,204,186,167,148,129,109,90,70,50,30,10,-10,-30,-50,-70,-90,-109,-129,-148,
                  -167,-186,-204,-222,-240,-257,-274,-291,-307,-322,-337,-352,-366,-379,-392,-404,-415,
                  -426,-436,-446,-454,-462,-470,-476,-482,-487,-491,-494,-497,-499,-500,-500,-499,-498,
                  -496,-493,-489,-484,-479,-473,-466,-459,-450,-441,-431,-421,-410,-398,-386,-372,-359,
                  -345,-330,-315,-299,-282,-266,-249,-231,-213,-195,-176,-157,-138,-119,-99,-80,-60,-40,
                  -20,0};
int i = 0, j = 52, k = 104, db1, db2, db3, dc, dc2, dc3;
int frequency = 50; // Initial frequency
volatile float index = 0.0;  // Initial index value
volatile bool rampingUp = false;  // Flag to indicate if ramp-up is in progress
unsigned long rampStart;  // Variable to store the start time of ramp-up
volatile bool startderive =false;
volatile bool feedback =false;
int voltagePin = A15; // Analog input pin for voltage measurement
void setup() {
  pinMode(11, OUTPUT); //A+
  pinMode(12, OUTPUT); //A-
  pinMode(6, OUTPUT); //B+
  pinMode(7, OUTPUT); //B-
  pinMode(5, OUTPUT); //C+
  pinMode(2, OUTPUT); //C-
// Configure Timer1 for PWM generation
TCCR5A = 0b00100010; // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
TCCR5B = 0b00011001;
ICR5 = 100000/frequency; // Set TOP value for 50 Hz (16 MHz / (1 * 2000) = 50 Hz).   
                         //  (#of samples =156)*Frequency=16MHz / ICR5 ==>  ICR5=102564.1/Frequency 
TCCR4A=0;//resets the value of Timer 4 Reg. A
TCCR4B=0;//resets the value of Timer 4 Reg. B
TCNT4=0;//resets the value of Timer 4
TCCR4B=0b00000001; 
TCCR3A=0;//resets the value of Timer 3 Reg. A
TCCR3B=0;//resets the value of Timer 3 Reg. B
TCNT3=0;//resets the value of Timer 3
TCCR3B=0b00000001; 
TCCR1A=0;//resets the value of Timer 3 Reg. A
TCCR1B=0;//resets the value of Timer 3 Reg. B
TCNT1=0;//resets the value of Timer 3
TCCR1B=0b00000001; 
  
// Set up push buttons
  pinMode(3, INPUT_PULLUP); // Button for increasing frequency
  pinMode(21, INPUT_PULLUP); // Button for decreasing frequency
  pinMode(19, INPUT_PULLUP); // Button for start
  pinMode(20, INPUT_PULLUP); // Button for stop
  attachInterrupt(digitalPinToInterrupt(19), startPulsesISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(20), stopPulsesISR, FALLING);
  pinMode(4, OUTPUT); // Configure pin 4 for the shutdown
  digitalWrite(4, HIGH); // Initially turn off the IR
  sei(); // Enable global interrupts
}

void loop() {
   if( feedback == false)
   {
   delay(10000);
   feedback=true;
 
   }

if (rampingUp) {
    unsigned long currentTime = millis();
   float indexramp = float(currentTime - rampStart) / 2000.0;  // Adjust the ramp duration as needed
    index = constrain(indexramp, 0.0, 1.0);

    // If the ramp is complete, set the flag to false
    if (index == 1.0) {
      rampingUp = false;
    }
  }
}

ISR(TIMER5_OVF_vect) {
  // Update PWM values here
 // dc = (int)(((sinpwm[i])*index) - 20); // 20 for deadtime 1.5us
 dc = (int)(((sinpwm[i]))* index + 480); // 20 for deadtime 1.5us
  if (dc >= 0)
    db1 = dc;
  else
    db1= 0;
  OCR1A = (dc+20) ;
  OCR1B = db1;
  i++;
  if (i > 157)
    i = 0;

 // dc2 = (int)(((sinpwm[j])*index) - 20);
  dc2 = (int)((sinpwm[j])* index + 480); // 20 for deadtime 1.5us
  if (dc2 >= 0)
    db2 = dc2;
  else
    db2 = 0;
  OCR4A = (dc2+20);
  OCR4B = db2;
  j++;
  if (j > 157)
    j = 0;

  //dc3 = (int)(((sinpwm[k])*index) - 20);
   dc3 = (int)((sinpwm[k])* index + 480); // 20 for deadtime 1.5us
  if (dc3 >= 0)
    db3 = dc3;
  else
    db3 = 0;
  OCR3A = (dc3+20);
  OCR3B = db3;
  k++;
  if (k > 157)
    k = 0;
}

void startPulsesISR() {
  if(   feedback==true )
  {
   if (!startderive )
  {
     digitalWrite(4, LOW); //to turn on the hardware of IR by shutdown
     startderive=true;
     rampStart = millis();  //  start time of ramp up
     TIMSK5 |= (1 << TOIE5); // Enable overflow interrupt
     TCCR1A=0b11100011;
     TCCR4A=0b11100011;
     TCCR3A=0b11100011;
     rampingUp = true;  // Set the ramp up flag
     rampStart = millis();  //  start time of ramp up
     lcd.begin(16, 2);  // Reinitialize the LCD
    sei();
  }
  }   
}

// Interrupt service routine for stop button
void stopPulsesISR() {
   startderive=false;
   digitalWrite(4, HIGH); //to turn on the hardware of IR by shutdown
   lcd.noDisplay(); // Turn off the LCD
   lcd.clear();
   TIMSK5 &= ~(1 << TOIE5); // Disable Timer5 overflow interrupt
   cli();
   TCCR4A=0;
   TCCR1A=0;
   TCCR3A=0;
   index=0.0;
   rampingUp = false; 
}
