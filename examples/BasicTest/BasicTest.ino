/*
  MiniTFT — базовый пример.
  Подключение: RES -> пин 10, DC -> пин 9, SCK/MOSI -> аппаратные пины SPI
  (D13/D11 на Arduino Uno). CS у модуля обычно посажен на GND на самой плате
  экрана — отдельный пин для CS драйверу не нужен.
*/
#include <MiniTFT.h>

MiniTFT tft(/*rstPin=*/10, /*dcPin=*/9);

void setup() {
  // Если ваш экран НЕ работает с настройками по умолчанию (SPI_MODE3,
  // 8 МГц, MADCTL=0xA0, offset X=80/Y=0) — раскомментируйте и подберите
  // свои значения ДО вызова init(). См. README.md библиотеки.
  // tft.setSpiMode(SPI_MODE0);
  // tft.setMadctl(0x00);
  // tft.setOffset(0, 0);

  tft.init(240, 240);
  tft.fillScreen(MTFT_BLACK);

  tft.setTextColor(MTFT_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("MiniTFT"));

  tft.setTextColor(MTFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.print(F("Free RAM: "));
  tft.println(123); // пример вывода числа

  tft.drawFastHLine(0, 60, 240, MTFT_GREY);

  tft.fillRect(10, 70, 60, 40, MTFT_RED);
  tft.fillRect(80, 70, 60, 40, MTFT_GREEN);
  tft.fillRect(150, 70, 60, 40, MTFT_BLUE);
}

void loop() {
}
