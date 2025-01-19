#define TRIGGER D2
#define ECHO D4
#define RGB D3

static float constexpr PWM_RANGE = 256.0f;
static float constexpr MIN_DISTANCE = 3.0f; // [cm]
static float constexpr MAX_DISTANCE = 30.0f; // [cm]

void setup() 
{
  set_pin_modes();
  Serial.begin(9600);
  analogWrite(RGB, PWM_RANGE);
}

void loop() 
{
  uint32_t const distance = measure_distance();

  // RGB diode used is in the common cathode configuration - applying max duty cycle equals no illumination.
  update_RGB_diode(distance);
}

void
set_pin_modes()
{
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(RGB, OUTPUT);
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
  uint32_t const distance = time / conversion_value;

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
  Serial.println(pwm_value);
  analogWrite(RGB, pwm_value);
}

