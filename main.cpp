#include <Arduino.h>
#include <Esp32McpwmMotor.h>
#include <Esp32PcntEncoder.h>

Esp32PcntEncoder encoders[4];
Esp32McpwmMotor motor;

int64_t last_ticks[4] = {0, 0, 0, 0};
int32_t delta_ticks[4] = {0, 0, 0, 0};
uint64_t last_update_time = 0;
float current_speeds[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(115200);

  motor.attachMotor(0, 5, 4);
  motor.attachMotor(1, 15, 16);
  motor.attachMotor(2, 3, 8);
  motor.attachMotor(3, 46, 9);

  encoders[0].init(0, 6, 7);
  encoders[1].init(1, 18, 17);
  encoders[2].init(2, 20, 19);
  encoders[3].init(3, 11, 10);

  for (int i = 0; i < 4; i++) {
    motor.updateMotorSpeed(i, 70);
    last_ticks[i] = encoders[i].getTicks();   // 初始化 tick
  }
  last_update_time = millis();
}

void loop() {
  delay(10);

  uint64_t now_ms = millis();
  uint64_t dt = now_ms - last_update_time;
  if (dt == 0) return;

  for (int i = 0; i < 4; i++) {
    int64_t now_ticks = encoders[i].getTicks();
    delta_ticks[i] = (int32_t)(now_ticks - last_ticks[i]);

    // tick->mm 的系数如果是 0.1051566 mm/tick，则速度 = mm/ms = m/s
    current_speeds[i] = float(delta_ticks[i] * 0.1051566f) / float(dt);

    last_ticks[i] = now_ticks;
  }

  last_update_time = now_ms;

  Serial.printf("speeds: 1=%f m/s 2=%f m/s 3=%f m/s 4=%f m/s\n",
                current_speeds[0], current_speeds[1],
                current_speeds[2], current_speeds[3]);
}
