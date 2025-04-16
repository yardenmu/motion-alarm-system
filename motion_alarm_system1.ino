#define SET_INT0_RISING_EDGE EICRA |= (1 << ISC00) | (1 << ISC01)
#define ENABLE_INT0 EIMSK |= (1 << INT0)
#define SET_INT1_RISING_EDGE EICRA |= (1 << ISC10) | (1 << ISC11)
#define ENABLE_INT1 EIMSK |= (1 << INT1)
#define BLINKING_LED(port, led_pin) port ^= (1 << led_pin)
#define TURN_OFF_LED(port, led_pin) port &= ~(1 << led_pin)
#define TURN_ON_ALARM(port, buz_pin) port |= (1 << buz_pin)
#define TURN_OFF_ALARM(port, buz_pin) port &= ~(1 << buz_pin)

volatile uint8_t alarm_flag = 0;
void setup()
{
  cli();
  
  //define led as output
  DDRB |= (1 << DDB1);
  
  //define buzzer as output
  DDRB |= (1 << DDB0);
  
  //define PIR, button as input
  DDRD &= ~((1 << DDD2) | (1 << DDD3));
  
  SET_INT0_RISING_EDGE;
  ENABLE_INT0;
  SET_INT1_RISING_EDGE;
  ENABLE_INT1;
  
  TCCR1A = 0;                              // Clear Timer1 Control Register A
  TCCR1B = (1 << WGM12);                  // Set CTC mode
  OCR1A = 62496;                         // Compare value for 4 seconds
  TIMSK1 |= (1 << OCIE1A);              // Enable Timer1 Compare Match A interrupt
  TCCR1B |= (1 << CS12) | (1 << CS10); // Start Timer1 with prescaler 1024
  
  Serial.begin(9600); // Initialize UART

  sei();
}

void loop()
{
  if(alarm_flag){
    BLINKING_LED(PORTB, PORTB1);
    TURN_ON_ALARM(PORTB, PORTB0);
    delay(500);
  }
  else{
    TURN_OFF_LED(PORTB, PORTB1);
  	TURN_OFF_ALARM(PORTB, PORTB0);
  }
  //Check if user sent a command via UART
  if(Serial.available()){
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if(cmd == "SILENCE"){
      alarm_flag = 0;
      Serial.println("Alarm silenced via UART");
    }
  }
  
}

//interrupt for PIR
ISR(INT0_vect){
  alarm_flag = 1;
}

//interrupt for button
ISR(INT1_vect){
  alarm_flag = 0;
}

ISR(TIMER1_COMPA_vect){
  if(alarm_flag){
	Serial.println("Motion Detected, Alarm Active");
  }
}