// 设置按钮连接的数字引脚。
const int buttonPin = 2;

// 设置三个 LED 连接的引脚。
const int ledPins[] = {9, 10, 11};

// 保存当前灯光是否开启。
bool lightsOn = false;

// 保存按钮上一次读取到的原始状态。
int lastButtonReading = HIGH;

// 保存经过消抖确认后的按钮状态。
int buttonState = HIGH;

// 记录按钮状态上次变化的时间。
unsigned long lastDebounceTime = 0;

// 设置按钮消抖所需的时间，单位是毫秒。
const unsigned long debounceDelay = 35;

// 保存 D9 LED 当前的亮度。
int brightness = 0;

// 设置亮度变化的方向，1 表示变亮，-1 表示变暗。
int fadeDirection = 1;

// 记录上次更新渐变亮度的时间。
unsigned long lastFadeUpdate = 0;

// 初始化按钮和 LED 引脚。
void setup() {
  // 使用 Arduino 内部上拉电阻读取按钮，按钮另一端接 GND。
  pinMode(buttonPin, INPUT_PULLUP);

  // 用 for 循环把三个 LED 引脚设置为输出。
  for (int i = 0; i < 3; i++) {
    // 设置当前 LED 引脚为输出模式。
    pinMode(ledPins[i], OUTPUT);
  }
}

// 持续读取按钮并更新灯光状态。
void loop() {
  // 读取按钮当前的原始状态。
  int reading = digitalRead(buttonPin);

  // 如果按钮读数发生变化，就重新开始消抖计时。
  if (reading != lastButtonReading) {
    // 记录这次状态变化的时间。
    lastDebounceTime = millis();
  }

  // 确认按钮状态已稳定超过消抖时间。
  if (millis() - lastDebounceTime > debounceDelay) {
    // 如果稳定后的读数与已确认状态不同，就更新按钮状态。
    if (reading != buttonState) {
      // 保存新的稳定按钮状态。
      buttonState = reading;

      // INPUT_PULLUP 模式下，按下按钮会读到 LOW。
      if (buttonState == LOW) {
        // 每次按下按钮都切换灯光开关状态。
        lightsOn = !lightsOn;
      }
    }
  }

  // 保存这次原始读数，供下一轮比较。
  lastButtonReading = reading;

  // 如果灯光关闭，就关闭三个 LED。
  if (!lightsOn) {
    // 用 for 循环逐个关闭三个 LED。
    for (int i = 0; i < 3; i++) {
      // 将当前 LED 引脚设为 LOW，也就是关闭。
      digitalWrite(ledPins[i], LOW);
    }

    // 将 D9 LED 的 PWM 亮度设为 0。
    analogWrite(ledPins[0], 0);

    // 灯光关闭时结束本轮循环。
    return;
  }

  // 让 D10 LED 常亮。
  digitalWrite(ledPins[1], HIGH);

  // 让 D11 LED 常亮。
  digitalWrite(ledPins[2], HIGH);

  // 每隔 10 毫秒更新一次 D9 LED 的亮度。
  if (millis() - lastFadeUpdate >= 10) {
    // 记录本次亮度更新时间。
    lastFadeUpdate = millis();

    // 按当前方向调整亮度。
    brightness = brightness + fadeDirection;

    // 到达最暗时，改为逐渐变亮。
    if (brightness <= 0) {
      // 把亮度限制在最低值。
      brightness = 0;

      // 设置亮度变化方向为变亮。
      fadeDirection = 1;
    }

    // 到达最亮时，改为逐渐变暗。
    if (brightness >= 255) {
      // 把亮度限制在最高值。
      brightness = 255;

      // 设置亮度变化方向为变暗。
      fadeDirection = -1;
    }

    // 用 PWM 设置 D9 LED 的亮度，实现渐变效果。
    analogWrite(ledPins[0], brightness);
  }
}
