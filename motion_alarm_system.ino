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
  
  // Set LED and Buzzer pins as output
  DDRB |= (1 << DDB1) | (1 << DDB0);
  
  // Set PIR and button pins as input
  DDRD &= ~((1 << DDD2) | (1 << DDD3));
  
  // Configure external interrupts for rising edge detection
  SET_INT0_RISING_EDGE;
  ENABLE_INT0;
  SET_INT1_RISING_EDGE;
  ENABLE_INT1;
  
  // Setup Timer1 in CTC mode to trigger every 4 seconds
  TCCR1A = 0;                              
  TCCR1B = (1 << WGM12);                  
  OCR1A = 62496;                         
  TIMSK1 |= (1 << OCIE1A);              
  TCCR1B |= (1 << CS12) | (1 << CS10); 
  
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