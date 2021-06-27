// Ska försöka göra en funktion som skriver pwm och sparar skrivvärdet

int voltPin = A2; //pin som läser av poten
int readVal;
int i; //skrivvärdet för PWM

//Statemachine led var
int state_prev_led = -1; //Starta i neutral
int state_led = -1;
long prev_v_i = 0;
long prev_r_i = 0;
long prev_g_i = 0;
long prev_b_i = 0;
bool write_v = false;
bool write_r = false;
bool write_g = false;
bool write_b = false;
int upphamtning_spel = 1;

int led_r = 6; //pin för röd led
int led_g = 5; //pin för grön led
int led_b = 3; //pin för blå led
int led_v = 9; //pin för vit led

//Statemachine fav var
int state_prev_fav = 1; //Startar med favoritläge istället
int state_fav = 1;
unsigned long t_fav = 0;
unsigned long t_0_fav = 0;
long select_delay = 200;
int short_fade_delay = 20;
int long_fade_delay = 60;
bool delay_criterion = false;
bool b = true;
long random_number;
int ir;
int j;
int k;

//Funktionsvariabler
// int write_pin;
// long pwm_value;


//Switchvariabler
int state_s1 = 0;
int state_prev_s1 = 0;
int pin_s1 = 2; //pin för switch
int val_s1 = 0;
unsigned long t_s1 = 0;
unsigned long t_0_s1 = 0;
unsigned long bounce_delay_s1 = 5;
long long_hold = 750;

//Funktioner
void check_i() {
  readVal = analogRead(voltPin);
  i = map (readVal, 0, 1023, 0, 255);
}

void write_pwm(int pin_output, long pwm_value) {
  analogWrite (pin_output, pwm_value);
  if (pin_output == led_v) { //vit
    prev_v_i = pwm_value;
    write_v = false; //ser till att den inte skriver i direkt om den hoppar till färgblandare
  }
  if (pin_output == led_r) { //röd
    prev_r_i = pwm_value;
    write_r = false;
  }
  if (pin_output == led_g) { //grön
    prev_g_i = pwm_value;
    write_g = false;
  }
  if (pin_output == led_b) { //blå
    prev_b_i = pwm_value;
    write_b = false;
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  SM_s1(); //Kallar switch statemachine
  SM_LED_write(); //Kallar LED statemachine
  SM_fav();

  // POT
  readVal = analogRead(voltPin); //läser värdet på poten
  i = map (readVal, 0, 1023, 0, 255); //omvandlar värdet till något jag kan använda som output

  //Styrning
  if (state_s1 == 4) {
    state_led++; // Vid knapptryck, state_led +
  }

  if (state_prev_led >= 4) {
    state_led = 0; //återställer statemachine till 0 när den når ett gränsvärde.
  }

  if (state_s1 == 6 && state_led >= 0) {
    state_led = -1;  //Sätt led SM till NEUTRAL vid långt intryck OM den var i ett annat läge innan.
    state_fav = 0;
  }
  if (state_led >= 0) {
    state_fav = -1; //Se till att den switchar mellan färgblandning och favorites
  }

  // I färgblandaren: tar snapshots av förra skrivvärdet (i) när den lämnar färgblandaren för en specifik färg. Sätter skriv-boolean false för att den inte ska köra direkt nästa gång
  // utan behöver sätta den till true först.
  if (state_prev_led != -1) {
    if (state_prev_led == 0 && state_s1 == 4) {
      prev_v_i = i;
      write_v = false;
      Serial.println("TAR ETT SNAPSHOT AV VIT i");
    }
  }
  if (state_prev_led == 1 && state_s1 == 4) {
    prev_r_i = i;
    write_r = false;
  }
  if (state_prev_led == 2 && state_s1 == 4) {
    prev_g_i = i;
    write_g = false;
  }
  if (state_prev_led == 3 && state_s1 == 4) {
    prev_b_i = i;
    write_b = false;
  }

  //Favourites styrning. Se timer-funktion nedan.
  if (delay_criterion == true) {
    if (state_fav >= 0 && i > 0 && i <= 20) {
      state_fav = 1;
    }
    if (state_fav >= 0 && i > 20 && i <= 40) {
      state_fav = 2;
    }
    if (state_fav >= 0 && i > 40 && i <= 60) {
      state_fav = 3;
    }
    if (state_fav >= 0 && i > 60 && i <= 80) {
      state_fav = 4;
    }
    if (state_fav >= 0 && i > 80 && i <= 100) {
      state_fav = 5;
    }
    if (state_fav >= 0 && i > 100 && i <= 120) {
      state_fav = 6;
    }
    if (state_fav >= 0 && i > 120 && i <= 140) {
      state_fav = 7;
    }
    if (state_fav >= 0 && i > 140 && i <= 160) {
      state_fav = 8;
    }
    if (state_fav >= 0 && i > 160 && i <= 180) {
      state_fav = 9;
    }
    if (state_fav >= 0 && i > 180 && i <= 200) {
      state_fav = 10;
    }
    if (state_fav >= 0 && i > 200 && i <= 220) {
      state_fav = 11;
    }
    if (state_fav >= 0 && i > 220 && i <= 240) {
      state_fav = 12;
    }
    if (state_fav >= 0 && i > 240 && i <= 255) {
      state_fav = 13;
    }
  }

  //Detta är ett delay criterion.
  if (state_fav != state_prev_fav) { //Starta en timer när favoritläge byts
    t_0_fav = millis();
  }
  if (state_fav >= 0) { //Ha igång en timer när favoritläget är igång.
    t_fav = millis();
  }

  if (t_fav - t_0_fav > select_delay) { //OM en viss tid har passerat sedan nytt favoritläge upptäktes, låt denna väljas (sker ovan)
    delay_criterion = true;
  }
  else {
    delay_criterion = false;
  }

  //DEBUGGING
  if (state_s1 == 4) {
    Serial.println("TRIGGERED!");
  }
  if (state_led != state_prev_led) {
    Serial.print("--------LED State = ");  //Endast konsollutskrivning
    Serial.println(state_led);
  }
  if (state_fav != state_prev_fav) {
    Serial.print("--------FAV State = ");  //Endast konsollutskrivning
    Serial.println(state_fav);
  }
  if (state_s1 != state_prev_s1) {
    Serial.println(state_s1);
  }
}//END VOIDLOOP

void SM_LED_write() { //LED OUTPUT STATEMACHINE. AKA. FÄRGBLANDAREN
  state_prev_led = state_led;

  switch (state_led) {

    case -1: //NEUTRAL
      break;

    case 0: //Skriv vit
      if (i < prev_v_i + upphamtning_spel && i > prev_v_i - upphamtning_spel) {
        write_v = true;
      }
      if (write_v == true) {
        analogWrite (led_v, i); //skriva PWM-värde
      }
      break;

    case 1: //Skriv röd
      if (i < prev_r_i + upphamtning_spel && i > prev_r_i - upphamtning_spel) {
        write_r = true;
      }
      if (write_r == true) {
        analogWrite (led_r, i); //skriva PWM-värde
      }
      break;

    case 2: //Skriv grön
      if (i < prev_g_i + upphamtning_spel && i > prev_g_i - upphamtning_spel) {
        write_g = true;
      }
      if (write_g == true) {
        analogWrite (led_g, i); //skriva PWM-värde
      }
      break;

    case 3: //Skriv blå
      if (i < prev_b_i + upphamtning_spel && i > prev_b_i - upphamtning_spel) {
        write_b = true;
      }
      if (write_b == true) {
        analogWrite (led_b, i); //skriva PWM-värde
      }
      break;

  }//END SWITCHCASE
}//END LED STATEMACHINE

void SM_fav() { //FAVORITLÄGE PRESETS
  state_prev_fav = state_fav;

  switch (state_fav) {
    case -1://NEUTRAL
      break;

    case 1: //FAV 1 Mysorange
      write_pwm(led_v, 10);
      write_pwm(led_r, 255);
      write_pwm(led_g, 35);
      write_pwm(led_b, 0);
      break;

    case 2: //FAV 2
      write_pwm(led_v, 0);
      b = true;

      if (b == true) {
        random_number = random(0, 256);
        check_i(); //Den här är till för att upptäcka bytt favorit-läge
        if (i < 20 || i > 40) {
          b = false;
          break;
        }

        //RED
        check_i();
        if (i < 20 || i > 40) {
          b = false;
          break;
        }
        else {
          for (ir; ir < random_number; ir++) {
            check_i();
            if (i < 20 || i > 40) {
              b = false;
              break;
            }
            write_pwm(led_r, ir);
            delay(short_fade_delay);
            Serial.println("Red"); Serial.print(ir);
          }
        }

        check_i();
        if (i < 20 || i > 40) {
          b = false;
          break;
        }
        else {
          for (ir; ir > random_number; ir--) {
            check_i();
            if (i < 20 || i > 40) {
              b = false;
              break;
            }
            write_pwm(led_r, ir);
            delay(short_fade_delay);
            Serial.println("Red"); Serial.print(ir);
          }
        }

        //GREEN
        check_i();
        if (i < 20 || i > 40) {
          b = false;
          break;
        }
        else {
          for (j; j < round(random_number * 0.1); j++) {
            check_i();
            if (i < 20 || i > 40) {
              b = false;
              break;
            }
            write_pwm(led_g, j);
            delay(short_fade_delay);
            Serial.println("Green"); Serial.print(j);
          }
        }

        check_i();
        if (i < 20 || i > 40) {
          b = false;
          break;
        }
        else {
          for (j; j > round(random_number * 0.1); j--) {
            check_i();
            if (i < 20 || i > 40) {
              b = false;
              break;
            }
            write_pwm(led_g, j);
            delay(short_fade_delay);
            Serial.println("Green"); Serial.print(j);
          }
        }

        //BLUE
        check_i();
        if (i < 20 || i > 40) {
          b = false;
          break;
        }
        else {
          for (k; k < round(random_number * 0.1); k++) {
            check_i();
            if (i < 20 || i > 40) {
              b = false;
              break;
            }
            write_pwm(led_b, k);
            delay(short_fade_delay);
            Serial.println("Blue"); Serial.print(k);
          }
        }

        check_i();
        if (i < 20 || i > 40) {
          b = false;
          break;
        }
        else {
          for (k; k > round(random_number * 0.1); k--) {
            check_i();
            if (i < 20 || i > 40) {
              b = false;
              break;
            }
            write_pwm(led_b, k);
            delay(short_fade_delay);
            Serial.println("Blue"); Serial.print(k);
          }
        }
      }
      break;


    case 3: //FAV 3
      write_pwm(led_v, 100);
      write_pwm(led_r, 255);
      write_pwm(led_g, 20);
      write_pwm(led_b, 50);
      break;

    case 4:
      write_pwm(led_v, 50);
      write_pwm(led_r, 225);
      write_pwm(led_g, 60);
      write_pwm(led_b, 0);
      break;

    case 5: //Simulerad eld
      write_pwm(led_b, 0);
      b = true;

      if (b == true) {
        for (ir = 225; ir < 255; ir++) { //Dimma upp röd från
          check_i();
          if (i < 80 || i > 100) { //Bryt loopen om poten är utanför favoritlägets område
            b = false;
            break;
          }
          write_pwm(led_r, ir);
          write_pwm(led_g, (ir-205)*0.4); //Skriv grön också, men nedskickade till lägre värden. 
          write_pwm(led_v, (ir-190)*0.3);
          float r = random(6,20)/10; //Försök att få den att fladdra lite med med extrema värden. 
          delay(random(12, 70)*r);
        }

        for (ir = 255; ir > 225; ir--) { //Dimma ned röd
          check_i();
          if (i < 80 || i > 100) {
            b = false;
            break;
          }
          write_pwm(led_r, ir);
          write_pwm(led_g, (ir-205)*0.4);
          write_pwm(led_v, (ir-190)*0.3);
          float r = random(6,20)/10; //Försök att få den att fladdra lite med med extrema värden. 
          delay(random(12, 120)*r);
        }

      } //Slut på skriv röd-loop
      break;

    case 6:
      write_pwm(led_v, 0);
      write_pwm(led_r, 0);
      write_pwm(led_g, 225);
      write_pwm(led_b, 160);
      break;

    case 7:
      write_pwm(led_v, 150);
      write_pwm(led_r, 0);
      write_pwm(led_g, 255);
      write_pwm(led_b, 160);
      break;

    case 8:
      write_pwm(led_v, 150);
      write_pwm(led_r, 160);
      write_pwm(led_g, 225);
      write_pwm(led_b, 160);
      break;

    case 9:
      write_pwm(led_v, 200);
      write_pwm(led_r, 225);
      write_pwm(led_g, 170);
      write_pwm(led_b, 160);
      break;

    case 10:
      write_pwm(led_v, 0);
      write_pwm(led_r, 225);
      write_pwm(led_g, 40);
      write_pwm(led_b, 0);
      break;

    case 11:
      analogWrite (led_v, 0);

      b = true;

      if (b == true) {
        random_number = random(0, 256);
        Serial.println("The random number is");
        Serial.println(random_number);
        check_i();
        if (i < 200 || i > 220) {
          b = false;
          break;
        }

        //RED
        check_i();
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (ir; ir < random_number; ir++) {
            check_i();
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
            analogWrite (led_r, ir);
            delay(short_fade_delay);
            Serial.println("Red"); Serial.print(ir);
          }
        }

        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (ir; ir > random_number; ir--) {
            check_i();
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
            analogWrite (led_r, ir);
            delay(short_fade_delay);
            Serial.println("Red"); Serial.print(ir);
          }
        }

        //GREEN
        check_i();
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (j; j < random_number; j++) {
            check_i();
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
            analogWrite (led_g, j);
            delay(short_fade_delay);
            Serial.println("Green"); Serial.print(j);
          }
        }

        check_i();
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (j; j > random_number; j--) {
            check_i();
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
            analogWrite (led_g, j);
            delay(short_fade_delay);
            Serial.println("Green"); Serial.print(j);
          }
        }

        //BLUE
        check_i();
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (k; k < random_number; k++) {
            check_i();
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
            analogWrite (led_b, k);
            delay(short_fade_delay);
            Serial.println("Blue"); Serial.print(k);
          }
        }

        check_i();
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (k; k > random_number; k--) {
            check_i();
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
            analogWrite (led_b, k);
            delay(short_fade_delay);
            Serial.println("Blue"); Serial.print(k);
          }
        }
      }
      break;

    case 13:
      analogWrite (led_v, 0);
      b = true;

      if (b == true) {
        random_number = random(0, 256);
        if (i < 240) {
          b = false;
          break;
        }

        //RED
        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (ir; ir < random_number; ir++) {
            analogWrite (led_r, ir);
            delay(long_fade_delay);
            Serial.println(ir);
            readVal = analogRead(voltPin);
            i = map (readVal, 0, 1023, 0, 255);
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
          }
        }

        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (ir; ir > random_number; ir--) {
            analogWrite (led_r, ir);
            delay(long_fade_delay);
            Serial.println(ir);
            readVal = analogRead(voltPin);
            i = map (readVal, 0, 1023, 0, 255);
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
          }
        }

        //GREEN
        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (j; j < random_number; j++) {
            analogWrite (led_g, j);
            delay(long_fade_delay);
            Serial.println(j);
            readVal = analogRead(voltPin);
            i = map (readVal, 0, 1023, 0, 255);
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
          }
        }

        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (j; j > random_number; j--) {
            analogWrite (led_g, j);
            delay(long_fade_delay);
            Serial.println(j);
            readVal = analogRead(voltPin);
            i = map (readVal, 0, 1023, 0, 255);
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
          }
        }

        //BLUE
        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (k; k < round(random_number * 0.3); k++) {
            analogWrite (led_b, k);
            delay(long_fade_delay);
            Serial.println(k);
            readVal = analogRead(voltPin);
            i = map (readVal, 0, 1023, 0, 255);
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
          }
        }

        readVal = analogRead(voltPin);
        i = map (readVal, 0, 1023, 0, 255);
        if (i < 200 || i > 220) {
          b = false;
          break;
        }
        else {
          for (k; k > round(random_number * 0.3); k--) {
            analogWrite (led_b, k);
            delay(long_fade_delay);
            Serial.println(k);
            readVal = analogRead(voltPin);
            i = map (readVal, 0, 1023, 0, 255);
            if (i < 200 || i > 220) {
              b = false;
              break;
            }
          }
        }
      }
      break;

  }//END SWITCHCASE
}//END STATEMACHINE FAVORITES


void SM_s1() { //*SWITCH STATEMACHINE*
  state_prev_s1 = state_s1;

  switch (state_s1) {

    case 0: //RESET
      state_s1 = 1;
      break;

    case 1: //START
      val_s1 = digitalRead(pin_s1);
      if (val_s1 == HIGH) {
        state_s1 = 2;
      }
      break;

    case 2: //KÖR TIMER
      t_0_s1 = millis();
      state_s1 = 3;
      break;

    case 3: //VÄNTA
      val_s1 = digitalRead(pin_s1);
      t_s1 = millis();
      if (val_s1 == LOW) {
        state_s1 = 0;
      }
      if (t_s1 - t_0_s1 > bounce_delay_s1) {
        state_s1 = 5;
      }
      break;

    case 4: //TRIGGERED
      state_s1 = 0;

      break;

    case 5: //ARMED + CHECK FOR LONG HOLD
      val_s1 = digitalRead(pin_s1);
      t_s1 = millis();
      if (val_s1 == LOW) {
        state_s1 = 4;
      }
      if (t_s1 - t_0_s1 > long_hold) {
        state_s1 = 6;
      }
      break;

    case 6: //LONG HOLD DETECTED
      state_s1 = 7;
      break;

    case 7: //LONG HOLD WAIT FOR LOW
      val_s1 = digitalRead(pin_s1);
      if (val_s1 == LOW) {
        state_s1 = 0;
      }
      break;

  }//end switchcase
}//end switch statemachine
