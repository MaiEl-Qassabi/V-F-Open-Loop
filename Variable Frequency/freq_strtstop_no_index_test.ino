#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h> 
LiquidCrystal lcd(14,23,25,27,29,31); //Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
// Global variables for PWM generation
int sinpwm[] = {520, 540, 560, 580, 599, 619, 638, 657, 676, 695, 713, 731, 749, 766, 782, 799, 815, 830, 845, 859, 872, 886, 898, 910, 921, 931, 941, 950, 959, 966, 973, 979, 984, 989, 993, 996, 998, 999, 1000, 1000, 999, 997, 994, 991, 987, 982, 976, 970, 962, 954, 946, 936, 926, 915, 904, 892, 879, 866, 852, 837, 822, 807, 791, 774, 757, 740, 722, 704, 686, 667, 648, 629, 609, 590, 570, 550, 530, 510, 490, 470, 450, 430, 410, 391, 371, 352, 333, 314, 296, 278, 260, 243, 226, 209, 193, 178, 163, 148, 134, 121, 108, 96, 85, 74, 64, 54, 46, 38, 30, 24, 18, 13, 9, 6, 3, 1, 0, 0, 1, 2, 4, 7, 11, 16, 21, 27, 34, 41, 50, 59, 69, 79, 90, 102, 114, 128, 141, 155, 170, 185, 201, 218, 234, 251, 269, 287, 305, 324, 343, 362, 381, 401, 420, 440, 460, 480, 500};
/*float sinpwm[] = {0.015625  ,  0.0546875 ,  0.09375   ,  0.1328125 ,  0.16992188,
        0.20898438,  0.24609375,  0.28320312,  0.3203125 ,  0.35742188,
        0.39257812,  0.42773438,  0.46289062,  0.49609375,  0.52734375,
        0.56054688,  0.59179688,  0.62109375,  0.65039062,  0.67773438,
        0.703125  ,  0.73046875,  0.75390625,  0.77734375,  0.79882812,
        0.81835938,  0.83789062,  0.85546875,  0.87304688,  0.88671875,
        0.90039062,  0.91210938,  0.921875  ,  0.93164062,  0.93945312,
        0.9453125 ,  0.94921875,  0.95117188,  0.953125  ,  0.953125  ,
        0.95117188,  0.94726562,  0.94140625,  0.93554688,  0.92773438,
        0.91796875,  0.90625   ,  0.89453125,  0.87890625,  0.86328125,
        0.84765625,  0.828125  ,  0.80859375,  0.78710938,  0.765625  ,
        0.7421875 ,  0.71679688,  0.69140625,  0.6640625 ,  0.63476562,
        0.60546875,  0.57617188,  0.54492188,  0.51171875,  0.47851562,
        0.4453125 ,  0.41015625,  0.375     ,  0.33984375,  0.30273438,
        0.265625  ,  0.22851562,  0.18945312,  0.15234375,  0.11328125,
        0.07421875,  0.03515625, -0.00390625, -0.04296875, -0.08203125,
       -0.12109375, -0.16015625, -0.19921875, -0.23632812, -0.27539062,
       -0.3125    , -0.34960938, -0.38671875, -0.421875  , -0.45703125,
       -0.4921875 , -0.52539062, -0.55859375, -0.59179688, -0.62304688,
       -0.65234375, -0.68164062, -0.7109375 , -0.73828125, -0.76367188,
       -0.7890625 , -0.8125    , -0.83398438, -0.85546875, -0.875     ,
       -0.89453125, -0.91015625, -0.92578125, -0.94140625, -0.953125  ,
       -0.96484375, -0.97460938, -0.98242188, -0.98828125, -0.99414062,
       -0.99804688, -1.        , -1.        , -0.99804688, -0.99609375,
       -0.9921875 , -0.98632812, -0.97851562, -0.96875   , -0.95898438,
       -0.94726562, -0.93359375, -0.91992188, -0.90234375, -0.88476562,
       -0.86523438, -0.84570312, -0.82421875, -0.80078125, -0.77734375,
       -0.75      , -0.72460938, -0.69726562, -0.66796875, -0.63867188,
       -0.60742188, -0.57421875, -0.54296875, -0.50976562, -0.47460938,
       -0.43945312, -0.40429688, -0.3671875 , -0.33007812, -0.29296875,
       -0.25585938, -0.21679688, -0.1796875 , -0.140625  , -0.1015625 ,
       -0.0625    , -0.0234375};*/
//float sinpwm[] = {20,40,60,80,99,119,138,157,176,195,213,231,249,266,282,299,315,330,345,359,372,386,398,410,421,431,441,450,459,466,473,479,484,489,493,496,498,499,500,500,499,497,494,491,487,482,476,470,462,454,446,436,426,415,404,392,379,366,352,337,322,307,291,274,257,240,222,204,186,167,148,129,109,90,70,50,30,10,-10,-30,-50,-70,-90,-109,-129,-148,-167,-186,-204,-222,-240,-257,-274,-291,-307,-322,-337,-352,-366,-379,-392,-404,-415,-426,-436,-446,-454,-462,-470,-476,-482,-487,-491,-494,-497,-499,-500,-500,-499,-498,-496,-493,-489,-484,-479,-473,-466,-459,-450,-441,-431,-421,-410,-398,-386,-372,-359,-345,-330,-315,-299,-282,-266,-249,-231,-213,-195,-176,-157,-138,-119,-99,-80,-60,-40,-20,0};

int i = 0, j = 52, k = 104, db1, db2, db3, dc, dc2, dc3;
int frequency = 5; // Initial frequency
//int voltagePin = A15; // Analog input pin for voltage measurement
volatile bool startderive =false; 
void setup() {
  pinMode(11, OUTPUT); //A+
  pinMode(12, OUTPUT); //A-
  pinMode(6, OUTPUT); //B+
  pinMode(7, OUTPUT); //B-
  pinMode(5, OUTPUT); //C+
  pinMode(2, OUTPUT); //C-
  pinMode(8, OUTPUT); 

  // Configure Timer1 for PWM generation
TCCR5A = 0b00100010; // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
TCCR5B = 0b00011001;
//ICR5 = 100000/frequency; // Set TOP value for 50 Hz (16 MHz / (1 * 2000) = 50 Hz).   Mostafa: (#of samples =156)*Frequency=16MHz / ICR5 ==>  ICR5=102564.1/Frequency 
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
  pinMode(4, OUTPUT); // Configure pin 4 for the shutdown __ simulating as LED on Proteus. initially gives high 
  digitalWrite(4, HIGH); // Initially turn off the IR (LED on Proteus) until the 'start' button is pressed.
  lcd.begin(16, 2);  // initialize the LCD
}

void loop() {
  if (startderive==true){
  // int voltageReading = analogRead(voltagePin);
  // float voltage = voltageReading * (5.0 / 1023.0);
  //float vdc = voltage* ( (4.7+390)/4.7 );
  lcd.setCursor(0, 0);
  lcd.print("Frequency: ");  // Display initial 
  lcd.setCursor(11, 0);  // Adjust the column position as needed
  lcd.print(String(frequency) + " Hz");
 /* lcd.setCursor(0, 1);
  lcd.print("index: ");
  lcd.print(index, 2);*/

  }
  else
  {
    lcd.setCursor(9, 0);
    lcd.print("                "); // Clear the first row
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear the second row
    lcd.setCursor(0, 0);
    lcd.print("Stop Mode");  // Display initial
   
    }
 
  // Check if the button for increasing frequency is pressed
  if (digitalRead(3) == LOW) {
      delay(100); // Debounce delay __  Adjust the debounce delay(20) for fast response on Proteus (simulation)
                                   //  Increase the delay value to 80 or higher for hardware to prevent button bounce issues
    if (startderive==true){
    if (frequency < 50) {
      frequency += 1; // Increase frequency by 1 Hz
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
    delay(100); // Debounce delay __  Adjust the debounce delay(20) for fast response on Proteus (simulation environment)
                                 //  Increase the delay value to 80 or higher for hardware to prevent button bounce issues
    if (startderive==true){
    if (frequency > 5) {
      frequency -= 1; // Decrease frequency by 1 Hz
      ICR5 = 100000 / (frequency ); // Update TOP value for new frequency
    }
   }
  }
   
}
// Timer1 overflow interrupt service routine
ISR(TIMER5_OVF_vect) {
 digitalWrite(8, HIGH);
  // Update PWM values here
  dc = (sinpwm[i]-20) ; // 20 for deadtime 1.5us
 //dc = ((sinpwm[i00010.0* index + 500; // 20 for deadtime 1.5us
 //dc = ((sinpwm[i]))*500 + 500; // 20 for deadtime 1.5us
 //dc = ((sinpwm[i])) + 500; // 20 for deadtime 1.5us
  if (dc >= 0)
    db1 = (dc);
  else
    db1 = 0;
  OCR1A = sinpwm[i];
  OCR1B = db1;
  i++;
  if (i > 156)
    i = 0;
 
 //dc2 = ((sinpwm[j]))*500+ 500; // 20 for deadtime 1.5us
 dc2 = (sinpwm[j]-20) ; // 20 for deadtime 1.5us
  if (dc2 >= 0)
    db2 = (dc2);
  else
    db2 = 0;
  OCR4A = sinpwm[j];
  OCR4B = db2;
  j++;
  if (j > 156)
    j = 0;

 //dc3 = ((sinpwm[k]))*500 + 500; // 20 for deadtime 1.5us
 dc3 = (sinpwm[k]-20) ; // 20 for deadtime 1.5us
  if (dc3 >= 0)
    db3 = (dc3);
  else
    db3 = 0;
  OCR3A = sinpwm[k];
  OCR3B = db3;
  k++;
  if (k > 156)
 
    k = 0;
    
 digitalWrite(8, LOW);

}
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
 // ICR5 = 100000 / (frequency ); // Update TOP value for new frequency
     digitalWrite(4, HIGH); //to turn on the hardware of IR by shutdown
     TIMSK5 &= ~(1 << TOIE5); // Disable Timer5 overflow interrupt
     TCCR4A=0;
     TCCR1A=0;
     TCCR3A=0;
     startderive=false;
     frequency=5;
}
