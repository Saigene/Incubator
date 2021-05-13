#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS3231.h>

#define Type DHT11
#define ONE_WIRE_BUS 8
#define sensehumidity 2
#define pinLED 3
#define buttonset 7
#define buttonnext 6
#define buttonprev 5
#define buttonback 4

unsigned long led_dalay;
bool stateLED=true;
float tempC;
int humidity;
//float value_1 = 63972.5;
float value_1 = 65535 - ((16000000 * .100)/1024);

//==============Set Varialble====
int high_limit_humid=80;
int low_limit_humid=70;
int high_limit_temp=37;
int low_limit_temp=35;

int seal_humidity=1;
int bulb_status=0;


//==============button==============
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;
int lastButtonState_set = LOW;
int buttonState_set;

int lastButtonState_next = LOW;
int buttonState_next;

int lastButtonState_prev = LOW;
int buttonState_prev;

int lastButtonState_back = LOW;
int buttonState_back;
   
int btn_state_menu = 0;
int btn_state_menu_lcd = 0;
int btn_state_set = 0;
int btn_state_enter=0;
int btn_back_state=0;
int btn_state_add=0;
int btn_state_minus=0;
int add_minus_value=0;

int Day_value=1;
int High_limit_temp=38;
int Low_limit_temp=36;

int temp_select=0;
int temp_select_high_state=1;
int temp_select_low_state=0;
int add_high_temp=0;
int add_low_temp=0;
String next_prev_menu="defualt";
String display_screen="main";
String display_menu="default";
String display_lcd="default";

int menu=0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT HT(sensehumidity,Type);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DS3231  rtc(SDA, SCL);

void setup() {
  // put your setup code here, to run once:
// noInterrupts();
lcd.begin();
welcomescreen();
HT.begin();
rtc.begin();
sensors.begin();
pinMode(pinLED, OUTPUT);
pinMode(buttonset, INPUT);
pinMode(buttonnext, INPUT);
pinMode(buttonprev, INPUT);
pinMode(buttonback, INPUT);
Serial.begin(9600);

loadingscreen();
 
// The following lines can be uncommented to set the date and time
//  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(23, 25, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(5, 8, 2021);   // Set the date to January 1st, 2014


 // Timer====================================================
   noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = value_1;                        // preload timer
  TCCR1B |= (1 << CS10)|(1 << CS12);    // 1024 prescaler 
  TIMSK1 |= (1 << TOIE1);               // enable timer overflow interrupt ISR  
  interrupts(); 

  // PCINT========================================================
//  PCICR |= (1 << PCIE2);
//  PCMSK2 |= (1 << PCINT23);
//  PCMSK2 |= (1 << PCINT22);
//  PCMSK2 |= (1 << PCINT21);
//  PCMSK2 |= (1 << PCINT20);


}

void loop() {
  if(display_screen == "main"){
    checkedHumidity();
    checkedTemperature();
    displaylcd();
    ledblinking();
  }



  if(menu == 1 && btn_state_menu == 1){
    Serial.println("Temp Setting");
      clearlcd();
    lcd.print("1.Set Temp:");
    lcd.setCursor(0,1);
    lcd.print("Enter <   > Back");
    btn_state_menu=0; 
  }else if(menu == 1 && btn_state_enter==1){
    clearlcd();
    lcd.print("Set Temp High:");
    lcd.setCursor(0,1);
    lcd.print("High:" + String(High_limit_temp) + "   Low:" +String(Low_limit_temp));
    btn_state_enter=0;
    }

  if(menu == 2 && btn_state_menu == 1){
    Serial.println("Humidity Setting");
    clearlcd();
    lcd.print("5.Set Humid");
    lcd.setCursor(0,1);
    lcd.print("Enter <   > Back");
    btn_state_menu=0;
  }else if(menu == 2 && btn_state_enter==1){
    clearlcd();
    lcd.print("Humidity:");
    lcd.setCursor(0,1);
    lcd.print("High:00  Low:00");
    btn_state_enter=0;
    }

  if(menu == 3 && btn_state_menu == 1){
    Serial.println("Turner Setting");
    clearlcd();
    lcd.print("4.Set Turner");
    lcd.setCursor(0,1);
    lcd.print("Enter <   > Back");
    btn_state_menu=0;
  }else if(menu == 3 && btn_state_enter==1){
    clearlcd();
    lcd.print("Turner:");
    lcd.setCursor(0,1);
    lcd.print("Hour:00 Min:00");
    btn_state_enter=0;
    }

  if(menu == 4 && btn_state_menu == 1){
    Serial.println("Day Setting");
    clearlcd();
    lcd.print("3.Set Day");
    lcd.setCursor(0,1);
    lcd.print("Enter <   > Back");
    btn_state_menu=0;
  }else if(menu == 4 && btn_state_enter==1){
    clearlcd();
    lcd.print("Set: Day");
    lcd.setCursor(0,1);
    lcd.print("Day:" + String(Day_value));
    btn_state_enter=0;
  }
  

  if(menu == 5 && btn_state_menu == 1){
    Serial.println("Time Setting");
    clearlcd();
    lcd.print("2.Set Time");
    lcd.setCursor(0,1);
    lcd.print("Enter <   > Back");
    btn_state_menu=0;
  }else if(menu == 5 && btn_state_enter==1){    
    clearlcd();
    lcd.print("Time:");
    lcd.setCursor(0,1);
    lcd.print("Hour:00:00AM");
    btn_state_enter=0;
    }

  checkedbutton();
}



void clearlcd(){
  lcd.clear();
  delay(200);
//  interrupts(); 
  }


void checkedbutton(){

int reading_set=digitalRead(buttonset);
int reading_next=digitalRead(buttonnext);
int reading_prev=digitalRead(buttonprev);
int reading_back=digitalRead(buttonback);

if(reading_set != lastButtonState_set){lastDebounceTime = millis();}
if(reading_next != lastButtonState_next){lastDebounceTime = millis();}
if(reading_prev != lastButtonState_prev){lastDebounceTime = millis();}
if(reading_back != lastButtonState_back){lastDebounceTime = millis();}


//===================================================================================SET BUTTON 1========================================================

if(display_screen=="settings"){

  if((millis()-lastDebounceTime) > debounceDelay){
      if(reading_set != buttonState_set){
        buttonState_set = reading_set;
          if(buttonState_set == HIGH){
            Serial.println("Enter Menu" + String(menu));
             btn_back_state=menu;
             btn_state_enter=1;
             next_prev_menu="add_minus";display_screen="enter_settings";
             if(menu == 1){
              if(temp_select_high_state == 1){
                Serial.println("Temp high");
                add_high_temp=1;add_low_temp=0;
                temp_select_high_state=0;
                temp_select_low_state=1;
              }
              }        
          }
        }
      }


//=========================================================================================================================NEXT PREV
  if(next_prev_menu == "next_prev"){
      
        if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_next != buttonState_next){
                buttonState_next = reading_next;
                if(buttonState_next == HIGH){
                   btn_state_menu = 1;menu = menu + 1;
                   if(menu==6){menu=1;}
                }
            }
         }

        if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_prev != buttonState_prev){
                buttonState_prev = reading_prev;
                if(buttonState_prev == HIGH){
                  btn_state_menu = 1;menu = menu - 1;
                  if(menu <= 0){menu=5;}
                }
            }
        }
    
   }


//=============================================================================================================================BACK
if((millis()-lastDebounceTime) > debounceDelay){
    if(reading_back != buttonState_back){
      buttonState_back = reading_back;
        if(buttonState_back == HIGH){
            Serial.println("Main"); 
            display_screen="main";
            clearlcd();
            menu=0; 
          
        }
      }
   } 
}

//========================================================================SET BUTTON 2==============================================================

if(display_screen == "enter_settings"){
  
  if((millis()-lastDebounceTime) > debounceDelay){
      if(reading_set != buttonState_set){
        buttonState_set = reading_set;
          if(buttonState_set == HIGH){
            Serial.println(menu);
            if(menu == 1){
              if(temp_select_high_state == 1){
                Serial.println("Temp high");
                add_high_temp=1;add_low_temp=0;                
                temp_select_high_state=0;
                temp_select_low_state=1;
              }else if(temp_select_low_state == 1){
                add_low_temp=1;add_high_temp=0;
                Serial.println("Temp low");
                temp_select_high_state=1;
                temp_select_low_state=0;
                }
              
              }
            if(menu == 2){Serial.println("move set humid");}
            if(menu == 3){Serial.println("move set turner");}
            if(menu == 4){Serial.println("move set day");}
            if(menu == 5){Serial.println("move set time");}      
          }
        }
      }
  
  //=========================================================================================================================ADD MINUS

   if(next_prev_menu == "add_minus"){

        if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_next != buttonState_next){
                buttonState_next = reading_next;
                if(buttonState_next == HIGH){
                 Serial.println("add");
                 if(menu==1 && add_high_temp==1){Serial.println("add hightemp");}
                 if(menu==1 && add_low_temp==1){Serial.println("add lowtemp");}
                }
            }
         }

        if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_prev != buttonState_prev){
                buttonState_prev = reading_prev;
                if(buttonState_prev == HIGH){
                  Serial.println("minus");
                }
            }
        }
    
    
    }
//===========================================================================================================================SAVING
    if((millis()-lastDebounceTime) > debounceDelay){
    if(reading_back != buttonState_back){
      buttonState_back = reading_back;
        if(buttonState_back == HIGH){             
              btn_state_menu = 1;menu=btn_back_state;btn_back_state=0;btn_state_add=0;btn_state_minus=0;add_minus_value=0;
              next_prev_menu = "next_prev";
              display_screen="settings";
              Serial.println("Setting");
              if(menu==1){Serial.println("Saving Temp");temp_select_low_state=0;temp_select_high_state=1;}
              if(menu==2){Serial.println("Saving Humid");}
              if(menu==3){Serial.println("Saving Turner");}
              if(menu==4){Serial.println("Saving Day");}
              if(menu==5){Serial.println("Saving Time");}
            }
        }
      }
   }
  

          
lastButtonState_set = reading_set;
lastButtonState_next = reading_next;
lastButtonState_prev = reading_prev;    
lastButtonState_back = reading_back; 
  
}

  



//==========================================================================Interrupt Button=====================================================================

ISR(TIMER1_OVF_vect)                    // interrupt service routine for overflow 
{                                   
    TCNT1 = value_1;                        //checked button if press.

int reading_set=digitalRead(buttonset);
  if(reading_set != lastButtonState_set){lastDebounceTime = millis();}
    if(display_screen=="main"){
  
      if((millis()-lastDebounceTime) > debounceDelay){
          if(reading_set != buttonState_set){
            buttonState_set = reading_set;
              if(buttonState_set == HIGH){
                 btn_state_set = 1;btn_state_menu = 1;
                 display_screen="settings";
                 next_prev_menu = "next_prev";
                  Serial.println("Settings");
                  Serial.println("Btn =:" + next_prev_menu);
                  menu=1;
              }
           }
      }      
   }
         
lastButtonState_set = reading_set;

}


//ISR (PCINT2_vect){
//
//lastDebounceTime = millis();
//
//if((millis()-lastDebounceTime) > debounceDelay){
//  Serial.println("Setting Temperature");
//    if(btn_state == 0 && PIND & B10000000){
//      btn_state =  1;
//    }else if(btn_state == 1 && !(PIND & B10000000)){
//      Serial.println("Setting Temperature");
//      btn_state=0;
//  }
//}
//
//}

//====================================================================checking Sensors===========================================================================

void checkedHumidity(){
    humidity=HT.readHumidity();
//    if(humidity >= high_limit_humid){seal_humidity=1;}
//    else if(humidity < low_limit_humid){seal_humidity=0;}
//    if(seal_humidity==1){Serial.print("Seal Open --  ");}             // set servo=1 open high humid
//    else if(seal_humidity==0){Serial.print("Seal Close  --  ");}     // set servo=2 close low humid
}

void checkedTemperature(){
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
//  if(tempC >= high_limit_temp){bulb_status=0;}
//  else if(tempC <= low_limit_temp){bulb_status=1;}
//  if(bulb_status==0){Serial.println("Light off");}                     //set bulb off in high temp
//  else if(bulb_status==1){Serial.println("Light On");}                 //set bulb on in low temp
}
 
void displaylcd(){
lcd.setCursor(0,0);
lcd.print("T:" + String(tempC));
lcd.print((char) 223);
lcd.print("C");
lcd.print("  H:" + String(humidity) + "%");
lcd.setCursor(0,1);
lcd.print(rtc.getTimeStr());
lcd.print(" D:" + String("10"));
}

void welcomescreen(){
lcd.print("    Welcome");
lcd.setCursor(0,1);
lcd.print(" Atan  Solution");
delay(2000);  
}

void loadingscreen(){
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Loading...");
  checkedHumidity();
  checkedTemperature();
  delay(2000);
}


void ledblinking(){ 
  if(micros() - led_dalay > 1000000){     
     stateLED = !stateLED;
     led_dalay=micros();
    }
      
  if(stateLED){
      digitalWrite(pinLED, LOW);
    }else{
      digitalWrite(pinLED, HIGH);
    }
}
