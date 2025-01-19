#include <LiquidCrystal.h>

#define LM35 A0
#define RED_DIODE D9
#define GREEN_DIODE D10
#define YELLOW_DIODE D11
#define MOTOR_PWM D3
#define MOTOR_PLUS D5
#define MOTOR_MINUS D6
#define BUTTON D1

static uint8_t constexpr DECIMAL_POINT = 1u;
static int8_t constexpr MIN_PWM = 0;
static int8_t constexpr MAX_PWM = 255;
static float constexpr DELTA_TEMP = 0.5f;
static float constexpr TEMP_HYST = 1.0f;
static float constexpr HIGH_TEMP = 31.0f;
static float constexpr LOW_TEMP = 25.0f;

static unsigned long prev_time = 0u;
static float previous_temp = 0.0f;
static bool motor_running = false;

enum class ACTIVE_DIODE
{
  YELLOW,
  GREEN,
  RED
};

static ACTIVE_DIODE active_diode = ACTIVE_DIODE::GREEN;
LiquidCrystal lcd(D2, D4, D7, D8, D12, D13);

void setup()
{
  set_pin_modes();
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();

  analogWrite(RED_DIODE, MIN_PWM);
  analogWrite(GREEN_DIODE, MAX_PWM);
  analogWrite(YELLOW_DIODE, MIN_PWM);
  analogWrite(MOTOR_PWM, MAX_PWM);
  stop_motor();
}
 
void loop()
{
  bool const is_button_pressed = (bool)digitalRead(BUTTON);
  check_stop_start_motor(is_button_pressed);

  float const current_temp = (((float)analogRead(LM35) / 1024.0f) * 5.0f) * 100.0f;

  if((abs(current_temp - previous_temp) >= DELTA_TEMP) && (millis() >= prev_time))
  {
    update_lcd(current_temp);
    update_diode(current_temp);
    previous_temp = current_temp;
    prev_time += 500u;
  }
}

void
set_pin_modes()
{
  pinMode(LM35, INPUT);
  pinMode(BUTTON, INPUT);

  pinMode(RED_DIODE, OUTPUT);
  pinMode(GREEN_DIODE, OUTPUT);
  pinMode(YELLOW_DIODE, OUTPUT);

  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_PLUS, OUTPUT);
  pinMode(MOTOR_MINUS, OUTPUT);
}

void
update_lcd(float current_temp)
{
  char const degrees_sign = 223;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura");
  lcd.setCursor(0, 1);
  lcd.print(current_temp, DECIMAL_POINT);
  lcd.print(" [");
  lcd.print(degrees_sign);
  lcd.print("C]");
}

void
update_diode(float current_temp)
{
  if(active_diode == ACTIVE_DIODE::GREEN)
  {
    if(current_temp > HIGH_TEMP + TEMP_HYST)
    {
      analogWrite(RED_DIODE, MAX_PWM);
      analogWrite(GREEN_DIODE, MIN_PWM);
      analogWrite(YELLOW_DIODE, MIN_PWM);
      active_diode = ACTIVE_DIODE::RED;
    }
    else if(current_temp < LOW_TEMP - TEMP_HYST)
    {
      analogWrite(RED_DIODE, MIN_PWM);
      analogWrite(GREEN_DIODE, MIN_PWM);
      analogWrite(YELLOW_DIODE, MAX_PWM);
      active_diode = ACTIVE_DIODE::YELLOW;
    }
  }
  else
  {
    if(((active_diode == ACTIVE_DIODE::RED) && (current_temp < HIGH_TEMP - TEMP_HYST)) || ((active_diode == ACTIVE_DIODE::YELLOW) && (current_temp > LOW_TEMP + TEMP_HYST)))
    {
      analogWrite(RED_DIODE, MIN_PWM);
      analogWrite(GREEN_DIODE, MAX_PWM);
      analogWrite(YELLOW_DIODE, MIN_PWM);
      active_diode = ACTIVE_DIODE::GREEN;
    }
  }
}

void
check_stop_start_motor(bool is_button_pressed)
{
  if(motor_running && is_button_pressed)
  {
    motor_running = false;
    stop_motor();
  }
  else if(!motor_running && is_button_pressed)
  {
    motor_running = true;
    start_motor();
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