#define TRIGGER D1
#define BUTTON D2
#define RGB D3
#define ECHO D4
#define MOTOR_PWM D5
#define MOTOR_PLUS D6
#define MOTOR_MINUS D7

static float constexpr MIN_PWM = 0.0f;
static float constexpr PWM_RANGE = 256.0f;
static float constexpr MIN_DISTANCE = 3.0f; // [cm]
static float constexpr MAX_DISTANCE = 50.0f; // [cm]
volatile bool motor_running = false;

static float constexpr DEBOUNCE_TIME = 100.0f; // [ms]
volatile float start_debounce = 0.0f;


void setup() 
{
  set_pin_modes();
  Serial.begin(9600);
  analogWrite(RGB, static_cast<int>(PWM_RANGE));
  analogWrite(MOTOR_PWM, static_cast<int>(MIN_PWM));
  attachInterrupt(digitalPinToInterrupt(BUTTON), check_stop_start_motor, RISING);
}

void loop() 
{
  uint32_t const distance = measure_distance();
  update_RGB_diode(distance);
}

void
set_pin_modes()
{
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RGB, OUTPUT);

  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_PLUS, OUTPUT);
  pinMode(MOTOR_MINUS, OUTPUT);
  pinMode(BUTTON, INPUT);
}

uint32_t
measure_distance()
{
  uint8_t const delay_low = 2u; // [us]
  uint8_t const delay_high = 10u; // [us]
  uint8_t const conversion_value = 58u;

  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(delay_low);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(delay_high);
  digitalWrite(TRIGGER, LOW);

  uint32_t const time = pulseIn(ECHO, HIGH);
  uint32_t const distance = static_cast<uint32_t>(static_cast<float>(time) / static_cast<float>(conversion_value));

  return distance;
}

void
update_RGB_diode(uint32_t distance)
{
  float distance_f = static_cast<float>(distance);
  distance_f = (distance_f > MAX_DISTANCE) ? MAX_DISTANCE : distance_f;
  distance_f = (distance_f < MIN_DISTANCE) ? MIN_DISTANCE : distance_f;

  // RGB diode used is in the common cathode configuration - applying zero duty cycle equals the largest illumination.
  float const pwm_value = ((distance_f - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE)) * PWM_RANGE;

  delayMicroseconds(100);
  analogWrite(RGB, static_cast<int>(pwm_value));
  update_motor_speed(pwm_value);
}

void
check_stop_start_motor()
{
  if((millis() - start_debounce) > DEBOUNCE_TIME)
  {
    if(motor_running)
    {
      motor_running = false;
      stop_motor();
      start_debounce = millis();
    }
    else
    {
      motor_running = true;
      start_motor();
      start_debounce = millis();
    }
  } 
}

void
stop_motor()
{
  digitalWrite(MOTOR_PLUS, PinStatus::LOW);
  digitalWrite(MOTOR_MINUS, PinStatus::LOW);
}

void
start_motor()
{
  digitalWrite(MOTOR_PLUS, PinStatus::HIGH);
  digitalWrite(MOTOR_MINUS, PinStatus::LOW);
}

void
update_motor_speed(float pwm_value)
{
  analogWrite(MOTOR_PWM, static_cast<int>(abs(PWM_RANGE - pwm_value)));
}