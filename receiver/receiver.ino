// Código para o recebimento de informações e controle do robô

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define MAX_PWM 250
#define MIN_PWM 0

#define PWM_INCREMENTOR 10

#define PWMA 32
#define A1 26
#define B1 27

#define PWMB 13
#define A2 25
#define B2 33

#define CANAL 1

// Definindo variáveis ------------------------------------------

int pwm = 100;

int first_mark = 0, second_mark;

typedef struct struct_message{
  char commands;
  int pwm;
} struct_message;

struct_message rcv_command;

// Atualizando dados recebidos via ESP-NOW------------------------

void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&rcv_command, incomingData, sizeof(rcv_command));
  first_mark = millis();
}

// Funções do motores -------------------------------------------

void motor_R(int power) {
  if (power > 0) {
    digitalWrite(A1, 1);
    digitalWrite(B1, 0);
  } else {
    digitalWrite(A1, 0);
    digitalWrite(B1, 1);
  }
  ledcWrite(1, abs(power));
}


void motor_L(int power) {
  if (power > 0) {
    digitalWrite(A2, 1);
    digitalWrite(B2, 0);
  } else {
    digitalWrite(A2, 0);
    digitalWrite(B2, 1);
  }
  ledcWrite(2, abs(power));
}


void motors_control(float power_l, float power_r) {
    motor_L(power_l);
    motor_R(power_r);
}

int wrap_to_pwm(int pwm)
{
  if (pwm > MAX_PWM)
  {
    return MAX_PWM;
  }
  else if (pwm < MIN_PWM)
  {
    return MIN_PWM;
  }
  else
  {
    return pwm;
  }
}

void increase_pwm()
{
  int nextPwm = pwm + PWM_INCREMENTOR;
  nextPwm = wrap_to_pwm(nextPwm);
  pwm = nextPwm;
  delay(500);
}

void decrease_pwm()
{
  int nextPwm = pwm - PWM_INCREMENTOR;
  nextPwm = wrap_to_pwm(nextPwm);
  pwm = nextPwm;
  delay(500);
}

// Setup e loop --------------------------------------------------

void setup() {
  
  // Configuração de pinos para PWM
  ledcAttachPin(PWMA, 1);
  ledcAttachPin(PWMB, 2);

  ledcSetup(1, 80000, 8);
  ledcSetup(2, 80000, 8);

  // Configuração de pinos
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  // Configurações comunicação ESP-NOW

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
  ESP_ERROR_CHECK( esp_wifi_start());
  ESP_ERROR_CHECK( esp_wifi_set_channel(CANAL, WIFI_SECOND_CHAN_NONE));

  if (esp_now_init() != ESP_OK) 
  {
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  second_mark = millis();

  // Protegendo o robô após a perda de informação
  if (second_mark - first_mark > 500) 
  {
    motors_control(0, 0);
  }
  else
  {
    // Executando o controle dos motores
    switch (rcv_command.commands)
    {
    case 'w':
        motors_control(pwm, pwm);
        break;
    case 'a':
        motors_control(-pwm/2, pwm/2);
        break;
    case 's':
        motors_control(-pwm, -pwm);
        break;
    case 'd':
        motors_control(pwm/2, -pwm/2);
        break;
    case 'n':
        motors_control(0, 0);
        break;
    case '1':
        increase_pwm();
        break;
    case '2':
        decrease_pwm();
        break;
    default:
        motors_control(0, 0);
        break;
    }
  }
}