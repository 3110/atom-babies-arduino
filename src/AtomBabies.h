#pragma once

#define FASTLED_INTERNAL
#include <M5Atom.h>

#include "Debug.h"
#include "plugins/AbstractAtomBabiesPlugin.h"

namespace M5Stack_AtomBabies {

enum FacePosition {
    FaceTop = 0,
    FaceUp,
    FaceNormal,
    FaceRight,
    FaceLeft,
    FaceDown,
    FaceBottom,
};

enum FaceOrientation {
    OrientationNormal = 0,
    OrientationRight,
    OrientationLeft,
    OrientationUpsideDown,
};

typedef struct {
    uint8_t loop;
    uint16_t open;
    uint16_t close;
    uint16_t interval;
} BlinkParam;

typedef struct {
    uint16_t before;
    uint16_t after;
} BowParam;

class AtomBabies {
public:
    static const char VERSION[];
    static constexpr size_t WIDTH = 5;
    static constexpr size_t HEIGHT = 5;
    static constexpr size_t MAX_POSITION = WIDTH * HEIGHT;
    static const CRGB DEFAULT_EYE_COLOR;
    static const CRGB DEFAULT_CHEEK_COLOR;
    static const CRGB DEFAULT_BACKGROUND_COLOR;
    static const BlinkParam DEFAULT_BLINK;
    static const BowParam DEFAULT_BOW;

    static constexpr size_t N_POSITIONS = 2;
    static constexpr float GRAVITY_THRESHOLD = 0.75;
    static constexpr float DEFAULT_TOUCH_THRESHOLD = 0.3;

    static constexpr size_t MAX_PLUGINS = 10;

    AtomBabies(FacePosition position = FaceNormal,
               FaceOrientation orientation = OrientationNormal,
               const CRGB& eyeColor = DEFAULT_EYE_COLOR,
               const CRGB& cheekColor = DEFAULT_CHEEK_COLOR,
               const CRGB& backgroundColor = DEFAULT_BACKGROUND_COLOR);
    virtual ~AtomBabies(void);

    virtual bool begin(void);
    virtual bool update(void);

    virtual void openEyes(void);
    virtual void closeEyes(void);

    virtual void setCheeks(void);
    virtual void clearCheeks(void);

    virtual void startBlink(void);
    virtual void stopBlink(void);
    virtual void toggleBlink(void);
    virtual bool isBlinking(void) const;
    virtual void setBlinkParam(const BlinkParam& param);

    virtual bool updateOrientation(void);
    virtual bool isAutoOrientation(void) const;
    virtual void setAutoOrientation(bool autoOrientation);
    virtual bool toggleAutoOrientation(void);
    virtual FaceOrientation detectOrientation(void);

    virtual AtomBabies& setOrientation(FaceOrientation orientation);
    virtual AtomBabies& setFace(FacePosition position);
    virtual AtomBabies& setEyesColor(const CRGB& color);
    virtual AtomBabies& setCheeksColor(const CRGB& color);
    virtual AtomBabies& setBackgroundColor(const CRGB& color);

    virtual void display(void);
    virtual void clear(bool partial = false);
    virtual void fill(const CRGB& color);
    virtual void displayData(const CRGB& color, const uint8_t pos[],
                             size_t len);

    virtual void bow(bool deep = false);
    virtual void setBowParam(const BowParam& param);

    virtual bool isPressed(void);
    virtual bool wasPressed(void);

    virtual void displayDigits(const CRGB& color, uint16_t val,
                               uint16_t interval);
    virtual void scrollDigits(const CRGB& color, uint16_t val,
                              uint16_t interval);

    virtual bool isTouched(float threshold = DEFAULT_TOUCH_THRESHOLD);

    virtual bool addPlugin(AbstractAtomBabiesPlugin& plugin);

    virtual void _doBlink(void);  // called from thread

protected:
    virtual void displayDigit(const CRGB& color, uint8_t digit);

    virtual void setEyes(const CRGB& color);
    virtual void setCheeks(const CRGB& color);

    virtual void setLED(const CRGB& color, uint8_t position);
    virtual void setLEDs(const CRGB& color,
                         const uint8_t (&position)[N_POSITIONS]);
    virtual uint8_t getLEDPosition(uint8_t position);

    virtual void displayScrollBuffer(const CRGB& color, uint16_t interval);

private:
    FacePosition _position;
    bool _autoOrientation;
    FaceOrientation _orientation;
    CRGB _eyeColor;
    CRGB _cheekColor;
    CRGB _backgroundColor;
    bool _blinking;
    BlinkParam _blinkParam;
    BowParam _bowParam;

    size_t _n_plugins;
    AbstractAtomBabiesPlugin* _plugins[MAX_PLUGINS];
};

}  // namespace M5Stack_AtomBabies