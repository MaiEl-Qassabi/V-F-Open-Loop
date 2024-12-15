  #include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h> 
LiquidCrystal lcd(14,23,25,27,29,31); //Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)

// Global variables for PWM generation
float sinpwm[] = {20,40,60,80,99,119,138,157,176,195,213,231,249,266,282,299,315,330,345,359,372,386,398,410,421,431,441,450,459,466,473,479,484,489,493,496,498,499,500,500,499,497,494,491,487,482,476,470,462,454,446,436,426,415,404,392,379,366,352,337,322,307,291,274,257,240,222,204,186,167,148,129,109,90,70,50,30,10,-10,-30,-50,-70,-90,-109,-129,-148,-167,-186,-204,-222,-240,-257,-274,-291,-307,-322,-337,-352,-366,-379,-392,-404,-415,-426,-436,-446,-454,-462,-470,-476,-482,-487,-491,-494,-497,-499,-500,-500,-499,-498,-496,-493,-489,-484,-479,-473,-466,-459,-450,-441,-431,-421,-410,-398,-386,-372,-359,-345,-330,-315,-299,-282,-266,-249,-231,-213,-195,-176,-157,-138,-119,-99,-80,-60,-40,-20,0};

int i = 0, j = 52, k = 104, db1, db2, db3, dc, dc2, dc3;
//straight line equation between modulation and frequency: (m = (0.018*freq) + 0.1 ); 
int frequency = 2; // Initial frequency
float index= 0.136;// Initial index     
int voltagePin = A15; // Analog input pin for voltage measurement
volatile bool startderive =false; 
void setup() {
  pinMode(11, OUTPUT); //A+
  pinMode(12, OUTPUT); //A-
  pinMode(6, OUTPUT); //B+
  pinMode(7, OUTPUT); //B-
  pinMode(5, OUTPUT); //C+
  pinMode(2, OUTPUT); //C-
 pinMode(8, OUTPUT); // checking overflow in Timer (ISR)
 // Configure Timer1 for PWM generation
TCCR5A = 0b00100010; // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
TCCR5B = 0b00011001;
//Timer5 16bits, max value for ICR = 65536; so@freq=1Hz ICR value should be 100000/1 =100000 "not acceptable" 
ICR5 = 100000/frequency; // Set TOP value for 50 Hz (16 MHz / (1 * 2000) = 50 Hz).   Mostafa: (#of samples =156)*Frequency=16MHz / ICR5 ==>  ICR5=102564.1/Frequency 
TCCR4A=0;//resets the value of Timer 4 Reg. A
TCCR4B=0;//resets the value of Timer 4 Reg. B
TCNT4=0;//resets the value of Timer 4
//TCCR4A=0b11100011;
TCCR4B=0b00000001; 
//TIMSK2 = 0b00000001;
TCCR3A=0;//resets the value of Timer 3 Reg. A
TCCR3B=0;//resets the value of Timer 3 Reg. B
TCNT3=0;//resets the value of Timer 3
//TCCR3A=0b11100011;
TCCR3B=0b00000001; 
TCCR1A=0;//resets the value of Timer 3 Reg. A
TCCR1B=0;//resets the value of Timer 3 Reg. B
TCNT1=0;//resets the value of Timer 3
//TCCR1A=0b11100011;
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
  lcd.begin(16, 2);  // Reinitialize the LCD
}

void loop() {
  if (startderive==true){
   int voltageReading = analogRead(voltagePin);
   float voltage = voltageReading * (5.0 / 1023.0);
   float vdc = voltage* ( (4.7+390)/4.7 );    //voltage divider gain , R1=390k & R2=4.7k
  lcd.setCursor(0, 0); // Display initial
  lcd.print("Frequency: ");   
  lcd.setCursor(11, 0);  // Adjust the column position as needed
  lcd.print(String(frequency) + " Hz");
  lcd.setCursor(0, 1);
  lcd.print("m:");
  lcd.print(index,3);
  lcd.setCursor(9, 1);
  lcd.print("Vdc:");
  lcd.print(vdc, 2);
  lcd.print("V        ");

  }
  else // showing stop mode at start
  {
    lcd.print("                "); // Clear the second row
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear the second row
    lcd.setCursor(0, 0);
    lcd.print("Stop Mode");  // Display initial
    lcd.setCursor(11, 0);
    }
 
  // Check if the button for increasing frequency is pressed
  if (digitalRead(3) == LOW) {
      delay(100); // Debounce delay __  Adjust the debounce delay(10) for fast response on Proteus (simulation)
                                   //  Increase the delay value to 80 or higher for hardware to prevent button bounce issues
    
   if (startderive==true)
   {
     if (frequency < 50) {         
      frequency += 1; // Increase frequency by 1 Hz
      index+=0.018;   //method(1) starting whith index 0.19 at 5 HZ
    ICR5 = 100000 / (frequency ); // Update TOP value for new frequency
   }
   }
  }
  /*It is preferable to check the startderive flag after each delay function rather than using a single main flag at the beginning of the while loop.
   This approach ensures that if the code is running within a delay function and the stop button is pressed
   ,interrupting the delay function and jumping to the ISR function for stop interrupt handling, the execution of the delay function will resume after the ISR completes.
   By checking the startderive flag after each delay function, we prevent scenarios where the interrupt execution alters the program flow
   ,potentially leading to unintended behavior such as  unwanted frequency increment.
   Placing the flag check after the delay function ensures a more robust and predictable program flow.  
  */
  // Check if the button for decreasing frequency is pressed
  if (digitalRead(21) == LOW) {
    delay(100); // Debounce delay __  Adjust the debounce delay(10) for fast response on Proteus (simulation environment)
                                 //  Increase the delay value to 80 or higher for hardware to prevent button bounce issues
    if (startderive==true){
    if (frequency > 2) {
      frequency -= 1; // Decrease frequency by 1 Hz
      index -=0.018;
      ICR5 = 100000 / (frequency ); // Update TOP value for new frequency
    }
   }
  }
   
}
// Timer1 overflow interrupt service routine
ISR(TIMER5_OVF_vect) {
 digitalWrite(8, HIGH);
  // Update PWM values here
  //*********************************************************//
  /*- To construct a sine lookup table suitable for pulse-width modulation (PWM)control in a system utilizing an OCR Timer,
     we first compute a normalized sine function scaled and offset to fit within the allowable range of the timer's output compare register.
     sin lookup table = 500*(sin(pi*i/#of samples)+500)   
    - The sine function is scaled by 500 and then offset by 500 to ensure all values are positive, 
     which matches the range required for the OCR Timer.index (modulation) just multiply with first term to keep shift (offest =500) as is it
     then we need reduce 20 for deadtime
    - So final equation is ( index *[500(sin(pi*i/#of samples)] ) +500-20;    [index*(sinpwm)]-480;
     Finally, the resulting value is cast to an integer type to match the data type required by the timer's register:(int)[index*(sinpwm)]-480*/
  //**********************************************************//
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
  digitalWrite(8, LOW);
}
// Interrupt service routine for start button
void startPulsesISR() {
   if (!startderive)
  {
    
    TIMSK5 |= (1 << TOIE5); // Enable overflow interrupt
    TCCR1A=0b11100011;
    TCCR4A=0b11100011;
    TCCR3A=0b11100011;
    TCCR4B=0b00000001; 
    TCCR3B=0b00000001; 
    TCCR1B=0b00000001; 
    sei();
    ICR5 = 100000/frequency;
    startderive=true;
    digitalWrite(4, LOW); //to turn on the hardware of IR by shutdown
  }  
}
// Interrupt service routine for stop button
void stopPulsesISR() {
     digitalWrite(4, HIGH); //to turn on the hardware of IR by shutdown
     TIMSK5 &= ~(1 << TOIE5); // Disable Timer5 overflow interrupt
     TCCR4A=0;
     TCCR1A=0;
     TCCR3A=0;
     startderive=false;
     index=0.136;
     frequency=2;
}
