//MK-TR


const int pot_pin = A0;           //potansiyometreden okunan degerin pini 
const int feedback = A1;          //cikistaki feedback'in 1/10 miktarindaki degeri
int vout = 0;                     //vout'u tanimladik
int vpot = 0;                     //vpot degerini tanimladik
int duty = 1;                     //duty'i tanimladik
int i=1;                          //i yumusak baslama icin
int down=0;                       //valley current engellemek icin
int memory=0;                     //duty'i hafizada tutmak icin

void setup() {
  cli();                          // stop interrupts----- interrupt'lari durdurduk
  TCCR1A = 0xA2;                  //registerleri belirledik--A2 olursa duz--F2 olursa tersleyen
  TCCR1B = 0x19;                  //registerleri belirledik
  TIMSK1 = 0x01 ;                 // overflow interrupt
  ICR1  = 249 ;                   // 31.25us cycle time, 32kHz PWM but 64kHz drive pulses (differential)----- frekansi ayarladik---Formul: 16000000 / (2 * ICR1) 
  OCR1A = 1 ;                     // example U drive--Doluluk orani(duty)
  OCR1B = 248 ;                   //Bosluk orani (1-duty)
  GTCCR = 0x83 ;                  // clear and halt prescalers
  TCNT1 = 0xFFFF ;                // synchronize counters exactly.
  GTCCR = 0x00 ;                  // allow prescalers to fly
  sei();                          // allow interrupts-----interrut'lari tekrardan baslattik
  pinMode(pot_pin, INPUT);
  pinMode(feedback, INPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);

}

ISR(TIMER1_OVF_vect){           //Bu kisimdan sonrasini kendi kodunuza gore duzenleyebilirsiniz.
  
 OCR1A=duty;                    //dutyi buradan ayarla bu kismi silebiliriz ama Silindiginde valley current'i kontrol etmemiz mumkun degil
 OCR1B=249-duty;
 
    if(vout>vpot){              //valley current engellemek icin
      down=down+1;
      if(down==20){
        downduty();
      }
    }
}



void loop(){
  
    oku();                        //potansiyometreden istenen gerilim ve cikistan alinan gerilimi okutuyoruz
    
    while(i==1){                  //Yumusak baslangic
      
      for(i=1;i<25;i++){
        duty=duty+1; 
        delay(4); 
      }
    }
    
    oku();
    
    if(vout<vpot){              //istenen gerilim cikis geriliminden buyuk ise calistirir--YUKSELTME

      while(vout<vpot){           //cikis pot geriliminden dusuk oldugunda duty'i yukseltmesi daha sonra tekrardan okumasini istiyoruz.
        
        duty=duty+1;
        dutybelirleme();
        delay(4+duty/100);
        oku();
        
      }
    }
    
    if(vout > vpot){         //istenen gerilim cikis geriliminden kucuk ise calisir--DUSURME
      
      if(vout>vpot+200){
         memory=duty;
         duty=0;
         OCR1A=duty;                    //dutyi buradan ayarla bu kismi silebiliriz.
         OCR1B=249-duty;
         delayMicroseconds(100);
         duty=memory-round((vout-vpot)/2);
      }
      oku();
      while(vout>vpot){           //cikis pot geriliminden yuksek oldugunda duty'i dusurmesi daha sonra tekrardan okumasini istiyoruz.
        
        
        duty=duty-2;
        dutybelirleme();
        delay(4+duty/100);
        oku();
       
      }
      
    }

    delay(1);                      //araya kucuk delay koyduk.
    
}

void dutybelirleme(){
  
    if(duty>210){
      duty=210;                     //duty'i sinirlandirdik.
    }
    else if(duty<1){
      duty=1;                      //duty'i sinirlandirdik.
    }
}

void oku(){
    
    vpot = analogRead(pot_pin);     //pottan okunan degeri vpot'a atadik
    
    vout = analogRead(feedback);   //cikistan okunan degeri vout'a atadik
    
    if(vpot>780){                   //pot degeri 4V'dan(cikistan alinan deger icin 40V'a tekabul ediyor) buyuk oldugunda 40V'a cekiyor.
      vpot=780;
    }
    
}


void downduty(){                 //Valley Current engellemek icin duty'i dusurup arttiriyoruz.

  memory=duty;
  duty=1;
  OCR1A=duty;                    //dutyi buradan ayarla bu kismi silebiliriz.
  OCR1B=249-duty;
  delayMicroseconds(100);
  duty=memory;
  down=0;
  
}
