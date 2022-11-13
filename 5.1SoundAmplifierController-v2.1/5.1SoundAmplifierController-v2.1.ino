// 5.1 Surround System
// by Ui Tech <https://uitech-blog.blogspot.com>

#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <IRremote.h>
#define PT2323_address 0b1001010
#define PT2322_address 0b1000100

#define btn_delay 300

#define sw01 9    // SW
#define sw02 11    // DT
#define sw03 10    // CLK
#define sw04 A0   // Input
#define sw05 A1   // Mute
#define sw06 A2   // Power
#define sw07 13   // Out

// IR HEX code
#define ir_power 0x807F827D     // IR power ON/OFF
#define ir_mute 0x807F42BD      // IR mute
#define ir_in 0x807F629D        // IR input select
#define ir_vol_i 0x807F906F     // IR vol++
#define ir_vol_d 0x807FA05F     // IR vol-- 
#define ir_sub_i 0x807FD02F     // IR sub++
#define ir_sub_d 0x807FE01F     // IR sub-- 
#define ir_fl_i 0x807F40BF      // IR fl++
#define ir_fl_d 0x807FC03F      // IR fl-- 
#define ir_fr_i 0x807F00FF      // IR fr++
#define ir_fr_d 0x807F807F      // IR fr--
#define ir_cn_i 0x807F50AF      // IR cn++
#define ir_cn_d 0x807F609F      // IR cn--
#define ir_sl_i 0x807F48B7      // IR sl++
#define ir_sl_d 0x807FC837      // IR sl--
#define ir_sr_i 0x807F08F7      // IR sr++
#define ir_sr_d 0x807F8877      // IR sr--
#define ir_bass_i 0x807F8A75    // IR bass++
#define ir_bass_d 0x807F4AB5    // IR bass--
#define ir_mid_i 0x807F6897     // IR mid++
#define ir_mid_d 0x807FE817     // IR mid--
#define ir_treb_i 0x807FAA55    // IR treb++
#define ir_treb_d 0x807F6A95    // IR treb--
#define ir_sp_mode 0x807F0AF5   // IR speaker mode change
#define ir_sou_mode 0x807FA857  // IR sound mode change

IRrecv irrecv(8);
decode_results results;

byte custom_num[8][8] = {
  { B00111,B01111,B11111,B11111,B11111,B11111,B11111,B11111 },
  { B11111,B11111,B11111,B00000,B00000,B00000,B00000,B00000 },
  { B11100,B11110,B11111,B11111,B11111,B11111,B11111,B11111 },
  { B11111,B11111,B11111,B11111,B11111,B11111,B01111,B00111 },
  { B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111 },
  { B11111,B11111,B11111,B11111,B11111,B11111,B11110,B11100 },
  { B11111,B11111,B11111,B00000,B00000,B00000,B11111,B11111 },
  { B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111 }};
    
const int digit_width = 3;
                                           //  0      1       2      3      4        5      6      7        8      9
const char custom_num_top[10][digit_width]={ 0,1,2, 1,2,32, 6,6,2, 6,6,2, 3,4,7,   7,6,6, 0,6,6, 1,1,2,   0,6,2, 0,6,2};
const char custom_num_bot[10][digit_width]={ 3,4,5, 4,7,4,  7,4,4, 4,4,5, 32,32,7, 4,4,5, 3,4,5, 32,32,7, 3,4,5, 4,4,5};

byte arrow_left[8] = {B00100,B01100,B11100,B01100,B00100,B00000,B00000,B00000};
byte arrow_right[8] = {B00100,B00110,B00111,B00110,B00100,B00000,B00000,B00000};
byte arrow_bottom[8] = {B00100,B00100,B00100,B11111,B01110,B00100,B00000,B00000};

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // RS,E,D4,D5,D6,D7
unsigned long time;
int in, vol, bass, mid, treb, sub, fl, cn, fr, sl, sr, mute, ch_mute, return_d, surr, mix, vol_10, vol_1, a, b, mute_sel, effect_sel, tone_sel, sound_mode, speaker_mode, sub_menu, menu_sel, power, vol_on;
int menu, sub_menu_sel, sub_menu_item, sub_menu_item_sel;
int sub_v, sub_10, sub_1, ir_on, ir_menu;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  irrecv.enableIRIn();
  
  pinMode(sw01, INPUT); // SW
  pinMode(sw02, INPUT); // DT
  pinMode(sw03, INPUT); // CLK
  pinMode(sw04, INPUT); // Input
  pinMode(sw05, INPUT); // Mute
  pinMode(sw06, INPUT); // Power
  pinMode(sw07, OUTPUT); // Out

  lcd.begin(16, 2);

  power = 0;
  eeprom_read();
  power_up();
}

void loop() {
  lcd_update();
  eeprom_update();
  ir_control();
  return_delay();
  
  if(menu == 0 || menu == 2) {
    custom_num_shape();
  } else {
    custom_shape();
  }

  //power -------------------------------------------------//
  if (Serial.read() == 49) {
    Serial.println("power");
    power++;
    if (power > 1) {
      power = 0;
    }
    power_up();
    in++;
      set_in();
    delay(btn_delay);
  }
  
  
    //select input -------------------------------------------------//
    if (Serial.read() == 50) {
      Serial.println("in");
      in++;
      set_in();
      delay(btn_delay);
    }
    
    //select menu -------------------------------------------------//
    if (digitalRead(sw01) == LOW && menu_sel == 0 && vol_on == 0) {
      menu++;
      if (menu > 4) {
        menu = 0;
      }
      btn_cl();
      lcd.clear();
    }
    
    //mute -------------------------------------------------//
    if (Serial.read() == 51) {
      Serial.println("mute");
      mute++;
      set_mute();
      delay(btn_delay);
      if (menu != 0) {
        lcd.clear();
        menu = 0;
      }
      if(mute == 1) {
        vol_on = 1;
      } else {
        vol_on = 0;
      }
    }
  

  //volume -------------------------------------------------//
  if (menu == 0) {
    if (digitalRead(sw02) == LOW && vol_on == 0) {
      vol++;
      set_vol();
      delay(btn_delay);
    }
    if (digitalRead(sw03) == LOW && vol_on == 0) {
      vol--;
      set_vol();
      delay(btn_delay);
    }
    menu_sel = 0;
    sub_menu = 0;
    sub_menu_item = 0;
    sub_menu_item_sel = 0;
  }

  //menu 1 -------------------------------------------------//
  if (menu == 1) {
    set_sub_menu();
    switch(sub_menu){
      //bass -------------------------------------------------//
      case 1:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          bass++;
          set_bass();
          sound_mode = 0;
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          bass--;
          set_bass();
          sound_mode = 0;
          btn_cl();
        }
        break;

      //mid -------------------------------------------------//  
      case 2:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          mid++;
          set_mid();
          sound_mode = 0;
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          mid--;
          set_mid();
          sound_mode = 0;
          btn_cl();
        }
        break;

      //treb -------------------------------------------------//  
      case 3:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          treb++;
          set_treb();
          sound_mode = 0;
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          treb--;
          set_treb();
          sound_mode = 0;
          btn_cl();
        }
        break;
        
      case 4:
        sub_menu = 0;
        break;
    }
  }

  //menu 2 -------------------------------------------------//
  if (menu == 2) {
    if (digitalRead(sw02) == LOW) {
      sub++; 
      set_sub();
      btn_cl();
    }
    if (digitalRead(sw03) == LOW) {
      sub--;
      set_sub();
      btn_cl();
    }
  }
  
  //menu 3 -------------------------------------------------//
  if (menu == 3) {  
    set_sub_menu();  
    switch(sub_menu){
      //FL -------------------------------------------------//
      case 1:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          fl++; 
          set_fl();
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          fl--;
          set_fl();
          btn_cl();
        }
        break;

      //FR -------------------------------------------------//  
      case 2:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          fr++; 
          set_fr();
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          fr--;
          set_fr();
          btn_cl();
        }
        break;

      //CN -------------------------------------------------//
      case 3:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          cn++;
          set_cn();
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          cn--;
          set_cn();
          btn_cl();
        }
        break;

      //SL -------------------------------------------------//  
      case 4:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          sl++;
          set_sl();
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          sl--;
          set_sl();
          btn_cl();
        }
        break;

      //SR -------------------------------------------------//  
      case 5:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          sr++;
          set_sr();
          btn_cl();
        }
        if (digitalRead(sw03) == LOW && sub_menu_item == 1) {
          sr--;
          set_sr();
          btn_cl();
        }
        break;
        
      case 6:
        sub_menu = 0;
        break;
    }
  }
  
  //menu 4 -------------------------------------------------//
  if (menu == 4) {
    set_sub_menu();
    switch(sub_menu){
      //surrond on off -------------------------------------------------//
      case 1:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          surr++;
          set_surr();
          btn_cl();
        }
        break;
        
      //speaker mode -------------------------------------------------//  
      case 2:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          speaker_mode++;
          set_speaker_mode();
          btn_cl();
        }
        break;
        
      //sound mode -------------------------------------------------//  
      case 3:
        if (digitalRead(sw02) == LOW && sub_menu_item == 1) {
          sound_mode++;
          set_sound_mode();
          btn_cl();
        }
        break;
        
      case 4:
        sub_menu = 0;
        break;
    }
  }
}

//eeprom --------------------------------------------------------------------------------//

void eeprom_update(){
  EEPROM.update(0, in);
  EEPROM.update(1, vol);
  EEPROM.update(2, bass);
  EEPROM.update(3, mid);
  EEPROM.update(4, treb);
  EEPROM.update(5, sub);
  EEPROM.update(6, fl);
  EEPROM.update(7, fr);
  EEPROM.update(8, cn);
  EEPROM.update(9, sl);
  EEPROM.update(10, sr);
  EEPROM.update(11, surr);
  EEPROM.update(12, mix);
  EEPROM.update(13, speaker_mode);
  EEPROM.update(14, sound_mode);
}

void eeprom_read(){
  in = EEPROM.read(0);
  vol = EEPROM.read(1); 
  bass = EEPROM.read(2);
  mid = EEPROM.read(3);
  treb = EEPROM.read(4);
  sub = EEPROM.read(5);
  fl = EEPROM.read(6);
  fr = EEPROM.read(7);
  cn = EEPROM.read(8);
  sl = EEPROM.read(9);
  sr = EEPROM.read(10);
  surr = EEPROM.read(11);
  mix = EEPROM.read(12);
  speaker_mode = EEPROM.read(13);
  sound_mode = EEPROM.read(14);
}

void btn_cl() {
  delay(btn_delay);
  time = millis();
  return_d = 1;
}
void ir_cl() {
  time = millis();
  return_d = 1;
}
void return_delay() {
  if (millis() - time > 5000 && return_d == 1 && menu != 0 && mute == 0) {
    menu = 0;
    return_d = 0;
    ir_menu = 0;
    lcd.clear();
  }
}

//power up --------------------------------------------------------------------------------//

void power_up() {
  if(power == 1) {
    digitalWrite(sw07, HIGH);
    mute = 1;
    lcd.clear();
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print("    Ui Tech     ");
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("   5.1 System   ");
    delay(1000);
    lcd.clear();
    mute = 0;
    pt2322_input_sw();
    pt2322_function(0, 1, 0);
    set_vol();
    set_bass();
    set_mid();
    set_treb();
    set_in();
    set_surr();
    set_mix();
    set_fl();
    set_fr();
    set_cn();
    set_sl();
    set_sr();
    set_mute();
    set_speaker_mode();
    set_sound_mode();
    delay(300);
    menu = 0;
    ir_on = 1;
    ir_menu = 0;
    vol_on = 0;
    
  } else {

    digitalWrite(sw07, LOW);
    mute = 1;
    set_mute();
    delay(100);
    menu = 100;
    ir_on = 0;
  }
}

//lcd --------------------------------------------------------------------------------//

void lcd_update(){
  lcd.setCursor(0, 0);
  switch(menu){
    case 0:
      //input -------------------------------------------------//
      lcd.setCursor(0, 0);
      if(in == 0) {
        lcd.print("USB");
      }
      if(in == 1) {
        lcd.print("BLU");
      }
      if(in == 2) {
        lcd.print("FM ");
      }
      if(in == 3) {
        lcd.print("AUX");
      }
      if(in == 4) {
        lcd.print("DVD");
      }

      //vol -------------------------------------------------//
      lcd.setCursor(0, 1);
      if(vol_on == 1) {
        a = 0;
        b = 0;
        lcd.print("MUT");
      } else {
        int x = vol - 29;
        a = x / 10;
        b = x - a * 10;
        lcd.print("VOL");
      }
      lcd.setCursor(4, 0);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_top[a][i]);
      
      lcd.setCursor(4, 1);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_bot[a][i]);
      
      lcd.setCursor(8, 0);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_top[b][i]);
      
      lcd.setCursor(8, 1);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_bot[b][i]);

      //speaker mode -------------------------------------------------//
      lcd.setCursor(13, 0);
      if(sound_mode == 0) {
        lcd.print("NOR");
      }
      if(sound_mode == 1) {
        lcd.print("ROC");
      }
      if(sound_mode == 2) {
        lcd.print("POP");
      }
      if(sound_mode == 3) {
        lcd.print("CLA");
      }

      //sound mode -------------------------------------------------//
      lcd.setCursor(13, 1);
      if(speaker_mode == 0) {
        lcd.print("5.1");
      }
      if(speaker_mode == 1) {
        lcd.print("2.1");
      }
      break;
      
    case 1:
      lcd.setCursor(0, 0);
      if(sub_menu == 0) {
        lcd.write(1);
      } else {
        lcd.print(" ");  
      }
      
      //bass -------------------------------------------------//
      lcd.setCursor(1, 0);
      if(sub_menu == 1 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 1) {
        lcd.write(3);  
      } else {
        lcd.print("Bass");  
      }
      lcd.setCursor(1, 1);
      if(bass < 10) {
        lcd.print("0");
      }
      lcd.print(bass);

      //mid -------------------------------------------------//
      lcd.setCursor(6, 0);
      if(sub_menu == 2 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 2) {
        lcd.write(3);  
      } else {
        lcd.print("Mid");  
      }
      lcd.setCursor(6, 1);
      if(mid < 10) {
        lcd.print("0");
      }
      lcd.print(mid);

      //treb -------------------------------------------------//
      lcd.setCursor(11, 0);
      if(sub_menu == 3 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 3) {
        lcd.write(3);  
      } else {
        lcd.print("Treb");  
      }
      lcd.setCursor(11, 1);
      if(treb < 10) {
        lcd.print("0");
      }
      lcd.print(treb);
      break;

    case 2:
      //SUB -------------------------------------------------//
      sub_10 = sub / 10;
      sub_1 = sub - sub_10 * 10;
      
      lcd.setCursor(0, 0);
      lcd.print("SUB");

      lcd.setCursor(4, 0);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_top[sub_10][i]);
      
      lcd.setCursor(4, 1);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_bot[sub_10][i]);
      
      lcd.setCursor(8, 0);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_top[sub_1][i]);
      
      lcd.setCursor(8, 1);
      for(int i=0; i < digit_width; i++)
      lcd.print(custom_num_bot[sub_1][i]);
      break;  
    case 3:
      lcd.setCursor(0, 0);
      if(sub_menu == 0) {
        lcd.write(1);
      } else {
        lcd.print(" ");  
      }
      
      //FL -------------------------------------------------//
      lcd.setCursor(1, 0);
      if(sub_menu == 1 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 1) {
        lcd.write(3);  
      } else {
        lcd.print("FL");  
      }
      lcd.setCursor(1, 1);
      if(fl < 10) {
        lcd.print("0");
      }
      lcd.print(fl);

      //FR -------------------------------------------------//
      lcd.setCursor(4, 0);
      if(sub_menu == 2 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 2) {
        lcd.write(3);  
      } else {
        lcd.print("FR");  
      }
      lcd.setCursor(4, 1);
      if(fr < 10) {
        lcd.print("0");
      }
      lcd.print(fr);

      //CN -------------------------------------------------//
      lcd.setCursor(7, 0);
      if(sub_menu == 3 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 3) {
        lcd.write(3);  
      } else {
        lcd.print("CN");  
      }
      lcd.setCursor(7, 1);
      if(cn < 10) {
        lcd.print("0");
      }
      lcd.print(cn);

      //SL -------------------------------------------------//
      lcd.setCursor(10, 0);
      if(sub_menu == 4 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 4) {
        lcd.write(3);  
      } else {
        lcd.print("SL");  
      }
      lcd.setCursor(10, 1);
      if(sl < 10) {
        lcd.print("0");
      }
      lcd.print(sl);

      //SR -------------------------------------------------//
      lcd.setCursor(13, 0);
      if(sub_menu == 5 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 5) {
        lcd.write(3);  
      } else {
        lcd.print("SR");  
      }
      lcd.setCursor(13, 1);
      if(sr < 10) {
        lcd.print("0");
      }
      lcd.print(sr);
      break;
      
    case 4:
      lcd.setCursor(0, 0);
      if(sub_menu == 0) {
        lcd.write(1);
      } else {
        lcd.print(" ");  
      }
      
      //surrond on off -------------------------------------------------//
      lcd.setCursor(1, 0);
      if(sub_menu == 1 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 1) {
        lcd.write(3);  
      } else {
        lcd.print("Surr");
      }
      lcd.setCursor(1, 1);
      if(surr == 0) {
        lcd.print("ON ");
      }
      if(surr == 1) {
        lcd.print("OFF");
      }

      //speaker mode -------------------------------------------------//
      lcd.setCursor(6, 0);
      if(sub_menu == 2 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 2) {
        lcd.write(3);  
      } else {
        lcd.print("Mode");  
      }
      lcd.setCursor(6, 1);
      if(speaker_mode == 0) {
        lcd.print("5.1");
      }
      if(speaker_mode == 1) {
        lcd.print("2.1");
      }

      //sound mode -------------------------------------------------//
      lcd.setCursor(11, 0);
      if(sub_menu == 3 && sub_menu_item_sel == 1) {
        lcd.write(2);
      } else if(sub_menu == 3) {
        lcd.write(3);  
      } else {
        lcd.print("Pro");  
      }
      lcd.setCursor(11, 1);
      if(sound_mode == 0) {
        lcd.print("NOR ");
      }
      if(sound_mode == 1) {
        lcd.print("ROCK");
      }
      if(sound_mode == 2) {
        lcd.print("POP ");
      }
      if(sound_mode == 3) {
        lcd.print("CLAS");
      }
      break;
    case 100:
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("    Stand by    ");
      break;
  }
}

//IR control --------------------------------------------------------------------------------//

void ir_control() {
  if (irrecv.decode(&results)) {
    switch (results.value) {
      //power -------------------------------------------------//
      case ir_power:
        power++;
        if (power > 1)
          power = 0;
        power_up();
        break;
    }
    if (ir_on == 1) {
      switch (results.value) {
        //mute -------------------------------------------------//
        case ir_mute:
          mute++;
          set_mute();
          ir_menu = 0;
          if (menu != 0) {
            lcd.clear();
            menu = 0;
          }
          if (mute == 1)
            vol_on = 1;
          else
            vol_on = 0;
          break;
        //select input -------------------------------------------------//
        case ir_in:
          in++;
          set_in();
          ir_cl();
          ir_menu = 0;
          break;
      }
    }
    if (ir_on == 1 && vol_on == 0) {
      switch (results.value) {
        //volume -------------------------------------------------//
        case ir_vol_i:
          vol++;
          set_vol();
          ir_cl();
          ir_menu = 0;
          break;
  
        case ir_vol_d:
          vol--;
          set_vol();
          ir_cl();
          ir_menu = 0;
          break; 
          
        //bass -------------------------------------------------//
        case ir_bass_i:
          bass++;
          set_bass();
          ir_cl();
          ir_menu = 1;
          sound_mode = 0;
          break;
  
        case ir_bass_d:
          bass--;
          set_bass();
          ir_cl();
          ir_menu = 1;
          sound_mode = 0;
          break; 

        //mid -------------------------------------------------//
        case ir_mid_i:
          mid++;
          set_mid();
          ir_cl();
          ir_menu = 1;
          sound_mode = 0;
          break;
  
        case ir_mid_d:
          mid--;
          set_mid();
          ir_cl();
          ir_menu = 1;
          sound_mode = 0;
          break; 

        //treb -------------------------------------------------//
        case ir_treb_i:
          treb++;
          set_treb();
          ir_cl();
          ir_menu = 1;
          sound_mode = 0;
          break;
  
        case ir_treb_d:
          treb--;
          set_treb();
          ir_cl();
          ir_menu = 1;
          sound_mode = 0;
          break;

        //sub -------------------------------------------------//
        case ir_sub_i:
          sub++;
          set_sub();
          ir_cl();
          ir_menu = 2;
          break;
  
        case ir_sub_d:
          sub--;
          set_sub();
          ir_cl();
          ir_menu = 2;
          break;

        //FL -------------------------------------------------//
        case ir_fl_i:
          fl++;
          set_fl();
          ir_cl();
          ir_menu = 3;
          break;
  
        case ir_fl_d:
          fl--;
          set_fl();
          ir_cl();
          ir_menu = 3;
          break;

        //FR -------------------------------------------------//
        case ir_fr_i:
          fr++;
          set_fr();
          ir_cl();
          ir_menu = 3;
          break;
  
        case ir_fr_d:
          fr--;
          set_fr();
          ir_cl();
          ir_menu = 3;
          break;

        //CN -------------------------------------------------//
        case ir_cn_i:
          cn++;
          set_cn();
          ir_cl();
          ir_menu = 3;
          break;
        
        case ir_cn_d:
          cn--;
          set_cn();
          ir_cl();
          ir_menu = 3;
          break;

        //SL -------------------------------------------------//
        case ir_sl_i:
          sl++;
          set_sl();
          ir_cl();
          ir_menu = 3;
          break;
  
        case ir_sl_d:
          sl--;
          set_sl();
          ir_cl();
          ir_menu = 3;
          break;

        //SR -------------------------------------------------//
        case ir_sr_i:
          sr++;
          set_sr();
          ir_cl();
          ir_menu = 3;
          break;
  
        case ir_sr_d:
          sr--;
          set_sr();
          ir_cl();
          ir_menu = 3;
          break;
          
        //speaker mode -------------------------------------------------//  
        case ir_sp_mode:
          speaker_mode++;
          set_speaker_mode();
          ir_cl();
          ir_menu = 0;
          break;
          
        //sound mode -------------------------------------------------//
        case ir_sou_mode:
          sound_mode++;
          set_sound_mode();
          ir_cl();
          ir_menu = 0;
          break;
      }
      if (ir_menu == 0 && menu != 0) {
        lcd.clear();
        menu = 0;
      }
      if (ir_menu == 1 && menu != 1) {
        lcd.clear();
        menu = 1;
      }
      if (ir_menu == 2 && menu != 2) {
        lcd.clear();
        menu = 2;
      }
      if (ir_menu == 3 && menu != 3) {
        lcd.clear();
        menu = 3;
      }
    }
    irrecv.resume();
  }
}

//custom shape --------------------------------------------------------------------------------//

void custom_num_shape() {
  for (int i = 0; i < 8; i++)
  lcd.createChar(i, custom_num[i]);
}

void custom_shape() {
  lcd.createChar(1, arrow_left);
  lcd.createChar(3, arrow_right);
  lcd.createChar(2, arrow_bottom);
}

//sub menu --------------------------------------------------------------------------------//

void set_sub_menu() {
  if (digitalRead(sw02) == LOW && sub_menu_sel == 0) {
    sub_menu++;
    btn_cl();
    lcd.clear();
  }
  if (digitalRead(sw03) == LOW && sub_menu_sel == 0) {
    sub_menu--;
    if (sub_menu < 0)
      sub_menu = 0;
    btn_cl();
    lcd.clear();
  }
  if (sub_menu != 0) {
    menu_sel = 1;
    if (digitalRead(sw01) == LOW && sub_menu_item_sel == 0 && menu_sel == 1) {
      sub_menu_sel = 1;
      sub_menu_item = 1;
      sub_menu_item_sel = 1;
      btn_cl();
      lcd.clear();
    }
    if (digitalRead(sw01) == LOW && sub_menu_item_sel == 1) {
      sub_menu_sel = 0;
      sub_menu_item = 0;
      sub_menu_item_sel = 0;
      btn_cl();
      lcd.clear();
    }
  } else if (sub_menu == 0) {
    menu_sel = 0;
    sub_menu_sel = 0;
  }
}

//speaker mode --------------------------------------------------------------------------------//

void set_speaker_mode() {
  if (speaker_mode > 1)
    speaker_mode = 0;
  switch (speaker_mode) {
    case 0:                     // 5.1 mode     
      ch_mute = 0;
      surr = 0;
      break;
    case 1:                     // 2.1 mode
      ch_mute = 1;
      surr = 1;
      break;
  }
  set_cn();
  set_sl();
  set_sr();
  set_surr();
}

//sound mode --------------------------------------------------------------------------------//

void set_sound_mode() {
  if (sound_mode > 3)
    sound_mode = 0;
  switch (sound_mode) {
    case 0:                     // Normal     
      bass = 7;
      mid = 7;
      treb = 7;
      break;
    case 1:                     // Rock
      bass = 11;
      mid = 5;
      treb = 12;
      break;
    case 2:                     // Pop
      bass = 4;
      mid = 10;
      treb = 5;
      break;
    case 3:                     // Classic
      bass = 7;
      mid = 7;
      treb = 5;
      break;
  }
  set_bass();
  set_mid();
  set_treb();
}

//pt2323 settings --------------------------------------------------------------------------------//

void set_in() {
  if (in > 4)
    in = 0;
  switch (in) {
    case 0:
      a = 0b11001011;
      break; // 1 input
    case 1:
      a = 0b11001010;
      break; // 2 input
    case 2:
      a = 0b11001001;
      break; // 3 input
    case 3:
      a = 0b11001000;
      break; // 4 input
    case 4:
      a = 0b11000111;
      break; // 6 CH input
  }
  pt2323_send(a);
}
void set_surr() {
  if (surr > 1)
    surr = 0;
  switch (surr) {
    case 0:
      a = 0b11010000;
      break; // Surround ON
    case 1:
      a = 0b11010001;
      break; // Surround OFF
  }
  pt2323_send(a);
}
void set_mix() {
  if (mix > 1)
    mix = 0;
  switch (mix) {
    case 0:
      a = 0b10010000;
      break; // 0dB setup
    case 1:
      a = 0b10010001;
      break; // +6dB setup
  }
  pt2323_send(a);
}
void set_mute() {
  if (mute > 1)
    mute = 0;
  switch (mute) {
    case 0:
      a = 0b11111110;
      break; // All CH mute disabled
    case 1:
      a = 0b11111111;
      break; // All CH mute
  }
  pt2323_send(a);
}

//pt2322 settings --------------------------------------------------------------------------------//

void pt2322_function(int pt2322_mute, int effect, int tone_ctrl) {
  switch (pt2322_mute) {
    case 0:
      mute_sel = 0b00000000;
      break;
    case 1:
      mute_sel = 0b00001000;
      break;
  }
  switch (effect) {
    case 0:
      effect_sel = 0b00000000;
      break;
    case 1:
      effect_sel = 0b00000100;
      break;
  }
  switch (tone_ctrl) {
    case 0:
      tone_sel = 0b00000000;
      break;
    case 1:
      tone_sel = 0b00000010;
      break;
  }
  pt2322_send(0b01110000 + mute_sel + effect_sel + tone_sel);
}

void set_vol() {
  if (vol > 79)
    vol = 79;
  if (vol < 29)
    vol = 29;
  if (vol == 29)
    mute = 1;
  else
    mute = 0;
  set_mute();
  int c = 79-vol;
  vol_10 = c/10;
  vol_1 = c-vol_10*10;
  a = vol_10+0b11100000;
  b = vol_1+0b11010000;
  Wire.beginTransmission(PT2322_address);
  Wire.write(a);
  Wire.write(b);
  Wire.endTransmission();
}

void set_bass() {
  if (bass > 15)
    bass = 15;
  if (bass < 0)
    bass = 0;
  a = bass; 
  if(bass > 7)
    a = 23-bass;
  pt2322_send(0b10010000+a);
}

void set_mid() {
  if (mid > 15)
    mid = 15;
  if (mid < 0)
    mid = 0;
  a = mid; 
  if(mid > 7)
    a = 23-mid;
  pt2322_send(0b10100000+a);
}

void set_treb() {
  if (treb > 15)
    treb = 15;
  if (treb < 0)
    treb = 0;
  a = treb; 
  if(treb > 7)
    a = 23-treb;
  pt2322_send(0b10110000+a);
}

void set_sub() {
  if (sub > 15)
    sub = 15;
  if (sub < 0)
    sub = 0;
  int c = 15-sub;
  pt2322_send(0b01100000+c);
}

void set_fl() {
  if (fl > 15)
    fl = 15;
  if (fl < 0)
    fl = 0;
  int c = 15-fl;
  pt2322_send(0b00010000+c);
}

void set_fr() {
  if (fr > 15)
    fr = 15;
  if (fr < 0)
    fr = 0;
  int c = 15 - fr;
  pt2322_send(0b00100000+c);
}

void set_cn() {
  if (cn > 15)
    cn = 15;
  if (cn < 0)
    cn = 0;
  int c = 15 - cn;
  switch (ch_mute) {
    case 0:
      a = 0b11110100;
      break; // CN mute disabled
    case 1:
      a = 0b11110101;
      break; // CN mute
  }
  pt2323_send(a);
  pt2322_send(0b00110000+c);
}

void set_sl() {
  if (sl > 15)
    sl = 15;
  if (sl < 0)
    sl = 0;
  int c = 15 - sl;
  switch(ch_mute) {
    case 0:
      a = 0b11111000;
      break; // SL mute disabled
    case 1:
      a = 0b11111001;
      break; // SL mute
  }
  pt2323_send(a);
  pt2322_send(0b01000000+c);
}

void set_sr() {
  if (sr > 15)
    sr = 15;
  if (sr < 0)
    sr = 0;
  int c = 15-sr;
  switch(ch_mute) {
    case 0:
      a = 0b11111010;
      break; // SR mute disabled
    case 1:
      a = 0b11111011;
      break; // SR mute
  }
  pt2323_send(a);
  pt2322_send(0b01010000+c);
}

//pt2323 & pt2322 send --------------------------------------------------------------------------------//

void pt2323_send(char c) {
  Wire.beginTransmission(PT2323_address);
  Wire.write(c);
  Wire.endTransmission();
}
void pt2322_input_sw() {
  Wire.beginTransmission(PT2322_address);
  Wire.write(0b11000111);
  Wire.endTransmission();
}
void pt2322_send(char c) {
  Wire.beginTransmission(PT2322_address);
  Wire.write(c);
  Wire.endTransmission();
}

//end code
