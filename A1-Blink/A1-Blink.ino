// LED 1 接 Arduino Uno 的 PWM 引脚 D3
const byte led1Pin = 3;

// LED 2 接 Arduino Uno 的 PWM 引脚 D5
const byte led2Pin = 5;

// LED 3 接 Arduino Uno 的 PWM 引脚 D6
const byte led3Pin = 6;

// 按钮接数字引脚 D2
const byte buttonPin = 2;

// 初始化 LED 和按钮引脚
void setup() {
  // 将 LED 1 引脚设置为输出
  pinMode(led1Pin, OUTPUT);

  // 将 LED 2 引脚设置为输出
  pinMode(led2Pin, OUTPUT);

  // 将 LED 3 引脚设置为输出
  pinMode(led3Pin, OUTPUT);

  // 将按钮引脚设置为输入
  pinMode(buttonPin, INPUT);

  // 开始时关闭 LED 1
  digitalWrite(led1Pin, LOW);

  // 开始时关闭 LED 2
  digitalWrite(led2Pin, LOW);

  // 开始时关闭 LED 3
  digitalWrite(led3Pin, LOW);
}

// 持续检查按钮，并在按下时运行灯光效果
void loop() {
  // 读取按钮状态；按下时外接电路会让引脚读到 HIGH
  if (digitalRead(buttonPin) == HIGH) {
    // 从亮度 0 开始，每次增加 5，直到亮度达到 255
    for (int brightness = 0; brightness <= 255; brightness += 5) {
      // 逐渐增加 LED 1 的亮度
      analogWrite(led1Pin, brightness);

      // 逐渐增加 LED 2 的亮度
      analogWrite(led2Pin, brightness);

      // 逐渐降低 LED 3 的亮度
      analogWrite(led3Pin, 255 - brightness);

      // 短暂等待，让亮度变化可以被观察到
      delay(15);
    }

    // 渐变结束后，让 LED 1 保持点亮
    digitalWrite(led1Pin, HIGH);

    // 渐变结束后关闭 LED 2
    digitalWrite(led2Pin, LOW);

    // 渐变结束后关闭 LED 3
    digitalWrite(led3Pin, LOW);

    // 等待按钮松开，避免按住时效果不断重复
    while (digitalRead(buttonPin) == HIGH) {
      // 短暂等待后再次检查按钮状态
      delay(10);
    }

    // 等待按钮触点稳定，减少抖动造成的误触发
    delay(30);
  }
}