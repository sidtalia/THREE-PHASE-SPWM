#define AP TIMER4_BASE->CCR1
#define AN TIMER4_BASE->CCR2
#define BP TIMER4_BASE->CCR3
#define BN TIMER4_BASE->CCR4
#define CP TIMER1_BASE->CCR1
#define CN TIMER1_BASE->CCR4

#define MULTI_CONST float(2*PI*0.000001)



TPS::TPS()
{
  sine_time = 0;
  sine_now = 0;
  loop_timer = 0;
}

void TPS::TPS_Setup(int triangle_Frequency,int CPU_clock)
{
  TRIANGLE_TIME = int32_t(1000000/triangle_Frequency);

  pinMode(PB6, PWM); //AP
  pinMode(PB7, PWM); //AN
  pinMode(PB8, PWM); //BP
  pinMode(PB9, PWM); //BN
  pinMode(PA8, PWM); //CP
  pinMode(PA11, PWM);//CN
  
  TIMER4_BASE->CR1 = TIMER_CR1_CEN | TIMER_CR1_ARPE;
  TIMER4_BASE->CR2 = 0;
  TIMER4_BASE->SMCR = 0;
  TIMER4_BASE->DIER = 0;
  TIMER4_BASE->EGR = 0;
  TIMER4_BASE->CCMR1 = (0b110 << 4) | TIMER_CCMR1_OC1PE |(0b110 << 12) | TIMER_CCMR1_OC2PE;
  TIMER4_BASE->CCMR2 = (0b110 << 4) | TIMER_CCMR2_OC3PE |(0b110 << 12) | TIMER_CCMR2_OC4PE;
  TIMER4_BASE->CCER = TIMER_CCER_CC3E | TIMER_CCER_CC4E;
  TIMER4_BASE->PSC = CPU_clock;
  TIMER4_BASE->ARR = TRIANGLE_TIME;
  TIMER4_BASE->DCR = 0;
  AP = 0;
  AN = 0;
  BP = 0;
  BN = 0;

  TIMER1_BASE->CR1 = TIMER_CR1_CEN | TIMER_CR1_ARPE;
  TIMER1_BASE->CR2 = 0;
  TIMER1_BASE->SMCR = 0;
  TIMER1_BASE->DIER = 0;
  TIMER1_BASE->EGR = 0;
  TIMER1_BASE->CCMR1 = (0b110 << 4) | TIMER_CCMR1_OC1PE;
  TIMER1_BASE->CCMR2 = (0b110 << 12) | TIMER_CCMR2_OC4PE;
  TIMER1_BASE->CCER = TIMER_CCER_CC1E | TIMER_CCER_CC4E;
  TIMER1_BASE->PSC = CPU_clock;
  TIMER1_BASE->ARR = TRIANGLE_TIME;
  TIMER1_BASE->DCR = 0;
  CP = 0;
  CN = 0;
}

void TPS::TPS_Time_Stamp()
{
  loop_timer = micros()+TRIANGLE_TIME;
}

void TPS::TPS_Create_SPWM(int sine_Frequency)
{
  time_period = 1000000/sine_Frequency;
  multiplier = MULTI_CONST*freq;

  sine_time = micros() - sine_now; //keeping a track of the time for the sine wave
  if(sine_time>=time_period)//if time period is complete
  {
    sine_now = micros(); //take the time stamp at the moment when cycle was completed
  }
  t = sine_time*multiplier; //scale the value of time period down to (0,2pi) range
  PWM_A = TRIANGLE_TIME*sin(t); //calculate pulse duty for phase A 
  PWM_B = TRIANGLE_TIME*sin(t + 0.6666*PI); //for phase B
  PWM_C = TRIANGLE_TIME*sin(t - 0.6666*PI); //for phase C
  
  TIMER4_BASE->CNT = TRIANGLE_TIME; //reset the pwm generator
  TIMER1_BASE->CNT = TRIANGLE_TIME; //same as above
  
  if(PWM_A>0) //TRY PUTTING THE AUTO-RELOAD RESET AFTER THE IF CONDITIONS
  {
    AP = PWM_A; //set the pwm value for phase A
    AN = 0;
  }
  else if(PWM_A<0)
  {
    AN = -PWM_A;
    AP = 0;
  }
    
  if(PWM_B>0)
  {
    BP = PWM_B; //set the pwm value for phase A
    BN = 0;
  }
  else if(PWM_B<0)
  {
    BN = -PWM_B;
    BP = 0;
  }
  
  if(PWM_C>0)
  {
    CP = PWM_C; //set the pwm value for phase A
    CN = 0;
  }
  else if(PWM_C<0)
  {
    CN = -PWM_C;
    CP = 0;
  }


  while(micros()-loop_timer>TRIANGLE_TIME);
}
