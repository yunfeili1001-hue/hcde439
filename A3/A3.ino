// 引入计算温度所需的数学函数。
#include <math.h>

// 绿色 LED 连接 D9。
const int GREEN_PIN = 9;
// 红色 LED 连接 D10。
const int RED_PIN = 10;
// 分压电路的中间接点连接 A0。
const int SENSOR_PIN = A0;

// 固定电阻为 10kΩ。
const float FIXED_R = 10000.0;
// 热敏电阻在 25°C 时为 10kΩ。
const float NOMINAL_R = 10000.0;
// 将标称温度 25°C 表示为开尔文温度。
const float NOMINAL_T = 298.15;
// 热敏电阻的 Beta 参数，需根据实际规格确认。
const float BETA = 3950.0;
// 红绿灯的切换温度为 80°F。
const float THRESHOLD_F = 140;

// 初始化 LED 引脚和串口。
void setup() {
  // 将绿灯引脚设为输出。
  pinMode(GREEN_PIN, OUTPUT);
  // 将红灯引脚设为输出。
  pinMode(RED_PIN, OUTPUT);
  // 启动串口，通信速率为 9600。
  Serial.begin(9600);
}

// 持续读取温度并控制 LED。
void loop() {
  // 读取 A0，Uno 默认读数范围为 0–1023。
  int raw = analogRead(SENSOR_PIN);

  // 检查边界读数，避免无效的温度计算。
  if (raw <= 0 || raw >= 1023) {
    // 关闭绿灯。
    analogWrite(GREEN_PIN, 0);
    // 关闭红灯。
    analogWrite(RED_PIN, 0);
    // 输出接线检查提示。
    Serial.println("Sensor reading invalid: check wiring.");
    // 等待半秒。
    delay(500);
    // 跳过本次循环的剩余部分。
    return;
  }

  // 根据分压公式计算下方热敏电阻的阻值。
  float resistance = FIXED_R * raw / (1023.0 - raw);
  // 使用 Beta 方程计算开尔文温度的倒数。
  float inverseT = (1.0 / NOMINAL_T) + log(resistance / NOMINAL_R) / BETA;
  // 将开尔文温度转换为摄氏温度。
  float tempC = (1.0 / inverseT) - 273.15;
  // 将摄氏温度转换为华氏温度。
  float tempF = tempC * 9.0 / 5.0 + 32.0;

  // 温度低于 80°F 时亮绿灯。
  if (tempF < THRESHOLD_F) {
    // 将用于亮度映射的温度限制在 60–80°F。
    long boundedTemp = constrain((long)tempF, 60L, 80L);
    // 将温度映射为绿灯亮度，越接近 80°F 越暗。
    int brightness = map(boundedTemp, 60, 80, 255, 60);
    // 使用 PWM 控制绿灯亮度。
    analogWrite(GREEN_PIN, brightness);
    // 关闭红灯。
    analogWrite(RED_PIN, 0);
  }
  // 温度达到或超过 80°F 时亮红灯。
  else {
    // 将用于亮度映射的温度限制在 80–100°F。
    long boundedTemp = constrain((long)tempF, 80L, 100L);
    // 将温度映射为红灯亮度，温度越高越亮。
    int brightness = map(boundedTemp, 80, 100, 60, 255);
    // 使用 PWM 控制红灯亮度。
    analogWrite(RED_PIN, brightness);
    // 关闭绿灯。
    analogWrite(GREEN_PIN, 0);
  }

  // 输出原始读数标签。
  Serial.print("ADC: ");
  // 输出 A0 原始读数。
  Serial.print(raw);
  // 输出温度标签。
  Serial.print(" | Temperature: ");
  // 输出华氏温度，保留一位小数。
  Serial.print(tempF, 1);
  // 输出温度单位和指示灯标签。
  Serial.print(" F | Indicator: ");
  // 输出当前亮起的 LED 颜色。
  Serial.println(tempF < THRESHOLD_F ? "GREEN" : "RED");
  // 每半秒更新一次。
  delay(500);
}