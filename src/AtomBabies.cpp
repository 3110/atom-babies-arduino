#include "AtomBabies.h"

#include "AtomBabiesVersion.h"

namespace M5Stack_AtomBabies {

constexpr bool ENABLE_SERIAL = true;
constexpr bool ENABLE_I2C = true;
constexpr bool ENABLE_DISPLAY = true;

constexpr BaseType_t BLINK_TASK_CORE_ID = 1;
constexpr uint32_t BLINK_TASK_STACK_DEPTH = 4096;
constexpr UBaseType_t BLINK_TASK_PRIORITY = 1;
constexpr char BLINK_TASK_NAME[] = "BlinkTask";

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

size_t bufPos = 0;
const size_t SCROLL_BUFFER_SIZE = DIGITS_SIZE[8] * AtomBabies::WIDTH;
uint8_t SCROLL_BUFFER[SCROLL_BUFFER_SIZE] = {0};

constexpr char AtomBabies::VERSION[] = ATOM_BABIES_VERSION;

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
      _autoOrientation(true),
      _orientation(orientation),
      _eyeColor(eyeColor),
      _cheekColor(cheekColor),
      _backgroundColor(backgroundColor),
      _blinking(false),
      _blinkParam(DEFAULT_BLINK),
      _bowParam(DEFAULT_BOW),
      _n_plugins(0),
      _plugins{0} {
}

AtomBabies::~AtomBabies(void) {
    for (size_t p = 0; p < MAX_PLUGINS; ++p) {
        this->_plugins[p] = nullptr;
    }
    this->_n_plugins = 0;
}

bool AtomBabies::begin(void) {
    M5.begin(ENABLE_SERIAL, ENABLE_I2C, ENABLE_DISPLAY);
    M5.IMU.Init();
    xTaskCreatePinnedToCore(blinkTask, BLINK_TASK_NAME, BLINK_TASK_STACK_DEPTH,
                            this, BLINK_TASK_PRIORITY, nullptr,
                            BLINK_TASK_CORE_ID);
    SERIAL_PRINTF_LN("ATOM Babies v%s", VERSION);
    updateOrientation();
    for (size_t p = 0; p < this->_n_plugins; ++p) {
        this->_plugins[p]->begin(*this);
    }
    return true;
}

bool AtomBabies::update(void) {
    M5.update();
    updateOrientation();
    for (size_t p = 0; p < this->_n_plugins; ++p) {
        this->_plugins[p]->update(*this);
    }
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

bool AtomBabies::updateOrientation(void) {
    if (this->_autoOrientation) {
        const FaceOrientation o = detectOrientation();
        if (o != this->_orientation) {
            this->_orientation = o;
            return true;
        }
    }
    return false;
}

bool AtomBabies::isAutoOrientation(void) const {
    return this->_autoOrientation;
}

void AtomBabies::setAutoOrientation(bool autoOrientation) {
    this->_autoOrientation = autoOrientation;
}

bool AtomBabies::toggleAutoOrientation(void) {
    this->_autoOrientation = !this->_autoOrientation;
    return this->_autoOrientation;
}

AtomBabies& AtomBabies::setOrientation(FaceOrientation orientation) {
    this->_orientation = orientation;
    setAutoOrientation(false);
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

void AtomBabies::displayData(const CRGB& color, const uint8_t pos[],
                             size_t len) {
    for (size_t p = 0; p < len; ++p) {
        setLED(color, pos[p]);
    }
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

void AtomBabies::displayDigits(const CRGB& color, uint16_t val,
                               uint16_t interval) {
    size_t n_digits = getDigit(val);
    uint8_t digit = 0;
    size_t m = 0;
    clear();
    for (size_t d = 0; d < n_digits; ++d) {
        m = n_digits - (d + 1);
        digit = static_cast<uint8_t>(val / pow(10, m));
        val %= static_cast<uint16_t>(pow(10, m));
        displayDigit(color, digit);
        delay(interval);
        clear();
        delay(interval);
    }
}

void AtomBabies::scrollDigits(const CRGB& color, uint16_t val,
                              uint16_t interval) {
    size_t n_digits = getDigit(val);
    uint8_t digit = 0;
    size_t m = 0;
    bufPos = 0;
    clear();
    for (size_t d = 0; d < n_digits; ++d) {
        m = n_digits - (d + 1);
        digit = static_cast<uint8_t>(val / pow(10, m));
        val %= static_cast<uint16_t>(pow(10, m));
        for (size_t x = 1; x <= WIDTH; ++x) {
            for (size_t p = 0; p < DIGITS_SIZE[digit]; ++p) {
                if (DIGITS[digit][p] % WIDTH == x) {
                    SCROLL_BUFFER[bufPos] =
                        WIDTH + WIDTH * static_cast<uint8_t>(
                                            (DIGITS[digit][p] - 1) / WIDTH);
                    ++bufPos;
                }
            }
            displayScrollBuffer(color, interval);
        }
    }
    for (size_t x = 1; x <= WIDTH; ++x) {
        displayScrollBuffer(color, interval);
    }
}

FaceOrientation AtomBabies::detectOrientation(void) {
    float ax, ay, az;
    M5.IMU.getAccelData(&ax, &ay, &az);
    if (ay >= GRAVITY_THRESHOLD) {
        return OrientationNormal;
    } else if (ax >= GRAVITY_THRESHOLD) {
        return OrientationRight;
    } else if (ax <= -GRAVITY_THRESHOLD) {
        return OrientationLeft;
    } else if (ay <= -GRAVITY_THRESHOLD) {
        return OrientationUpsideDown;
    } else {
        return this->_orientation;
    }
}

void AtomBabies::displayDigit(const CRGB& color, uint8_t digit) {
    if (digit >= 10) {
        return;
    }
    const uint8_t* pos = DIGITS[digit];
    const size_t size = DIGITS_SIZE[digit];
    displayData(color, pos, size);
}

bool AtomBabies::isTouched(float threshold) {
    float ax, ay, az;
    M5.IMU.getAccelData(&ax, &ay, &az);
    // SERIAL_PRINTF_LN("Accel: x = %.1f y = %.1f, z = %.1f", ax, ay, az);
    if (this->_orientation == OrientationNormal ||
        this->_orientation == OrientationUpsideDown) {
        return (abs(ax) >= threshold || 1.0 - abs(ay) >= threshold ||
                abs(az) >= threshold);
    } else if (this->_orientation == OrientationLeft ||
               this->_orientation == OrientationRight) {
        return (1.0 - abs(ax) >= threshold || abs(ay) >= threshold ||
                abs(az) >= threshold);
    }
    return false;
}

bool AtomBabies::addPlugin(AbstractAtomBabiesPlugin& plugin) {
    if (this->_n_plugins + 1 >= MAX_PLUGINS) {
        return false;
    }
    this->_plugins[this->_n_plugins] = &plugin;
    ++this->_n_plugins;
    SERIAL_PRINTF_LN("Adding %s Plugin", plugin.getName());
    return true;
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
    displayData(color, position, N_POSITIONS);
}

uint8_t AtomBabies::getLEDPosition(uint8_t position) {
    if (position == 0) {
        return 0;
    }
    return ORIENTATIONS[this->_orientation][position - 1];
}

void AtomBabies::displayScrollBuffer(const CRGB& color, uint16_t interval) {
    // Display
    for (size_t p = 0; p < bufPos; ++p) {
        if (SCROLL_BUFFER[p] == 0) {
            continue;
        }
        setLED(color, SCROLL_BUFFER[p]);
    }
    delay(interval);
    // clear
    for (size_t p = 0; p < bufPos; ++p) {
        if (SCROLL_BUFFER[p] == 0) {
            continue;
        }
        setLED(this->_backgroundColor, SCROLL_BUFFER[p]);
    }
    // scroll
    for (size_t p = 0; p < bufPos; ++p) {
        if (SCROLL_BUFFER[p] == 0) {
            continue;
        }
        --SCROLL_BUFFER[p];
        if (SCROLL_BUFFER[p] % WIDTH == 0) {
            SCROLL_BUFFER[p] = 0;
        }
    }
}

}  // namespace M5Stack_AtomBabies