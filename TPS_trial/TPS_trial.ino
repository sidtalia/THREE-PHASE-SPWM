

class TPS
{
private:
  int32_t sine_time;
  int32_t sine_now;
  int32_t TRIANGLE_TIME;
  float PWM_A,PWM_B,PWM_C,t;
  float freq,multiplier;
  long time_period,loop_timer;
public:
  TPS();
  void TPS_Time_Stamp();
  void TPS_Setup(int triangle_Frequency,int CPU_clock);
  void TPS_Create_SPWM(int sine_Frequency);
};

TPS generator;

void setup() {
  // put your setup code here, to run once:
  generator.TPS_Setup(3000,127);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  generator.TPS_Time_Stamp();
  //put other code here
  generator.TPS_Create_SPWM(50);
}
