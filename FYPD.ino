#include <LiquidCrystal.h>

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
int lcd_key     = 0;
int lcd_key_prev = btnNONE;
int adc_key_in  = 0;

#define DETECT 2  // pin 2 for sensor
int speed_limit;
int counter_LOW = 0;
int counter_blocked = 0;
double time_start = -1;
double time_end = -1;
double speed_current;
double v[6]; 

// read the buttons
int read_LCD_buttons() {
  adc_key_in = analogRead(0);  // read the value from the sensor
  
  // my buttons when read are centered at these valies: 0, 143, 328, 504, 741
  // we add approx 50 to those values and check to see if we are close 
  if (adc_key_in > 1000) return btnNONE;  // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;
  
  return btnNONE;  // when all others fail, return this...
}

void setup() {
  Serial.begin(9600);
  pinMode(DETECT, INPUT);  //define detect input pin
  lcd.begin(16, 2);  // initialize the interface and specify the dimensions: 16*2
  //lcd.setCursor(0, 0);
  //lcd.print("Speed: ");
  lcd.setCursor(0, 1);
  lcd.print("Limit: ");
  speed_limit = 20;
}

void loop() {
  lcd_key = read_LCD_buttons();  // read the buttons
  
  switch (lcd_key) {
    case btnRIGHT:
      // code
      break;
      
    case btnLEFT:
      // code
      break;
     
    case btnUP:
      if (lcd_key != lcd_key_prev) {
        if (speed_limit < 25) speed_limit++;
        else speed_limit = 25;
        //lcd.setCursor(7, 1);  
        //lcd.print(speed_limit);
      }
      break;
      
    case btnDOWN:
    if (lcd_key != lcd_key_prev) {
      if (speed_limit > 15) speed_limit--;
      else speed_limit = 15;
      //lcd.setCursor(7, 1);  
      //lcd.print(speed_limit);
    }
    break;
      
    case btnSELECT:
      // code
      break;
      
    case btnNONE:
      // code
      break;
  }
  lcd_key_prev = lcd_key;

  
  
  double time_per_cycle;
  int detected = digitalRead(DETECT);  // read Laser sensor
  
  if (detected == LOW) {
    counter_LOW++;
    lcd.setCursor(0, 0);
    lcd.print("LOW ");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("HIGH");
    if (counter_LOW > 5) {
      if (time_start != -1) {
        time_end = millis();
        time_per_cycle = time_end - time_start; // the time it takes for to do a full rotation
        speed_current = 7464.42/time_per_cycle;
        if (counter_blocked < 3) {
          v[counter_blocked] = speed_current;
          counter_blocked++;
        } else {
          double average = (v[0] + v[1] + v[2])/3;
          lcd.setCursor(7, 0);
          lcd.print("         ");
          lcd.setCursor(7, 0);
          lcd.print(average);
          v[0] = speed_current;
          counter_blocked = 1;
        }
      }
      time_start = millis();
    }
    counter_LOW = 0;
  }

  double time_span = millis() - time_start;
  if (time_span > 4000) {
    lcd.setCursor(7, 0);
    lcd.print("0        ");
  }
}
