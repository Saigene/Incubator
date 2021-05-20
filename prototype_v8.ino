#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS3231.h>
#include <EEPROM.h>

#define Type DHT11
#define ONE_WIRE_BUS 2                       //temperature sensor
#define sensehumidity 3                      //humidity sensor
#define pinLED A2                            //status led
#define buttonset 7                          //button set
#define buttonnext 6                         //button next, +  
#define buttonprev 5                         //button pre, -
#define buttonback 4                         //button back, save, display setting
#define bulb 8                               // bulb relay 
#define seal A3                              // servo

unsigned long led_dalay;
bool stateLED=true;
float tempC;
int humidity;
//float value_1 = 63972.5;
float value_1 = 65535 - ((16000000 * .100)/1024);

//==============Set Varialble====
//int high_limit_humid=80;
//int low_limit_humid=70;
//int high_limit_temp=37;
//int low_limit_temp=35;
//
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

int High_limit_temp=0, 
    Low_limit_temp=0;

int High_limit_humid, 
    Low_limit_humid, 
    High_limit_humid_1=0, 
    High_limit_humid_2=0, 
    Low_limit_humid_1=0, 
    Low_limit_humid_2=0;

int Turner_Hour, 
    Turner_Min, 
    Turner_Hour_1=0, 
    Turner_Hour_2=0, 
    Turner_Min_1=0, 
    Turner_Min_2=0;

int Day_value=1;

int Time_Hour, 
    Time_Min, 
    Time_Hour_1=0, 
    Time_Hour_2=0, 
    Time_Min_1=0, 
    Time_Min_2=0;
    
int humid_select=1, humid_add_minus=0;
int turner_select=1, turner_add_minus=0;
int temp_select=1, temp_add_minus=0;    
int time_select=1, time_add_minus=0;

int save_yes_no=0;

String next_prev_menu="defualt";
String display_screen="main";
String display_menu="default";
String display_lcd="default";

int menu=0;

int data_temp_high = 37;
int data_temp_low = 36;
int data_humid_high = 80;
int data_humid_low = 66;
int data_turner_hr = 5;
int data_turner_min = 0;
int data_day_value = 21;
int data_time_hr = 1;
int data_time_min = 30;

int read_data_temp_high;
int read_data_temp_low;
int read_data_humid_high;
int read_data_humid_low;
int read_data_turner_hr;
int read_data_turner_min;
int read_data_day_value;
int read_day_left;
//int read_data_time_hr;
//int read_data_time_min;

boolean display_settings = false;

int capture_hour_now;
int capture_min_now;
int capture_sec_now;
int day = 0;
int state_time_now;
int last_state_time_now;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT HT(sensehumidity,Type);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DS3231  rtc(SDA, SCL);
Time t;

void setup() {
  // put your setup code here, to run once:
lcd.begin();
welcomescreen();
HT.begin();
rtc.begin();
sensors.begin();

pinMode(pinLED, OUTPUT);
pinMode(seal, OUTPUT);
pinMode(bulb, OUTPUT);
pinMode(buttonset, INPUT);
pinMode(buttonnext, INPUT);
pinMode(buttonprev, INPUT);
pinMode(buttonback, INPUT);
Serial.begin(9600);

loadingscreen();
extract_digit();
 
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

}

void extract_digit(){ 
    High_limit_temp = read_data_temp_high; 
    Low_limit_temp = read_data_temp_low;

    High_limit_humid_1 = (read_data_humid_high / 10u) % 10;
    High_limit_humid_2 = (read_data_humid_high / 1u) % 10;
    Low_limit_humid_1 = (read_data_humid_low / 10u) % 10;
    Low_limit_humid_2 = (read_data_humid_low / 1u) % 10;

    Turner_Hour_1 = (read_data_turner_hr / 10u) % 10; 
    Turner_Hour_2 = (read_data_turner_hr / 1u) % 10; 
    Turner_Min_1 = (read_data_turner_min / 10u) % 10; 
    Turner_Min_2 = (read_data_turner_min / 1u) % 10; 

    Day_value = read_data_day_value;
 }

void loop() {
  t = rtc.getTime();
  if(display_screen == "main"){
    checkedHumidity();
    checkedTemperature();
    checkeDay();
    displaylcd();
    ledblinking();
  }

  if(menu == 1 && btn_state_menu == 1){
      clearlcd();
    lcd.print("1.Set Temp:");
    lcd.setCursor(0,1);
    lcd.print("Set  <   > Start");
    btn_state_menu=0; 
  }else if(menu == 1 && btn_state_enter==1){
    clearlcd();
    lcd.print("Set Temp High:");
    
    if(High_limit_temp > 9){
      lcd.setCursor(0,1);
      lcd.print("High:" + String(High_limit_temp) + "  Low:" +String(Low_limit_temp));  
    }else{
      lcd.setCursor(0,1);
      lcd.print("High:" + String(High_limit_temp) + "   Low:" +String(Low_limit_temp));               
    }
    btn_state_enter=0;
    }

  if(menu == 2 && btn_state_menu == 1){
    clearlcd();
    lcd.print("5.Set Humid");
    lcd.setCursor(0,1);
    lcd.print("Set  <   > Start");
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
    clearlcd();
    lcd.print("4.Set Turner");
    lcd.setCursor(0,1);
    lcd.print("Set  <   > Start");
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
    clearlcd();
    lcd.print("3.Set Day");
    lcd.setCursor(0,1);
    lcd.print("Set  <   > Start");
    btn_state_menu=0;
  }else if(menu == 4 && btn_state_enter==1){
    clearlcd();
    lcd.print("Set: Day");
    lcd.setCursor(0,1);
    lcd.print("Day:" + String(Day_value));
    btn_state_enter=0;
  }
  
  if(menu == 5 && btn_state_menu == 1){
    clearlcd();
    lcd.print("2.Set Time");
    lcd.setCursor(0,1);
    lcd.print("Set  <   > Start");
    btn_state_menu=0;
  }else if(menu == 5 && btn_state_enter==1){    
    clearlcd();
    lcd.print("Set Time Hour:");
    lcd.setCursor(0,1);
    lcd.print("Hour:"+ String(Time_Hour_1) + String(Time_Hour_2) + " Min:" + String(Time_Min_1) + String(Time_Min_1));
    lcd.setCursor(5,1); lcd.blink(); 
    btn_state_enter=0;
    }

  if(display_settings){
      lcd.setCursor(0,0); lcd.print("=== Settings ===");
      lcd.setCursor(0,1); lcd.print("================");
      delay(2000);
      lcd.setCursor(0,0); lcd.print("Temp High:" + String(read_data_temp_high) + "    ");
      lcd.setCursor(0,1); lcd.print("Temp  Low:" + String(read_data_temp_low) + "    ");
      delay(2000);
      lcd.setCursor(0,0); lcd.print("Humid High:" + String(read_data_humid_high) + "    ");
      lcd.setCursor(0,1); lcd.print("Humid  Low:" + String(read_data_humid_low) + "    ");
      delay(2000);            
      lcd.setCursor(0,0); lcd.print("Turner Hour:" + String(read_data_turner_hr) + "    ");
      lcd.setCursor(0,1); lcd.print("Turner  Min:" + String(read_data_turner_min) + "    ");
      delay(2000);
      lcd.setCursor(0,0); lcd.print(String(read_data_day_value) + "Days            ");
      lcd.setCursor(0,1); lcd.print("TimeNow:" + String(rtc.getTimeStr()));
      delay(1500);
      display_settings=false;              
      clearlcd();
    }  

  checkedbutton();
}


void clearlcd(){
  lcd.clear();
  delay(200);
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
              if(temp_select == 1){
                temp_add_minus=1; temp_select=2;
                lcd.setCursor(9,0); lcd.print("High:");              
                }
              }
             if(menu == 2){
               if(humid_select == 1){
              humid_add_minus=1; humid_select=2;
              lcd.setCursor(0,0); lcd.print("Humidity High:");
              lcd.setCursor(5,1); lcd.blink();}             
              }

              if(menu == 3){
                  if(turner_select == 1){
                  turner_add_minus=1; turner_select=2;
                  lcd.setCursor(0,0); lcd.print("Set Turner Hour:");
                  lcd.setCursor(5,1); lcd.blink();} 
                }

              if(menu == 5){
                if(time_select == 1){
                time_add_minus=1; time_select=2;
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

            lcd.noBlink();
            Serial.println("Main");
            lcd.setCursor(0,0); lcd.print("=== Settings ===");
            lcd.setCursor(0,1); lcd.print("================");
            delay(2000);
            lcd.setCursor(0,0); lcd.print("Temp High:" + String(read_data_temp_high) + "    ");
            lcd.setCursor(0,1); lcd.print("Temp  Low:" + String(read_data_temp_low) + "    ");
            delay(2000);
            lcd.setCursor(0,0); lcd.print("Humid High:" + String(read_data_humid_high) + "    ");
            lcd.setCursor(0,1); lcd.print("Humid  Low:" + String(read_data_humid_low) + "    ");
            delay(2000);            
            lcd.setCursor(0,0); lcd.print("Turner Hour:" + String(read_data_turner_hr) + "    ");
            lcd.setCursor(0,1); lcd.print("Turner  Min:" + String(read_data_turner_min) + "    ");
            delay(2000);
            lcd.setCursor(0,0); lcd.print(String(read_data_day_value) + "Days            ");
            lcd.setCursor(0,1); lcd.print("TimeNow:" + String(rtc.getTimeStr()));
            delay(1500);
            
            display_screen="main";
            menu=0;              
            clearlcd();  
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
              if(temp_select == 1){
                temp_add_minus=1; temp_select=2;
                lcd.setCursor(9,0); lcd.print("High:");
             }
                
              else if(temp_select == 2){
                temp_add_minus=2; temp_select=1;
                lcd.setCursor(9,0);lcd.print("Low: ");
                }
             }
            if(menu == 2){
              if(humid_select == 1){
              humid_add_minus=1; humid_select=2;
              lcd.setCursor(0,0); lcd.print("Humidity High:");
              lcd.setCursor(5,1); lcd.blink();
              }
              else if(humid_select == 2){
              humid_add_minus=2;  humid_select=3;
              lcd.setCursor(0,0); lcd.print("Humidity High:");
              lcd.setCursor(6,1); lcd.blink();
              }            
              else if(humid_select == 3){
              humid_add_minus=3; humid_select=4;
              lcd.setCursor(0,0); lcd.print("Humidity Low:  ");
              lcd.setCursor(13,1); lcd.blink();
              }
              else if(humid_select == 4){
              humid_add_minus=4; humid_select=1;
              lcd.setCursor(0,0); lcd.print("Humidity Low:  ");
              lcd.setCursor(14,1); lcd.blink();
              }
           }
            if(menu == 3){
              if(turner_select == 1){
                  turner_add_minus=1; turner_select=2;
                  lcd.setCursor(0,0); lcd.print("Set Turner Hour:");
                  lcd.setCursor(5,1); lcd.blink();
                  }
              else if(turner_select == 2){
                  turner_add_minus=2; turner_select=3;
                  lcd.setCursor(0,0); lcd.print("Set Turner Hour:");
                  lcd.setCursor(6,1); lcd.blink();
                  }    
              else if(turner_select == 3){
                  turner_add_minus=3; turner_select=4;
                  lcd.setCursor(0,0); lcd.print("Set Turner Min:  ");
                  lcd.setCursor(12,1); lcd.blink();
                  }
              else if(turner_select == 4){
                  turner_add_minus=4; turner_select=1;
                  lcd.setCursor(0,0); lcd.print("Set Turner Min:  ");
                  lcd.setCursor(13,1); lcd.blink();
                  }    
            }
            if(menu == 4){Serial.println("move set day");}
            
            if(menu == 5){
              if(time_select == 1){
                  time_add_minus=1; time_select=2;
                  lcd.setCursor(0,0); lcd.print("Set Time Hour:");
                  lcd.setCursor(5,1); lcd.blink();
                  }
              else if(time_select == 2){
                  time_add_minus=2; time_select=3;
                  lcd.setCursor(0,0); lcd.print("Set Time Hour:");
                  lcd.setCursor(6,1); lcd.blink();
                  }    
              else if(time_select == 3){
                  time_add_minus=3; time_select=4;
                  lcd.setCursor(0,0); lcd.print("Set Time Min:  ");
                  lcd.setCursor(12,1); lcd.blink();
                  }
              else if(time_select == 4){
                  time_add_minus=4; time_select=1;
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
                            
              if(menu==1){Serial.println("Saving Temp");saving_yes_no();}
              if(menu==2){Serial.println("Saving Humid");saving_yes_no();}
              if(menu==3){Serial.println("Saving Turner");saving_yes_no();}
              if(menu==4){Serial.println("Saving Day");saving_yes_no();}
              if(menu==5){Serial.println("Saving Time");saving_yes_no();}
              lcd.noBlink();
            }
        }
      }
}

if(next_prev_menu == "back_save"){
     
     if((millis()-lastDebounceTime) > debounceDelay){
      if(reading_set != buttonState_set){
        buttonState_set = reading_set;
          if(buttonState_set == HIGH){
           
            if(save_yes_no == 1){
               Serial.println("Saving data");
               saving_data();
              btn_state_menu = 1;
              menu=btn_back_state;
              temp_add_minus == 0;
              next_prev_menu = "next_prev";
              display_screen="settings";
              save_yes_no=0;
              clearlcd();
              }
            if(save_yes_no == 2){
              btn_state_menu = 1;
              menu=btn_back_state;
              temp_add_minus == 0;
              next_prev_menu = "next_prev";
              display_screen="settings";
              save_yes_no=0;
              clearlcd();
              extract_digit();
              }
                         
          }
        }
      }

      if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_next != buttonState_next){
                buttonState_next = reading_next;
                if(buttonState_next == HIGH){
                Serial.println("Yes");
                  save_yes_no=1;
                lcd.setCursor(11,1);
                lcd.print(" ");
                lcd.setCursor(6,1);
                lcd.print(">");
                }
            }
         }

        if((millis()-lastDebounceTime) > debounceDelay){
            if(reading_prev != buttonState_prev){
                buttonState_prev = reading_prev;
                if(buttonState_prev == HIGH){
                  Serial.println("No");
                  save_yes_no=2;
                  lcd.setCursor(6,1);
                  lcd.print(" ");
                  lcd.setCursor(11,1);
                  lcd.print(">");
                }
            }
        }    
    }



  

          
lastButtonState_set = reading_set;
lastButtonState_next = reading_next;
lastButtonState_prev = reading_prev;    
lastButtonState_back = reading_back; 
  
}

void saving_yes_no(){
  clearlcd();
  lcd.setCursor(0,0);
  lcd.print("Do you want to");
  lcd.setCursor(0,1);
  lcd.print("Save? >Yes  No");
  next_prev_menu = "back_save";
  display_screen="default";
  save_yes_no=1;  
}

void saving_data(){
if(menu == 1){
  data_temp_high = High_limit_temp;
  data_temp_low = Low_limit_temp;
  EEPROM.write(0,data_temp_high);  //temp high
  EEPROM.write(1,data_temp_low);  //temp low
  read_data_temp_high = EEPROM.read(0);
  read_data_temp_low = EEPROM.read(1);
  delay(200);
  }
if(menu == 2){
  High_limit_humid = (10 * High_limit_humid_1) + High_limit_humid_2;
  Low_limit_humid = (10*Low_limit_humid_1) + Low_limit_humid_2;
  data_humid_high = High_limit_humid;        
  data_humid_low = Low_limit_humid;    
  EEPROM.write(2,data_humid_high);  //humid high
  EEPROM.write(3,data_humid_low);  //humid low  
  read_data_humid_high = EEPROM.read(2);
  read_data_humid_low = EEPROM.read(3);
 
  delay(200); 
  }
if(menu == 3){
    
  Turner_Hour = (10*Turner_Hour_1) + Turner_Hour_2;  
  Turner_Min = (10*Turner_Min_1) + Turner_Min_2;  
  data_turner_hr=Turner_Hour;
  data_turner_min=Turner_Min;;  
  EEPROM.write(4,data_turner_hr);  //turner hr
  EEPROM.write(5,data_turner_min);  //turner min 
  read_data_turner_hr = EEPROM.read(4);
  read_data_turner_min = EEPROM.read(5);
  delay(200); 
  }
if(menu == 4){
  data_day_value=Day_value;
  Serial.println("Saving Day:" + String(data_day_value));  
  EEPROM.write(6,data_day_value);  //Day
  read_data_day_value = EEPROM.read(6);
  capture_hour_now = t.hour;
  capture_min_now = t.min;
  capture_sec_now = t.sec;   
  delay(200);
  }
if(menu == 5){
  Time_Hour = (10*Time_Hour_1) + Time_Hour_2;
  Time_Min = (10*Time_Min_1) + Time_Min_2;  
  data_time_hr=Time_Hour;
  data_time_min=Time_Min;

//  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(Time_Hour, Time_Min, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(5, 8, 2021);   // Set the date to January 1st, 2014  
  }      
}

 



//==========================================================================Interrupt Button=====================================================================

ISR(TIMER1_OVF_vect)                    // interrupt service routine for overflow 
{                                   
    TCNT1 = value_1;                        //checked button if press.

  int reading_set=digitalRead(buttonset);
  int reading_back=digitalRead(buttonback);
  
  if(reading_set != lastButtonState_set){lastDebounceTime = millis();}
  if(reading_back != lastButtonState_back){lastDebounceTime = millis();}
  
    if(display_screen=="main"){
  
      if((millis()-lastDebounceTime) > debounceDelay){
          if(reading_set != buttonState_set){
            buttonState_set = reading_set;
              if(buttonState_set == HIGH){
                 btn_state_set = 1;btn_state_menu = 1;
                 display_screen="settings";
                 next_prev_menu = "next_prev";
                  Serial.println("Settings");
                  menu=1;
              }
           }
      }

      if((millis()-lastDebounceTime) > debounceDelay){
          if(reading_back != buttonState_back){
            buttonState_back = reading_back;
              if(buttonState_back == HIGH){
                display_settings = true;
                  
            }
          }
      }      
   }
         
lastButtonState_set = reading_set;
lastButtonState_back = reading_back;

}



//====================================================================checking Sensors===========================================================================

void checkedHumidity(){
  
    humidity=HT.readHumidity();   
    if(humidity >= read_data_humid_high){
      seal_humidity=1;
      digitalWrite(seal, HIGH);
      Serial.println( String(humidity)  + " " + String(read_data_humid_low));
      }
    else if(
      humidity <= read_data_humid_low){
        seal_humidity=0; 
        digitalWrite(seal, LOW);
        }
//
//    if(seal_humidity==1){Serial.print("Seal Open --  ");}             // set servo=1 open high humid
//    else if(seal_humidity==0){Serial.print("Seal Close  --  ");}     // set servo=2 close low humid
}

void checkedTemperature(){
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  if(tempC >= read_data_temp_high){bulb_status=0; digitalWrite(bulb, LOW);}
  else if(tempC <= read_data_temp_low){bulb_status=1; digitalWrite(bulb, HIGH);}
//  
//  if(bulb_status==0){Serial.println("Light off");}                     //set bulb off in high temp
//  else if(bulb_status==1){Serial.println("Light On");}                 //set bulb on in low temp
}

void checkeDay(){
// Serial.println("capture Hour:" + String(capture_hour_now) + " | Capture Min:" + String(capture_min_now));
// Serial.println("Now Hour:" + String(t.hour) + " | Now Min:" + String(t.min));
// if(t.hour == capture_hour_now && t.min == capture_min_now){
//  day = day + 1;
//  }

//  Serial.println("Capture Sec:" + String(capture_sec_now) + "Now Sec:" + String(t.sec));
  
  if(t.sec == capture_sec_now){
        state_time_now = 1;
      if(last_state_time_now != state_time_now){   // 0 1               
          delay(100);
          day = day + 1;
          EEPROM.write(7, day);
          Serial.println(day);
          last_state_time_now = state_time_now;   //last =  1
      } 
  }else{
      last_state_time_now = 0;
  }

  if(day == read_data_day_value){
      day=0;
    }

  
  

}
void displaylcd(){
lcd.setCursor(0,0);
lcd.print("T:" + String(tempC));
lcd.print((char) 223);
lcd.print("C");
lcd.print("  H:" + String(humidity) + "%");
lcd.setCursor(0,1);
lcd.print(rtc.getTimeStr());
lcd.print("  Day:");
if(day < 10){
    lcd.setCursor(14, 1); 
    lcd.print(day);
    lcd.setCursor(15,1); 
    lcd.print(" ");  
}else{
  lcd.setCursor(14, 1);
  lcd.print(String(day));}

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
  
 read_data_temp_high = EEPROM.read(0);
 read_data_temp_low = EEPROM.read(1);
 
 read_data_humid_high = EEPROM.read(2);
 read_data_humid_low = EEPROM.read(3);
 
 read_data_turner_hr = EEPROM.read(4);
 read_data_turner_min = EEPROM.read(5);
 
 read_data_day_value = EEPROM.read(6);
 day = EEPROM.read(7);

  
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
//      Serial.println("TempHigh:" + String(read_data_temp_high) + " TempLow:" + String(read_data_temp_low)
//                  + " | " + "HumidHigh:" + String(read_data_humid_high) + " HumidLow:" + String(read_data_humid_low)
//                  + " | " + "TurnerHr:" + String(read_data_turner_hr) + " TurnerMin:" + String( read_data_turner_min)
//                  + " | " + "Day:" + String(read_data_day_value));
                  
    }
}

void temperature_add(){
  Serial.println("add temp");
  if(temp_add_minus == 1){
    High_limit_temp = High_limit_temp + 1;
    if(High_limit_temp > 99){ High_limit_temp = 1;}    
     if(High_limit_temp < 10){
      lcd.setCursor(5,1); lcd.print(High_limit_temp);
      lcd.setCursor(6,1); lcd.print(" ");
    }else{
      lcd.setCursor(5,1); lcd.print(High_limit_temp);
      }
    }
  if(temp_add_minus == 2){
    Low_limit_temp = Low_limit_temp + 1;
    if(Low_limit_temp > 99){ Low_limit_temp = 1;}     
    if(Low_limit_temp < 10){
      lcd.setCursor(13,1); lcd.print(Low_limit_temp);
      lcd.setCursor(14,1); lcd.print(" ");
      }else{
        lcd.setCursor(13,1); lcd.print(Low_limit_temp);
        }
      }
}
  
void temperature_minus(){
  if(temp_add_minus == 1){
    High_limit_temp = High_limit_temp - 1;
    if(High_limit_temp < 1){ High_limit_temp = 99;}
    if(High_limit_temp < 10){
      lcd.setCursor(5,1); lcd.print(High_limit_temp);
      lcd.setCursor(6,1); lcd.print(" ");
    }else{
      lcd.setCursor(5,1); lcd.print(High_limit_temp);
      }    
    }
  if(temp_add_minus == 2){
    Low_limit_temp = Low_limit_temp - 1;
    if(Low_limit_temp < 1){ Low_limit_temp = 99;}
    if(Low_limit_temp < 10){
      lcd.setCursor(13,1); lcd.print(Low_limit_temp);
      lcd.setCursor(14,1); lcd.print(" ");
      }else{
        lcd.setCursor(13,1); lcd.print(Low_limit_temp);
        }  
    }  

}

void humidity_add(){
  if(humid_add_minus==1){
     High_limit_humid_1 = High_limit_humid_1 + 1;
     if(High_limit_humid_1 > 9){ High_limit_humid_1 = 0;}
     lcd.setCursor(5,1);  lcd.print(High_limit_humid_1);
     lcd.setCursor(5,1); lcd.blink();
  }
  if(humid_add_minus==2){
     High_limit_humid_2 = High_limit_humid_2 + 1;
     if(High_limit_humid_2 > 9){ High_limit_humid_2 = 0;}
     lcd.setCursor(6,1);  lcd.print(High_limit_humid_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(humid_add_minus==3){
     Low_limit_humid_1 = Low_limit_humid_1 + 1;
     if(Low_limit_humid_1 > 9){ Low_limit_humid_1 = 0;}
     lcd.setCursor(13,1);  lcd.print(Low_limit_humid_1);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  if(humid_add_minus==4){
     Low_limit_humid_2 = Low_limit_humid_2 + 1;
     if(Low_limit_humid_2 > 9){ Low_limit_humid_2 = 0;}
     lcd.setCursor(14,1);  lcd.print(Low_limit_humid_2);
     lcd.setCursor(14,1); lcd.blink(); 
  }
}  
void humidity_minus(){
  if(humid_add_minus==1){
     High_limit_humid_1 = High_limit_humid_1 - 1;
     if(High_limit_humid_1 <= 1){ High_limit_humid_1 = 1;}
     lcd.setCursor(5,1);  lcd.print(High_limit_humid_1);
     lcd.setCursor(5,1); lcd.blink();
  }
  if(humid_add_minus==2){
     High_limit_humid_2 = High_limit_humid_2 - 1;
     if(High_limit_humid_2 < 0){ High_limit_humid_2 = 0;}
     lcd.setCursor(6,1);  lcd.print(High_limit_humid_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(humid_add_minus==3){
     Low_limit_humid_1 = Low_limit_humid_1 - 1;
     if(Low_limit_humid_1 <= 1){ Low_limit_humid_1 = 1;}
     lcd.setCursor(13,1);  lcd.print(Low_limit_humid_1);
     lcd.setCursor(13,1); lcd.blink(); 
  }
  if(humid_add_minus==4){
     Low_limit_humid_2 = Low_limit_humid_2 - 1;
     if(Low_limit_humid_2 < 0 ){ Low_limit_humid_2 = 0;}
     lcd.setCursor(14,1);  lcd.print(Low_limit_humid_2);
     lcd.setCursor(14,1); lcd.blink(); 
  }
}

void turner_add(){
  if(turner_add_minus==1){
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
  if(turner_add_minus==2){
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
  if(turner_add_minus==3){
     Turner_Min_1 = Turner_Min_1 + 1;
     if(Turner_Min_1 > 5){ Turner_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Turner_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(turner_add_minus==4){
     Turner_Min_2 = Turner_Min_2 + 1;
     if(Turner_Min_2 > 9){ Turner_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Turner_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }  
}

void turner_minus(){
  if(turner_add_minus==1){
      Turner_Hour_1 = Turner_Hour_1 - 1;
      if(Turner_Hour_1 < 0){ Turner_Hour_1 = 0;} 
     lcd.setCursor(5,1);  lcd.print(Turner_Hour_1);
     lcd.setCursor(5,1); lcd.blink();
  }
  if(turner_add_minus==2){
     Turner_Hour_2 = Turner_Hour_2 - 1;
     if(Turner_Hour_2 < 0){ Turner_Hour_2 = 0;}
     lcd.setCursor(6,1);  lcd.print(Turner_Hour_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(turner_add_minus==3){
     Turner_Min_1 = Turner_Min_1 - 1;
     if(Turner_Min_1 < 0){ Turner_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Turner_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(turner_add_minus==4){
     Turner_Min_2 = Turner_Min_2 - 1;
     if(Turner_Min_2 < 0){ Turner_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Turner_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }
}   

void day_add(){
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
  if(time_add_minus==1){
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
  if(time_add_minus==2){
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
  if(time_add_minus==3){
     Time_Min_1 = Time_Min_1 + 1;
     if(Time_Min_1 > 5){ Time_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Time_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(time_add_minus==4){
     Time_Min_2 = Time_Min_2 + 1;
     if(Time_Min_2 > 9){ Time_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Time_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }  
  }
void time_minus(){
  if(time_add_minus==1){
       Time_Hour_1 = Time_Hour_1 - 1;
       if(Time_Hour_1 < 0){ Time_Hour_1 = 0;} 
     lcd.setCursor(5,1);  lcd.print(Time_Hour_1);
     lcd.setCursor(5,1); lcd.blink(); 
  } 
  if(time_add_minus==2){
    Time_Hour_2 = Time_Hour_2 - 1;
    if(Time_Hour_2 < 0){ Time_Hour_2 = 0;}         
     lcd.setCursor(6,1);  lcd.print(Time_Hour_2);
     lcd.setCursor(6,1); lcd.blink();  
  }
  if(time_add_minus==3){
     Time_Min_1 = Time_Min_1 - 1;
     if(Time_Min_1 < 0){ Time_Min_1 = 0;}
     lcd.setCursor(12,1);  lcd.print(Time_Min_1);
     lcd.setCursor(12,1); lcd.blink(); 
  }
   if(time_add_minus==4){
     Time_Min_2 = Time_Min_2 - 1;
     if(Time_Min_2 < 9){ Time_Min_2 = 0;}
     lcd.setCursor(13,1);  lcd.print(Time_Min_2);
     lcd.setCursor(13,1); lcd.blink(); 
  }
}


  
  
