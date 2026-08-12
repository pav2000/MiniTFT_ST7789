# MiniTFT
A minimal SPI driver for 240x240 ST7789 displays, designed as a replacement for the `Adafruit_GFX` + `Adafruit_ST7789` + `Adafruit_BusIO` stack in projects where those libraries exceed available flash memory (e.g., ATmega328 / Arduino Uno with 32 KB of flash).

It supports only the essentials for simple information or diagnostic displays: drawing filled rectangles and lines, and rendering text using a compact built-in 5x7 font (uppercase letters, numbers, and basic punctuation; lowercase letters are automatically converted to uppercase).

The class inherits from `Print`, so `print()` and `println()` work with all standard overloads—numbers, `String` objects, `F()`-macro strings from PROGMEM, etc.—without the need for manual code duplication.
-------------------------------------------------------------------------
Минимальный SPI-драйвер для экранов ST7789 240x240, написанный как замена
связке `Adafruit_GFX + Adafruit_ST7789 + Adafruit_BusIO` для проектов, где
эти библиотеки не влезают во флеш-память (например, ATmega328 / Arduino Uno,
32 КБ флеша).

Поддерживается только то, что нужно для простых информационных/
диагностических экранов: заливка прямоугольников и линий, вывод текста
компактным встроенным шрифтом 5x7 (заглавные буквы, цифры, базовая
пунктуация — строчные буквы автоматически приводятся к заглавным).

Класс наследует `Print`, поэтому `print()` / `println()` работают со всеми
стандартными перегрузками — числа, `String`, `F()`-строки из PROGMEM и т.д.
— без ручного дублирования кода.

## Установка

Скопируйте папку `MiniTFT` целиком в `Documents/Arduino/libraries/`
(Windows/macOS/Linux) — после перезапуска Arduino IDE библиотека появится
в `Скетч -> Подключить библиотеку -> MiniTFT`, а пример — в
`Файл -> Примеры -> MiniTFT -> BasicTest`.

## Быстрый старт

```cpp
#include <MiniTFT.h>

MiniTFT tft(10, 9);

void setup() {
  tft.init(240, 240);
  tft.fillScreen(MTFT_BLACK);
  tft.setTextColor(MTFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(F("Hello!"));
}

void loop() {}
```

CS у большинства дешёвых модулей ST7789 разведён на самой плате экрана и
посажен на GND — отдельный пин для CS драйверу не требуется и не
запрашивается в конструкторе.

## Если экран не заработал "из коробки"

Параметры по умолчанию (`SPI_MODE3`, 8 МГц, `MADCTL=0xA0`, смещение
X=+80/Y=0) подобраны экспериментально для КОНКРЕТНОЙ партии модулей 1.3"
240x240 и **не универсальны для всех модулей ST7789**. У других панелей
эти параметры могут отличаться. Настройте их ДО вызова `init()`:

```cpp
tft.setSpiSpeed(8000000UL); // скорость SPI в Гц
tft.setSpiMode(SPI_MODE0);  // SPI_MODE0..SPI_MODE3
tft.setMadctl(0x00);        // регистр 0x36 контроллера — ориентация/цвет
tft.setOffset(0, 0);        // смещение X/Y в CASET/RASET
```

### Как подбирать параметры (кратко, по опыту отладки)

1. **SPI_MODE.** У Adafruit в заголовке `Adafruit_ST77xx.h` буквально
   написан комментарий: `spiMode = SPI_MODE0; // Certain display needs
   MODE3 instead`. Если экран совсем не откликается (ни разу, ни на одном
   цвете) — первым делом попробуйте оба режима, `MODE0` и `MODE3`.

2. **Смещение (offset).** У ST7789 физический GRAM контроллера часто
   рассчитан на 240x320, даже если видимая матрица — только 240x240
   (актуально для многих 1.3" панелей). Тогда нужно смещение в 80 пикселей
   по одной из осей в `CASET`/`RASET`, иначе пиксели пишутся в невидимую
   область памяти контроллера — SPI-обмен при этом полностью корректен,
   просто ничего не видно.

3. **MADCTL и то, к какой оси идёт offset — связаны.** Если в offset
   попали смещением не в ту ось (X вместо Y или наоборот) — экран тоже
   останется тёмным. У Adafruit_ST7789 это зависит от `rotation`: для
   `rotation=0` типично `MADCTL=0xC0` со смещением по Y, для `rotation=1`
   — `MADCTL=0xA0` со смещением по X. Если есть работающий пример на
   `Adafruit_ST7789` для вашей платы — посмотрите, какой `rotation`
   используется, и возьмите соответствующую пару значений.

4. **Надёжность после включения питания.** На одной из плат в процессе
   отладки одного прохода инициализации сразу после подачи питания было
   недостаточно — экран иногда оставался тёмным. Повтор полной
   последовательности инициализации дважды подряд (уже реализовано в
   `init()`) решил эту проблему стабильно на всех проверенных циклах
   включения/выключения питания. Если у вас похожая проблема "работает не
   всегда" — это первое, что стоит попробовать увеличить (например, три
   раза подряд вместо двух, или больше начальной задержки).

Если ни один из этих параметров не помогает и экран вообще ни на что не
реагирует (ни на одном сочетании SPI_MODE/offset/MADCTL) — вероятно, дело
не в софте, а в самой плате/уровнях/проводке (проверьте питание, RES/DC
пины, целостность пайки), либо конкретно эта партия модулей несовместима
с прямым SPI без уровневого преобразователя, либо наоборот.

## API

| Метод | Назначение |
|---|---|
| `MiniTFT(rstPin, dcPin)` | Конструктор |
| `setSpiSpeed(hz)` / `setSpiMode(mode)` / `setMadctl(v)` / `setOffset(x,y)` | Настройка перед `init()` |
| `init(w, h)` | Инициализация экрана |
| `fillScreen(color)` / `fillRect(...)` / `fillRoundRect(...)` (радиус игнорируется) | Заливка |
| `drawFastHLine(...)` / `drawFastVLine(...)` / `drawPixel(...)` | Линии/точки |
| `setTextColor(fg)` / `setTextColor(fg, bg)` | Цвет текста (без / с фоном) |
| `setCursor(x, y)` / `setTextSize(n)` | Курсор и размер текста |
| `print(...)` / `println(...)` | Унаследовано от `Print` — все стандартные перегрузки |
| `width()` / `height()` | Текущие размеры экрана |

Цветовые константы: `MTFT_BLACK`, `MTFT_WHITE`, `MTFT_RED`, `MTFT_GREEN`,
`MTFT_BLUE`, `MTFT_CYAN`, `MTFT_MAGENTA`, `MTFT_YELLOW`, `MTFT_GREY`
(значения RGB565, совпадают с `ST77XX_*` из Adafruit-библиотек).
