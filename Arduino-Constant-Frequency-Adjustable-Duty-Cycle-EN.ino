//MK-EN-This code is coded for Buck-Boost Converter. Can be customize for your project 


const int pot_pin = A0;           //Adjustable Resistor Voltage pin 
const int feedback = A1;          //Output Feed back Voltage pin
int vout = 0;                     //Vout
int vpot = 0;                     //V-adj-res
int duty = 1;                     //duty
int i=1;                          //For Slow Start
int down=0;                       //Avoid Valley Current
int memory=0;                     //For The Duty

void setup() {
  cli();                          // stop interrupts
  TCCR1A = 0xA2;                  // Register -A2 Noninvert-F2 invert
  TCCR1B = 0x19;                  // 
  TIMSK1 = 0x01 ;                 // overflow interrupt
  ICR1  = 249 ;                   // 31.25us cycle time, 32kHz PWM but 64kHz drive pulses (differential)--Frequency Formula: 16000000 / (2 * ICR1) 
  OCR1A = 1 ;                     // example U drive-Duty
  OCR1B = 248 ;                   // (1-duty)
  GTCCR = 0x83 ;                  // clear and halt prescalers
  TCNT1 = 0xFFFF ;                // synchronize counters exactly.
  GTCCR = 0x00 ;                  // allow prescalers to fly
  sei();                          // allow interrupts
  pinMode(pot_pin, INPUT);
  pinMode(feedback, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);

}

ISR(TIMER1_OVF_vect){
  
 OCR1A=duty;                    //can be delete ISR Function and can be change in loop. I did like this. Cause i want to avoid valley Current
 OCR1B=249-duty;
 
    if(vout>vpot){              //Avoid Valley Current 
      down=down+1;
      if(down==20){
        downduty();
      }
    }
}

                                  //customize for your project from here 

void loop(){
  
    readpot();                    //read adjustable resistor voltage
    
    while(i==1){                  //Slow Start
      
      for(i=1;i<25;i++){
        duty=duty+1; 
        delay(4); 
      }
    }
    
    readpot();
    
    if(vout<vpot){              //BOOST-Function

      while(vout<vpot){           //Boost-read-Boost
        
        duty=duty+1;
        detduty();
        delay(4+duty/100);
        readpot();
        
      }
    }
    
    if(vout > vpot){         //BUCK
      
      if(vout>vpot+200){
         memory=duty;
         duty=0;
         OCR1A=duty;                    //Avoid overvoltage
         OCR1B=249-duty;
         delayMicroseconds(100);
         duty=memory-round((vout-vpot)/2);
      }
      readpot();
      while(vout>vpot){           //Buck-read-Buck
        
        
        duty=duty-2;
        detduty();
        delay(4+duty/100);
        readpot();
       
      }
      
    }

    delay(1);                      //Delay 
    
}

void detduty(){
  
    if(duty>210){
      duty=210;                     //max duty
    }
    else if(duty<1){
      duty=1;                      //min duty
    }
}

void readpot(){
    
    vpot = analogRead(pot_pin);     //adjustable resistance voltage to vpot
    
    vout = analogRead(feedback);   //feedback to vout
    
    if(vpot>780){                   //if output voltage > 36Volts  dont be increase 
      vpot=780;
    }
    
}


void downduty(){                 //Avoid Valley Current

  memory=duty;
  duty=1;
  OCR1A=duty;                    
  OCR1B=249-duty;
  delayMicroseconds(100);
  duty=memory;
  down=0;
  
}
