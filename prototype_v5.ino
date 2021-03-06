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
int High_limit_humid=38, High_limit_humid_1=3, High_limit_humid_2=8;
int Low_limit_humid=36, Low_limit_humid_1=3, Low_limit_humid_2=6;
int Turner_Hour_1=1, Turner_Hour_2=0, Turner_Min_1=0, Turner_Min_2=0;
int  Time_Hour_1=0, Time_Hour_2=0, Time_Min_1=0, Time_Min_2=0;

int humid_select_high_1=1, humid_select_high_2=0, humid_select_low_1=0, humid_select_low_2=0, humid_add_high_1=0, humid_add_high_2=0, humid_add_low_1=0, humid_add_low_2=0;
//    humid_low_high_1=0, humid_low_high_2=0, humid_low_low_1=0, humid_low_low_2=0;

int turner_select_hour_1=1,turner_select_hour_2=0,turner_select_min_1=0,turner_select_min_2=0, 
    turner_add_hour_1=0, turner_add_hour_2=0, turner_add_min_1=0, turner_add_min_2=0;

int temp_select_high_state=1, temp_select_low_state=0, 
    add_high_temp=0, add_low_temp=0, minus_high_temp=0, minus_low_temp=0;

int time_select_hour_1=1,time_select_hour_2=0,time_select_min_1=0, time_select_min_2=0, 
    time_add_hour_1=0, time_add_hour_2=0, time_add_min_1=0, time_add_min_2=0;

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
    lcd.setCursor(6,1); lcd.blink();
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
    lcd.print("Humidity High:");
    lcd.setCursor(0,1);
    lcd.print("High:" + String(High_limit_humid_1)+ String(High_limit_humid_2) +  "  Low:" + String(Low_limit_humid_1) + String(Low_limit_humid_2));
    lcd.setCursor(5,1); lcd.blink(); 
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
    lcd.print("Set Turner Hour:");
    lcd.setCursor(0,1);
    lcd.print("Hour:" + String(Turner_Hour_1) + String(Turner_Hour_2) + " Min:" + String(Turner_Min_1) + String(Turner_Min_2));
    lcd.setCursor(5,1); lcd.blink(); 
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
    lcd.print("Set Time Hour:");
    lcd.setCursor(0,1);
    lcd.print("Hour:"+ String(Time_Hour_1) + String(Time_Hour_2) + " Min:" + String(Time_Min_1) + String(Time_Min_1));
    lcd.setCursor(5,1); lcd.blink(); 
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
                Serial.println("Temp high");add_high_temp=1;add_low_temp=0;temp_select_high_state=0;temp_select_low_state=1;
                 lcd.setCursor(6,1); lcd.blink();}
              }
             if(menu == 2){
               if(humid_select_high_1 == 1){Serial.println("humid high 1");
              humid_add_high_1=1; humid_add_high_2=0; humid_add_low_1=0; humid_add_low_2=0; humid_select_high_1=0; humid_select_high_2=1; humid_select_low_1=0; humid_select_low_2=0;
              lcd.setCursor(0,0); lcd.print("Humidity High:");
              lcd.setCursor(5,1); lcd.blink();}             
              }

              if(menu == 3){
                  if(turner_select_hour_1 == 1){Serial.println("turner hour 1");
                  turner_add_hour_1=1; turner_add_hour_2=0; turner_add_min_1=0; turner_add_min_2=0;  turner_select_hour_1=0; turner_select_hour_2=1; turner_select_min_1=0; turner_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Turner Hour:");
                  lcd.setCursor(5,1); lcd.blink();} 
                }

              if(menu == 5){
                if(time_select_hour_1 == 1){Serial.println("time hour 1");
                time_add_hour_1=1; time_add_hour_2=0; time_add_min_1=0; time_add_min_2=0;  time_select_hour_1=0; time_select_hour_2=1; time_select_min_1=0; time_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Time Hour:");
                  lcd.setCursor(5,1); lcd.blink();}
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
            lcd.noBlink(); 
          
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
              if(temp_select_high_state == 1){Serial.println("Temp high");
                add_high_temp=1; minus_high_temp=1; add_low_temp=0; minus_low_temp=0; temp_select_high_state=0; temp_select_low_state=1; lcd.setCursor(9,0); lcd.print("High:");
                lcd.setCursor(6,1); lcd.blink();
                }
              else if(temp_select_low_state == 1){Serial.println("Temp low");
                add_low_temp=1;minus_low_temp=1;add_high_temp=0;minus_high_temp=0;temp_select_high_state=1;temp_select_low_state=0;lcd.setCursor(9,0);lcd.print("Low: ");
                lcd.setCursor(15,1); lcd.blink();
                }
                
              }
            if(menu == 2){
              if(humid_select_high_1 == 1){Serial.println("humid high");
              humid_add_high_1=1; humid_add_high_2=0; humid_add_low_1=0; humid_add_low_2=0; humid_select_high_1=0; humid_select_high_2=1; humid_select_low_1=0; humid_select_low_2=0;
              lcd.setCursor(0,0); lcd.print("Humidity High:");
              lcd.setCursor(5,1); lcd.blink();
              }
              else if(humid_select_high_2 == 1){Serial.println("humid high");
              humid_add_high_1=0; humid_add_high_2=1; humid_add_low_1=0; humid_add_low_2=0; humid_select_high_1=0; humid_select_high_2=0; humid_select_low_1=1; humid_select_low_2=0;
              lcd.setCursor(0,0); lcd.print("Humidity High:");
              lcd.setCursor(6,1); lcd.blink();
              }
              
              else if(humid_select_low_1 == 1){Serial.println("humid low");
              humid_add_high_1=0; humid_add_high_2=0; humid_add_low_1=1; humid_add_low_2=0; humid_select_high_1=0; humid_select_high_2=0; humid_select_low_1=0; humid_select_low_2=1;
              lcd.setCursor(0,0); lcd.print("Humidity Low:  ");
              lcd.setCursor(13,1); lcd.blink();
              }
              else if(humid_select_low_2 == 1){Serial.println("humid low");
              humid_add_high_1=0; humid_add_high_2=0; humid_add_low_1=0; humid_add_low_2=1; humid_select_high_1=1; humid_select_high_2=0; humid_select_low_1=0; humid_select_low_2=0;
              lcd.setCursor(0,0); lcd.print("Humidity Low:  ");
              lcd.setCursor(14,1); lcd.blink();
              }
              
              
            }
            if(menu == 3){
              if(turner_select_hour_1 == 1){Serial.println("turner hour 1");
                  turner_add_hour_1=1; turner_add_hour_2=0; turner_add_min_1=0; turner_add_min_2=0;  turner_select_hour_1=0; turner_select_hour_2=1; turner_select_min_1=0; turner_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Turner Hour:");
                  lcd.setCursor(5,1); lcd.blink();
                  }
              else if(turner_select_hour_2 == 1){Serial.println("turner hour 2");
                  turner_add_hour_1=0; turner_add_hour_2=1; turner_add_min_1=0; turner_add_min_2=0;  turner_select_hour_1=0; turner_select_hour_2=0; turner_select_min_1=1; turner_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Turner Hour:");
                  lcd.setCursor(6,1); lcd.blink();
                  }    
              else if(turner_select_min_1 == 1){Serial.println("turner min 1");
                  turner_add_hour_1=0; turner_add_hour_2=0; turner_add_min_1=1; turner_add_min_2=0;  turner_select_hour_1=0; turner_select_hour_2=0; turner_select_min_1=0; turner_select_min_2=1;
                  lcd.setCursor(0,0); lcd.print("Set Turner Min:  ");
                  lcd.setCursor(12,1); lcd.blink();
                  }
              else if(turner_select_min_2 == 1){Serial.println("turner min 2");
                  turner_add_hour_1=0; turner_add_hour_2=0; turner_add_min_1=0; turner_add_min_2=1;  turner_select_hour_1=1; turner_select_hour_2=0; turner_select_min_1=0; turner_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Turner Min:  ");
                  lcd.setCursor(13,1); lcd.blink();
                  }    
            }
            if(menu == 4){Serial.println("move set day");}
            
            if(menu == 5){
              if(time_select_hour_1 == 1){Serial.println("time hour 1");
                  time_add_hour_1=1; time_add_hour_2=0; time_add_min_1=0; time_add_min_2=0;  time_select_hour_1=0; time_select_hour_2=1; time_select_min_1=0; time_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Time Hour:");
                  lcd.setCursor(5,1); lcd.blink();
                  }
              else if(time_select_hour_2 == 1){Serial.println("time hour 2");
                  time_add_hour_1=0; time_add_hour_2=1; time_add_min_1=0; time_add_min_2=0;  time_select_hour_1=0; time_select_hour_2=0; time_select_min_1=1; time_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Time Hour:");
                  lcd.setCursor(6,1); lcd.blink();
                  }    
              else if(time_select_min_1 == 1){Serial.println("time min 1");
                  time_add_hour_1=0; time_add_hour_2=0; time_add_min_1=1; time_add_min_2=0;  time_select_hour_1=0; time_select_hour_2=0; time_select_min_1=0; time_select_min_2=1;
                  lcd.setCursor(0,0); lcd.print("Set Time Min:  ");
                  lcd.setCursor(12,1); lcd.blink();
                  }
              else if(time_select_min_2 == 1){Serial.println("time min 2");
                  time_add_hour_1=0; time_add_hour_2=0; time_add_min_1=0; time_add_min_2=1;  time_select_hour_1=1; time_select_hour_2=0; time_select_min_1=0; time_select_min_2=0;
                  lcd.setCursor(0,0); lcd.print("Set Time Min:  ");
                  lcd.setCursor(13,1); lcd.blink();
                  }
              }      
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
                 if(menu == 1){ temperature_add();}
                 if(menu == 2){humidity_add();}
                 if(menu == 3){turner_add();}
                 if(menu==4){day_add();}
                 if(menu==5){time_add();}
                }
            }
         }

        if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_prev != buttonState_prev){
                buttonState_prev = reading_prev;
                if(buttonState_prev == HIGH){
                  Serial.println("minus");
                  if(menu == 1){ temperature_minus();}
                  if(menu == 2){humidity_minus();}
                  if(menu == 3){turner_minus();}
                  if(menu==4){day_minus();}
                  if(menu==5){time_minus();}
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
              lcd.noBlink();
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

void temperature_add(){
  if(add_high_temp == 1){Serial.println("add hightemp");High_limit_temp = High_limit_temp + 1;}
  if(add_low_temp==1){Serial.println("add lowtemp");Low_limit_temp = Low_limit_temp + 1;}
  lcd.setCursor(5,1);  lcd.print(High_limit_temp);
  lcd.setCursor(14,1);  lcd.print(Low_limit_temp);
  }
void temperature_minus(){
  if(minus_high_temp==1){Serial.println("minus hightemp");High_limit_temp = High_limit_temp - 1;}
  if(minus_low_temp==1){Serial.println("minus lowtemp");Low_limit_temp = Low_limit_temp - 1;} 
  lcd.setCursor(5,1);  lcd.print(High_limit_temp);
  lcd.setCursor(14,1);  lcd.print(Low_limit_temp);
  }

void humidity_add(){
  Serial.println("adding humid");
  if(humid_add_high_1==1){Serial.println("add humid h1");
     High_limit_humid_1 = High_limit_humid_1 + 1;
     if(High_limit_humid_1 > 9){ High_limit_humid_1 = 0;}
     lcd.setCursor(5,1);  lcd.print(High_limit_humid_1);
     lcd.setCursor(5,1); lcd.blink();
  }
  if(humid_add_high_2==1){Serial.println("add humid h2");
     High_limit_humid_2 = High_limit_humid_2 + 1;
     if(High_limit_humid_2 > 9){ High_limit_humid_2 = 0;}
     lcd.setCursor(6,1);  lcd.print(High_limit_humid_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(humid_add_low_1==1){Serial.println("add humid l1");
     Low_limit_humid_1 = Low_limit_humid_1 + 1;
     if(Low_limit_humid_1 > 9){ Low_limit_humid_1 = 0;}
     lcd.setCursor(13,1);  lcd.print(Low_limit_humid_1);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  if(humid_add_low_2==1){Serial.println("add humid l2");
     Low_limit_humid_2 = Low_limit_humid_2 + 1;
     if(Low_limit_humid_2 > 9){ Low_limit_humid_2 = 0;}
     lcd.setCursor(14,1);  lcd.print(Low_limit_humid_2);
     lcd.setCursor(14,1); lcd.blink(); 
  }

  }  
void humidity_minus(){
    Serial.println("minus humid");
  if(humid_add_high_1==1){Serial.println("add humid h1");
     High_limit_humid_1 = High_limit_humid_1 - 1;
     if(High_limit_humid_1 <= 1){ High_limit_humid_1 = 1;}
     lcd.setCursor(5,1);  lcd.print(High_limit_humid_1);
     lcd.setCursor(5,1); lcd.blink();
  }
  if(humid_add_high_2==1){Serial.println("add humid h2");
     High_limit_humid_2 = High_limit_humid_2 - 1;
     if(High_limit_humid_2 < 0){ High_limit_humid_2 = 0;}
     lcd.setCursor(6,1);  lcd.print(High_limit_humid_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(humid_add_low_1==1){Serial.println("add humid l1");
     Low_limit_humid_1 = Low_limit_humid_1 - 1;
     if(Low_limit_humid_1 <= 1){ Low_limit_humid_1 = 1;}
     lcd.setCursor(13,1);  lcd.print(Low_limit_humid_1);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  if(humid_add_low_2==1){Serial.println("add humid l2");
     Low_limit_humid_2 = Low_limit_humid_2 - 1;
     if(Low_limit_humid_2 < 0 ){ Low_limit_humid_2 = 0;}
     lcd.setCursor(14,1);  lcd.print(Low_limit_humid_2);
     lcd.setCursor(14,1); lcd.blink(); 
  }
  
  }

void turner_add(){
  Serial.println("adding turner");
  if(turner_add_hour_1==1){Serial.println("add turner hour 1");
    if(Turner_Hour_2 > 4){
       Turner_Hour_1 = Turner_Hour_1 + 1;
       if(Turner_Hour_1 > 1){ Turner_Hour_1 = 0;} 
     }else{
        Turner_Hour_1 = Turner_Hour_1 + 1;
       if(Turner_Hour_1 > 2){ Turner_Hour_1 = 0;} 
      }
     lcd.setCursor(5,1);  lcd.print(Turner_Hour_1);
     lcd.setCursor(5,1); lcd.blink(); 
  } 
     
  if(turner_add_hour_2==1){Serial.println("add turner hour 2");
    if(Turner_Hour_1 > 1){
       Turner_Hour_2 = Turner_Hour_2 + 1;
       if(Turner_Hour_2 > 4){ Turner_Hour_2 = 0;} 
      }else{
        Turner_Hour_2 = Turner_Hour_2 + 1;
       if(Turner_Hour_2 > 9){ Turner_Hour_2 = 0;}
        } 
     lcd.setCursor(6,1);  lcd.print(Turner_Hour_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(turner_add_min_1==1){Serial.println("add turner min 1");
     Turner_Min_1 = Turner_Min_1 + 1;
     if(Turner_Min_1 > 5){ Turner_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Turner_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(turner_add_min_2==1){Serial.println("add turner min 2");
     Turner_Min_2 = Turner_Min_2 + 1;
     if(Turner_Min_2 > 9){ Turner_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Turner_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  
  }

void turner_minus(){
    Serial.println("minus turner");
  if(turner_add_hour_1==1){Serial.println("add turner hour 1");
      Turner_Hour_1 = Turner_Hour_1 - 1;
      if(Turner_Hour_1 < 0){ Turner_Hour_1 = 0;} 
     lcd.setCursor(5,1);  lcd.print(Turner_Hour_1);
     lcd.setCursor(5,1); lcd.blink();
  }
  if(turner_add_hour_2==1){Serial.println("add turner hour 2");
     Turner_Hour_2 = Turner_Hour_2 - 1;
     if(Turner_Hour_2 < 0){ Turner_Hour_2 = 0;}
     lcd.setCursor(6,1);  lcd.print(Turner_Hour_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(turner_add_min_1==1){Serial.println("add turner min 1");
     Turner_Min_1 = Turner_Min_1 - 1;
     if(Turner_Min_1 < 0){ Turner_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Turner_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(turner_add_min_2==1){Serial.println("add turner min 2");
     Turner_Min_2 = Turner_Min_2 - 1;
     if(Turner_Min_2 < 0){ Turner_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Turner_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  
  }   

void day_add(){
  Serial.println("adding day");
  Day_value = Day_value + 1;
   if(Day_value <= 9){ lcd.setCursor(4,1); lcd.print(Day_value); lcd.setCursor(5,1); lcd.print(" ");
      }else{ lcd.setCursor(4,1); lcd.print(Day_value); }
}
void day_minus(){
   Day_value = Day_value - 1;
   if(Day_value == 0){Day_value=1;}
   if(Day_value <= 9){ lcd.setCursor(4,1); lcd.print(Day_value); lcd.setCursor(5,1); lcd.print(" ");
      }else{ lcd.setCursor(4,1); lcd.print(Day_value); }
}

void time_add(){
Serial.println("adding time");
  if(time_add_hour_1==1){Serial.println("add time hour 1");
    if(Time_Hour_2 > 4){
       Time_Hour_1 = Time_Hour_1 + 1;
       if(Time_Hour_1 > 1){ Time_Hour_1 = 0;} 
     }else{
        Time_Hour_1 = Time_Hour_1 + 1;
       if(Time_Hour_1 > 2){ Time_Hour_1 = 0;} 
      }
     lcd.setCursor(5,1);  lcd.print(Time_Hour_1);
     lcd.setCursor(5,1); lcd.blink(); 
  } 
     
  if(time_add_hour_2==1){Serial.println("add time hour 2");
    if(Time_Hour_1 > 1){
       Time_Hour_2 = Time_Hour_2 + 1;
       if(Time_Hour_2 > 4){ Time_Hour_2 = 0;} 
      }else{
        Time_Hour_2 = Time_Hour_2 + 1;
       if(Time_Hour_2 > 9){ Time_Hour_2 = 0;}
        } 
     lcd.setCursor(6,1);  lcd.print(Time_Hour_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(time_add_min_1==1){Serial.println("add time min 1");
     Time_Min_1 = Time_Min_1 + 1;
     if(Time_Min_1 > 5){ Time_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Time_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(time_add_min_2==1){Serial.println("add time min 2");
     Time_Min_2 = Time_Min_2 + 1;
     if(Time_Min_2 > 9){ Time_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Time_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }  
  }
void time_minus(){
  Serial.println("minus time");
  if(time_add_hour_1==1){Serial.println("add time hour 1");
       Time_Hour_1 = Time_Hour_1 - 1;
       if(Time_Hour_1 < 0){ Time_Hour_1 = 0;} 
      
     lcd.setCursor(5,1);  lcd.print(Time_Hour_1);
     lcd.setCursor(5,1); lcd.blink(); 
  } 
     
  if(time_add_hour_2==1){Serial.println("add time hour 2");
    Time_Hour_2 = Time_Hour_2 - 1;
    if(Time_Hour_2 < 0){ Time_Hour_2 = 0;}
         
     lcd.setCursor(6,1);  lcd.print(Time_Hour_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(time_add_min_1==1){Serial.println("add time min 1");
     Time_Min_1 = Time_Min_1 - 1;
     if(Time_Min_1 < 0){ Time_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Time_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(time_add_min_2==1){Serial.println("add time min 2");
     Time_Min_2 = Time_Min_2 - 1;
     if(Time_Min_2 < 9){ Time_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Time_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  
  
  }
  
  
