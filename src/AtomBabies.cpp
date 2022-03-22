#include "AtomBabies.h"

#include "AtomBabiesVersion.h"

namespace M5Stack_AtomBabies {

const bool ENABLE_SERIAL = true;
const bool ENABLE_I2C = true;
const bool ENABLE_DISPLAY = true;

const BaseType_t BLINK_TASK_CORE_ID = 1;
const uint32_t BLINK_TASK_STACK_DEPTH = 4096;
const UBaseType_t BLINK_TASK_PRIORITY = 1;
const char BLINK_TASK_NAME[] = "BlinkTask";

void blinkTask(void* arg) {
    AtomBabies* babies = reinterpret_cast<AtomBabies*>(arg);
    while (true) {
        if (babies->isBlinking()) {
            babies->_doBlink();
        }
        vTaskDelay(1);
    }
}

const uint8_t EYE_POSITIONS[][AtomBabies::N_POSITIONS] = {
    {2, 4},    // Top
    {7, 9},    // Up
    {12, 14},  // Normal
    {11, 13},  // Right
    {13, 15},  // Left
    {17, 19},  // Down
    {22, 24},  // Bottom
};

const uint8_t CHEEK_POSITIONS[][AtomBabies::N_POSITIONS] = {
    {6, 10},   // Top
    {11, 15},  // Up
    {16, 20},  // Normal
    {0, 19},   // Right
    {0, 17},   // Left
    {21, 25},  // Down
    {0, 0},    // Bottom
};

const uint8_t ORIENTATIONS[][AtomBabies::MAX_POSITION] = {
    {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
     14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25},  // Normal
    {21, 16, 11, 6,  1,  22, 17, 12, 7,  2,  23, 18, 13,
     8,  3,  24, 19, 14, 9,  4,  25, 20, 15, 10, 5},  // Right
    {5,  10, 15, 20, 25, 4,  9,  14, 19, 24, 3,  8, 13,
     18, 23, 2,  7,  12, 17, 22, 1,  6,  11, 16, 21},  // Left
    {25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13,
     12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1},  // Upside Down
};

const uint8_t DIGIT0[] = {2, 3, 4, 7, 9, 12, 14, 17, 19, 22, 23, 24};
const uint8_t DIGIT1[] = {3, 7, 8, 13, 18, 22, 23, 24};
const uint8_t DIGIT2[] = {2, 3, 4, 9, 12, 13, 14, 17, 22, 23, 24};
const uint8_t DIGIT3[] = {2, 3, 4, 9, 12, 13, 14, 19, 22, 23, 24};
const uint8_t DIGIT4[] = {2, 4, 7, 9, 12, 13, 14, 19, 24};
const uint8_t DIGIT5[] = {2, 3, 4, 7, 12, 13, 14, 19, 22, 23, 24};
const uint8_t DIGIT6[] = {2, 3, 4, 7, 12, 13, 14, 17, 19, 22, 23, 24};
const uint8_t DIGIT7[] = {2, 3, 4, 9, 14, 19, 24};
const uint8_t DIGIT8[] = {2, 3, 4, 7, 9, 12, 13, 14, 17, 19, 22, 23, 24};
const uint8_t DIGIT9[] = {2, 3, 4, 7, 9, 12, 13, 14, 19, 22, 23, 24};

const uint8_t* DIGITS[] = {DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4,
                           DIGIT5, DIGIT6, DIGIT7, DIGIT8, DIGIT9};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

const size_t DIGITS_SIZE[] = {
    ARRAY_SIZE(DIGIT0), ARRAY_SIZE(DIGIT1), ARRAY_SIZE(DIGIT2),
    ARRAY_SIZE(DIGIT3), ARRAY_SIZE(DIGIT4), ARRAY_SIZE(DIGIT5),
    ARRAY_SIZE(DIGIT6), ARRAY_SIZE(DIGIT7), ARRAY_SIZE(DIGIT8),
    ARRAY_SIZE(DIGIT9),
};

const char AtomBabies::VERSION[] = ATOM_BABIES_VERSION;

const CRGB AtomBabies::DEFAULT_EYE_COLOR(0x00, 0x64, 0x00);
const CRGB AtomBabies::DEFAULT_CHEEK_COLOR(0x64, 0x00, 0x00);
const CRGB AtomBabies::DEFAULT_BACKGROUND_COLOR(0x00, 0x00, 0x00);

const BlinkParam AtomBabies::DEFAULT_BLINK = {
    2,    // loop
    500,  // open
    100,  // close
    2000  // interval
};

const BowParam AtomBabies::DEFAULT_BOW = {
    500,  // before
    500,  // after
};

size_t getDigit(uint16_t val) {
    size_t digit = 1;
    while (val /= 10) {
        ++digit;
    }
    return digit;
}

AtomBabies::AtomBabies(FacePosition position, FaceOrientation orientation,
                       const CRGB& eyeColor, const CRGB& cheekColor,
                       const CRGB& backgroundColor)
    : _position(position),
      _orientation(orientation),
      _eyeColor(eyeColor),
      _cheekColor(cheekColor),
      _backgroundColor(backgroundColor),
      _blinking(false),
      _blinkParam(DEFAULT_BLINK),
      _bowParam(DEFAULT_BOW) {
}

AtomBabies::~AtomBabies(void) {
}

bool AtomBabies::begin(void) {
    M5.begin(ENABLE_SERIAL, ENABLE_I2C, ENABLE_DISPLAY);
    xTaskCreatePinnedToCore(blinkTask, BLINK_TASK_NAME, BLINK_TASK_STACK_DEPTH,
                            this, BLINK_TASK_PRIORITY, nullptr,
                            BLINK_TASK_CORE_ID);
    SERIAL_PRINTF_LN("ATOM Babies v%s", VERSION);
    return true;
}

bool AtomBabies::update(void) {
    M5.update();
    return true;
}

void AtomBabies::openEyes(void) {
    setEyes(this->_eyeColor);
}

void AtomBabies::closeEyes(void) {
    setEyes(this->_backgroundColor);
}

void AtomBabies::setCheeks(void) {
    setCheeks(this->_cheekColor);
}

void AtomBabies::clearCheeks(void) {
    setCheeks(this->_backgroundColor);
}

void AtomBabies::startBlink(void) {
    this->_blinking = true;
}

void AtomBabies::stopBlink(void) {
    this->_blinking = false;
    delay(this->_blinkParam.open);
}

void AtomBabies::toggleBlink(void) {
    if (isBlinking()) {
        stopBlink();
    } else {
        startBlink();
    }
}

bool AtomBabies::isBlinking(void) const {
    return this->_blinking;
}

void AtomBabies::setBlinkParam(const BlinkParam& param) {
    this->_blinkParam = param;
}

AtomBabies& AtomBabies::setOrientation(FaceOrientation orientation) {
    this->_orientation = orientation;
    return *this;
}

AtomBabies& AtomBabies::setFace(FacePosition position) {
    this->_position = position;
    return *this;
}

AtomBabies& AtomBabies::setEyesColor(const CRGB& color) {
    this->_eyeColor = color;
    return *this;
}

AtomBabies& AtomBabies::setCheeksColor(const CRGB& color) {
    this->_cheekColor = color;
    return *this;
}

AtomBabies& AtomBabies::setBackgroundColor(const CRGB& color) {
    this->_backgroundColor = color;
    return *this;
}

void AtomBabies::display(void) {
    clear();
    openEyes();
    setCheeks();
}

void AtomBabies::clear(bool partial) {
    if (partial) {
        setEyes(this->_backgroundColor);
        setCheeks(this->_backgroundColor);
    } else {
        fill(this->_backgroundColor);
    }
}

void AtomBabies::fill(const CRGB& color) {
    M5.dis.fillpix(color);
}

void AtomBabies::bow(bool deep) {
    setFace(FaceNormal).display();
    delay(this->_bowParam.before);
    setFace(deep ? FaceBottom : FaceDown).display();
    delay(this->_bowParam.after);
    setFace(FaceNormal).display();
}

void AtomBabies::setBowParam(const BowParam& param) {
    this->_bowParam = param;
}

bool AtomBabies::isPressed(void) {
    return M5.Btn.isPressed();
}

bool AtomBabies::wasPressed(void) {
    return M5.Btn.wasPressed();
}

void AtomBabies::displayDigits(uint16_t val, const CRGB& color,
                               uint16_t interval) {
    size_t n = getDigit(val);
    uint8_t digit;
    for (size_t i = n; i != 0; --i) {
        digit = (uint8_t)(val / pow(10, i - 1));
        val %= (uint16_t)pow(10, i - 1);
        clear();
        delay(interval);
        displayDigit(digit, color);
        delay(interval);
    }
    clear();
    delay(interval);
}

void AtomBabies::displayDigit(uint8_t digit, const CRGB& color) {
    if (digit > 10) {
        return;
    }
    const uint8_t* pos = DIGITS[digit];
    const size_t size = DIGITS_SIZE[digit];
    for (size_t p = 0; p < size; ++p) {
        setLED(color, pos[p]);
    }
}

void AtomBabies::_doBlink(void) {
    for (int i = 0, n = random(1, this->_blinkParam.loop + 1); i < n; ++i) {
        if (!isBlinking()) {
            break;
        }
        delay(this->_blinkParam.open);
        closeEyes();
        delay(this->_blinkParam.close);
        openEyes();
    }
    if (isBlinking()) {
        delay(this->_blinkParam.interval);
    }
}

void AtomBabies::setEyes(const CRGB& color) {
    setLEDs(color, EYE_POSITIONS[this->_position]);
}

void AtomBabies::setCheeks(const CRGB& color) {
    setLEDs(color, CHEEK_POSITIONS[this->_position]);
}

void AtomBabies::setLED(const CRGB& color, uint8_t position) {
    position = getLEDPosition(position);
    if (position == 0) {
        return;
    }
    M5.dis.drawpix(position - 1, color);
}

void AtomBabies::setLEDs(const CRGB& color,
                         const uint8_t (&position)[N_POSITIONS]) {
    for (size_t i = 0; i < N_POSITIONS; ++i) {
        setLED(color, position[i]);
    }
}

uint8_t AtomBabies::getLEDPosition(uint8_t position) {
    if (position == 0) {
        return 0;
    }
    return ORIENTATIONS[this->_orientation][position - 1];
}

}  // namespace M5Stack_AtomBabies