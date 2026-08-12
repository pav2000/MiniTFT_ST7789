#include "MiniTFT.h"

// ============================== ШРИФТ 5x7 ===================================
// Компактный шрифт: заглавные буквы, цифры, базовая пунктуация (48 глифов,
// 240 байт в PROGMEM). Строчные буквы автоматически приводятся к заглавным
// в write(). Символы вне этого набора просто не рисуются (место под них на
// строке всё равно сдвигается, чтобы разметка не "плыла").
static const char FONT_CHARS[] PROGMEM =
  " !%()+,-./0123456789:=ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const uint8_t FONT_DATA[48][5] PROGMEM = {
  { 0x00, 0x00, 0x00, 0x00, 0x00 }, // ' '
  { 0x00, 0x2F, 0x00, 0x00, 0x00 }, // '!'
  { 0x63, 0x10, 0x08, 0x26, 0x41 }, // '%'
  { 0x1C, 0x22, 0x41, 0x00, 0x00 }, // '('
  { 0x00, 0x00, 0x41, 0x22, 0x1C }, // ')'
  { 0x08, 0x08, 0x3E, 0x08, 0x08 }, // '+'
  { 0x40, 0x20, 0x00, 0x00, 0x00 }, // ','
  { 0x08, 0x08, 0x08, 0x08, 0x08 }, // '-'
  { 0x60, 0x60, 0x00, 0x00, 0x00 }, // '.'
  { 0x10, 0x08, 0x04, 0x02, 0x01 }, // '/'
  { 0x3E, 0x51, 0x49, 0x45, 0x3E }, // '0'
  { 0x00, 0x42, 0x7F, 0x40, 0x00 }, // '1'
  { 0x42, 0x61, 0x51, 0x49, 0x46 }, // '2'
  { 0x21, 0x41, 0x49, 0x4D, 0x32 }, // '3'
  { 0x18, 0x14, 0x12, 0x7F, 0x10 }, // '4'
  { 0x27, 0x45, 0x45, 0x45, 0x39 }, // '5'
  { 0x3C, 0x4A, 0x49, 0x49, 0x30 }, // '6'
  { 0x01, 0x71, 0x09, 0x05, 0x03 }, // '7'
  { 0x36, 0x49, 0x49, 0x49, 0x36 }, // '8'
  { 0x06, 0x09, 0x49, 0x69, 0x1E }, // '9'
  { 0x00, 0x00, 0x36, 0x00, 0x00 }, // ':'
  { 0x0A, 0x0A, 0x0A, 0x0A, 0x0A }, // '='
  { 0x7E, 0x09, 0x09, 0x09, 0x7E }, // 'A'
  { 0x7F, 0x49, 0x49, 0x49, 0x36 }, // 'B'
  { 0x3E, 0x41, 0x41, 0x41, 0x22 }, // 'C'
  { 0x7F, 0x41, 0x41, 0x41, 0x3E }, // 'D'
  { 0x7F, 0x49, 0x49, 0x49, 0x41 }, // 'E'
  { 0x7F, 0x09, 0x09, 0x09, 0x01 }, // 'F'
  { 0x3E, 0x41, 0x49, 0x49, 0x3A }, // 'G'
  { 0x7F, 0x08, 0x08, 0x08, 0x7F }, // 'H'
  { 0x00, 0x41, 0x7F, 0x41, 0x00 }, // 'I'
  { 0x20, 0x40, 0x40, 0x41, 0x3F }, // 'J'
  { 0x7F, 0x08, 0x14, 0x22, 0x41 }, // 'K'
  { 0x7F, 0x40, 0x40, 0x40, 0x40 }, // 'L'
  { 0x7F, 0x02, 0x04, 0x02, 0x7F }, // 'M'
  { 0x7F, 0x02, 0x04, 0x08, 0x7F }, // 'N'
  { 0x3E, 0x41, 0x41, 0x41, 0x3E }, // 'O'
  { 0x7F, 0x09, 0x09, 0x09, 0x06 }, // 'P'
  { 0x3E, 0x41, 0x51, 0x21, 0x5E }, // 'Q'
  { 0x7F, 0x09, 0x19, 0x29, 0x46 }, // 'R'
  { 0x46, 0x49, 0x49, 0x49, 0x31 }, // 'S'
  { 0x01, 0x01, 0x7F, 0x01, 0x01 }, // 'T'
  { 0x3F, 0x40, 0x40, 0x40, 0x3F }, // 'U'
  { 0x1F, 0x20, 0x40, 0x20, 0x1F }, // 'V'
  { 0x3F, 0x40, 0x38, 0x40, 0x3F }, // 'W'
  { 0x63, 0x14, 0x08, 0x14, 0x63 }, // 'X'
  { 0x03, 0x04, 0x78, 0x04, 0x03 }, // 'Y'
  { 0x61, 0x51, 0x49, 0x45, 0x43 }, // 'Z'
};
static const uint8_t FONT_COUNT = 48;

// ============================== КОНСТРУКТОР / НАСТРОЙКА =====================

MiniTFT::MiniTFT(uint8_t rstPin, uint8_t dcPin)
  : _rstPin(rstPin), _dcPin(dcPin) {}

void MiniTFT::setSpiSpeed(uint32_t hz) { _spiSpeed = hz; }
void MiniTFT::setSpiMode(uint8_t mode) { _spiMode = mode; }
void MiniTFT::setMadctl(uint8_t madctl) { _madctl = madctl; }
void MiniTFT::setOffset(int16_t xOffset, int16_t yOffset) {
  _xOffset = xOffset;
  _yOffset = yOffset;
}

// ============================== НИЗКОУРОВНЕВЫЕ КОМАНДЫ ======================

void MiniTFT::cmd(uint8_t c) { digitalWrite(_dcPin, LOW);  SPI.transfer(c); }
void MiniTFT::dat(uint8_t d) { digitalWrite(_dcPin, HIGH); SPI.transfer(d); }
void MiniTFT::dat16(uint16_t d) {
  digitalWrite(_dcPin, HIGH);
  SPI.transfer(d >> 8);
  SPI.transfer(d & 0xFF);
}

void MiniTFT::initSequence() {
  digitalWrite(_rstPin, HIGH); delay(10);
  digitalWrite(_rstPin, LOW);  delay(20);
  digitalWrite(_rstPin, HIGH); delay(150);

  cmd(0x01); delay(150);           // SWRESET
  cmd(0x11); delay(120);           // SLPOUT
  cmd(0x3A); dat(0x55); delay(10); // COLMOD: 16 бит/пиксель
  cmd(0x36); dat(_madctl);         // MADCTL: ориентация/порядок цвета
  cmd(0x21); delay(10);            // INVON
  cmd(0x13); delay(10);            // NORON
  cmd(0x29); delay(10);            // DISPON
}

void MiniTFT::init(uint16_t w, uint16_t h) {
  _w = w; _h = h;
  pinMode(_dcPin, OUTPUT);
  pinMode(_rstPin, OUTPUT);

  delay(100); // стабилизация питания после старта — важно на некоторых платах

  SPI.begin();
  SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, _spiMode));

  // Инициализация выполняется ДВАЖДЫ подряд. Экспериментально подтверждено
  // на реальном железе: одного прохода сразу после подачи питания иногда
  // недостаточно для стабильного запуска панели, повтор решает это надёжно.
  initSequence();
}

void MiniTFT::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  x0 += _xOffset; x1 += _xOffset;
  y0 += _yOffset; y1 += _yOffset;
  cmd(0x2A); dat16(x0); dat16(x1); // CASET
  cmd(0x2B); dat16(y0); dat16(y1); // RASET
  cmd(0x2C);                       // RAMWR
}

// ============================== ГРАФИЧЕСКИЕ ПРИМИТИВЫ ========================

void MiniTFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (x >= _w || y >= _h || w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > _w) w = _w - x;
  if (y + h > _h) h = _h - y;
  if (w <= 0 || h <= 0) return;

  setAddrWindow(x, y, x + w - 1, y + h - 1);
  digitalWrite(_dcPin, HIGH);
  uint8_t hi = color >> 8, lo = color & 0xFF;
  uint32_t n = (uint32_t)w * (uint32_t)h;
  for (uint32_t i = 0; i < n; i++) { SPI.transfer(hi); SPI.transfer(lo); }
}

void MiniTFT::fillScreen(uint16_t color) { fillRect(0, 0, _w, _h, color); }

void MiniTFT::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t /*r*/, uint16_t color) {
  fillRect(x, y, w, h, color); // радиус игнорируется — экономия флеша
}

void MiniTFT::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void MiniTFT::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void MiniTFT::drawPixel(int16_t x, int16_t y, uint16_t color) {
  fillRect(x, y, 1, 1, color);
}

// ============================== ТЕКСТ =======================================

void MiniTFT::setTextColor(uint16_t fg) { _fg = fg; _useBg = false; }
void MiniTFT::setTextColor(uint16_t fg, uint16_t bg) { _fg = fg; _bg = bg; _useBg = true; }
void MiniTFT::setCursor(int16_t x, int16_t y) { _cx = x; _cy = y; }
void MiniTFT::setTextSize(uint8_t s) { _size = (s == 0) ? 1 : s; }
void MiniTFT::setTextWrap(bool) { /* автоперенос не поддерживается */ }

void MiniTFT::drawChar(int16_t x, int16_t y, char c) {
  if (c >= 'a' && c <= 'z') c -= 32; // приводим к верхнему регистру
  int8_t idx = -1;
  for (uint8_t i = 0; i < FONT_COUNT; i++) {
    if ((char)pgm_read_byte(&FONT_CHARS[i]) == c) { idx = i; break; }
  }
  if (idx < 0) return; // неизвестный символ — просто не рисуем

  for (uint8_t col = 0; col < 5; col++) {
    uint8_t line = pgm_read_byte(&(FONT_DATA[idx][col]));
    for (uint8_t row = 0; row < 7; row++) {
      bool on = line & (1 << row);
      if (!on && !_useBg) continue; // прозрачный фон — "пустые" пиксели не трогаем
      uint16_t color = on ? _fg : _bg;
      if (_size == 1) fillRect(x + col, y + row, 1, 1, color);
      else fillRect(x + col * _size, y + row * _size, _size, _size, color);
    }
  }
}

// write() — единственный метод, обязательный для реализации Print. Через
// него автоматически работают print()/println() со ВСЕМИ стандартными
// перегрузками (String, char*, числа, F()-строки из PROGMEM и т.д.).
size_t MiniTFT::write(uint8_t c) {
  if (c == '\n') {
    _cx = 0;
    _cy += 8 * _size;
  } else {
    drawChar(_cx, _cy, (char)c);
    _cx += 6 * _size;
  }
  return 1;
}
