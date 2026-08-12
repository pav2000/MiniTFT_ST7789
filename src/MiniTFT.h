/*
  ==========================================================================
  MiniTFT — минимальный SPI-драйвер ST7789 (240x240) без Adafruit_GFX/
  Adafruit_ST7789/Adafruit_BusIO.

  Написан для платы, где связка Adafruit_GFX+Adafruit_ST7789+Adafruit_BusIO
  не влезала во флеш-память ATmega328 (Arduino Uno). Поддерживает только
  то, что реально нужно для простых диагностических/информационных экранов:
  заливку прямоугольников, линии, текст компактным шрифтом 5x7 (только
  заглавные буквы, цифры и базовая пунктуация — экономия флеша; строчные
  буквы автоматически переводятся в заглавные при выводе).

  ВАЖНО — параметры SPI/адресации подобраны экспериментально для конкретной
  партии модулей ST7789 240x240 (см. README.md) и НЕ являются универсальными
  для всех модулей ST7789! Если экран не заработает "из коробки" на вашей
  плате — см. setSpiMode()/setMadctl()/setOffset() ниже и раздел
  "Если экран не заработал" в README.md.

  Использование (класс наследует Print — значит print()/println() со всеми
  стандартными перегрузками, включая F()-строки, доступны сразу):

    #include <MiniTFT.h>
    MiniTFT tft(10, 9); // CS у модуля обычно сажают на GND
    void setup() {
      tft.init(240, 240);
      tft.fillScreen(MTFT_BLACK);
      tft.setTextColor(MTFT_GREEN);
      tft.setTextSize(2);
      tft.setCursor(10, 10);
      tft.println(F("Hello, MiniTFT!"));
    }

  Лицензия: делайте с этим кодом что хотите (MIT-подобно), автор не против.
  ==========================================================================
*/

#ifndef MINI_TFT_H
#define MINI_TFT_H

#include <Arduino.h>
#include <SPI.h>

// Базовые цвета RGB565 (совпадают по значению с ST77XX_* из Adafruit_ST7789,
// так что любой код/справочник по RGB565-цветам для ST7789 подходит).
#define MTFT_BLACK   0x0000
#define MTFT_WHITE   0xFFFF
#define MTFT_RED     0xF800
#define MTFT_GREEN   0x07E0
#define MTFT_BLUE    0x001F
#define MTFT_CYAN    0x07FF
#define MTFT_MAGENTA 0xF81F
#define MTFT_YELLOW  0xFFE0
#define MTFT_GREY    0x7BEF  // тёмно-серый — полезно для линий-разделителей

class MiniTFT : public Print {
public:
  // rstPin — пин RESET экрана, dcPin — пин DC (Data/Command).
  // CS у большинства дешёвых модулей ST7789 разведён на плате и посажен на
  // GND — отдельный CS-пин драйверу не нужен и не запрашивается.
  MiniTFT(uint8_t rstPin, uint8_t dcPin);

  // --- Настройка ПЕРЕД init(), если у вашей панели другие требования ---
  // Значения по умолчанию (SPI_MODE3, 8 МГц, MADCTL=0xA0, offset X=+80/Y=0)
  // подобраны экспериментально для конкретной партии модулей 240x240 — на
  // других панелях может понадобиться MODE0 и/или другие offset/MADCTL.
  void setSpiSpeed(uint32_t hz);     // по умолчанию 8 000 000
  void setSpiMode(uint8_t mode);     // SPI_MODE0..3, по умолчанию SPI_MODE3
  void setMadctl(uint8_t madctl);    // регистр 0x36, по умолчанию 0xA0
  void setOffset(int16_t xOffset, int16_t yOffset); // по умолчанию 80, 0

  // Инициализация экрана. Выполняет полную последовательность настройки
  // ДВАЖДЫ подряд — экспериментально подтверждено, что одного прохода сразу
  // после подачи питания недостаточно для стабильного запуска (см. README).
  void init(uint16_t w, uint16_t h);

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillScreen(uint16_t color);
  // Радиус игнорируется (экономия флеша) — просто прямоугольник. Сделано
  // для совместимости по сигнатуре с Adafruit_GFX::fillRoundRect().
  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  void setTextColor(uint16_t fg);              // прозрачный фон
  void setTextColor(uint16_t fg, uint16_t bg);  // непрозрачный фон
  void setCursor(int16_t x, int16_t y);
  void setTextSize(uint8_t s);
  void setTextWrap(bool w); // не поддерживается (нет автопереноса), но
                             // метод есть для совместимости по сигнатуре

  int16_t width()  const { return _w; }
  int16_t height() const { return _h; }

  // Реализация Print — этим методом печатаются все print()/println(),
  // включая перегрузки для чисел, String и F()-строк (из PROGMEM).
  virtual size_t write(uint8_t c) override;

private:
  uint8_t _rstPin, _dcPin;
  uint32_t _spiSpeed = 8000000UL;
  uint8_t _spiMode   = SPI_MODE3;
  uint8_t _madctl    = 0xA0;
  int16_t _xOffset   = 80;
  int16_t _yOffset   = 0;

  int16_t _w = 0, _h = 0;
  int16_t _cx = 0, _cy = 0;
  uint16_t _fg = 0xFFFF, _bg = 0x0000;
  uint8_t _size = 1;
  bool _useBg = false;

  void cmd(uint8_t c);
  void dat(uint8_t d);
  void dat16(uint16_t d);
  void initSequence();
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void drawChar(int16_t x, int16_t y, char c);
};

#endif // MINI_TFT_H
