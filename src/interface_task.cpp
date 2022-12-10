#include <FastLED.h>
#include <HX711.h>
#include <Adafruit_VEML7700.h>

#include "interface_task.h"
#include "util.h"

#define COUNT_OF(A) (sizeof(A) / sizeof(A[0]))

CRGB leds[NUM_LEDS];

HX711 scale;

Adafruit_VEML7700 veml = Adafruit_VEML7700();

bool inMenu = false;
bool compatibleMode = false;

static PB_SmartKnobConfig configs[] = {
    // int32_t num_positions;
    // int32_t position;
    // float position_width_radians;
    // float detent_strength_unit;
    // float endstop_strength_unit;
    // float snap_point;
    // char text[51];

    {
        51,
        0,
        10 * PI / 180,
        0.5,
        1,
        1.1,
        "0 Volume\nMedium detents",
    },
    {
        0,
        0,
        10 * PI / 180,
        0,
        1,
        1.1,
        "1 Slide\nNo detents",
    },
    {
        0,
        0,
        8.225806452 * PI / 180,
        2,
        1,
        1.1,
        "2 MediaCtrl\nStrong detents",
    },
    {
        2,
        0,
        60 * PI / 180,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "3 switch On/off\nStrong detent",
    },
    {
        3,
        0,
        45 * PI / 180,
        0.5,
        1,
        0.55,
        "4 setup2 menu\nMedium detents",
    },
    {
        4,
        0,
        45 * PI / 180,
        0.5,
        1,
        0.55,
        "5 Menu\nMedium detents",
    },
    {
        0,
        0,
        10 * PI / 180,
        0,
        1,
        1.1,
        "6 Unbounded\nNo detents",
    },
    {
        0,
        0,
        10 * PI / 180,
        0.5,
        1,
        1.1,
        "7 Unbounded\nWith detents",
    },
    {
        0,
        0,
        10 * PI / 180,
        2,
        1,
        1.1,
        "8 Unbounded\nStrong detents",
    },
};

InterfaceTask::InterfaceTask(const uint8_t task_core, MotorTask &motor_task) : Task("Interface", 3000, 1, task_core),
                                                                               stream_(),
                                                                               motor_task_(motor_task),
                                                                               plaintext_protocol_(stream_, motor_task_),
                                                                               proto_protocol_(stream_, motor_task_)
{

    log_queue_ = xQueueCreate(10, sizeof(std::string *));
    assert(log_queue_ != NULL);

    knob_state_queue_ = xQueueCreate(1, sizeof(PB_SmartKnobState));
    assert(knob_state_queue_ != NULL);
}

void InterfaceTask::run()
{
    stream_.begin();

    FastLED.addLeds<SK6812, PIN_LED_DATA, GRB>(leds, NUM_LEDS);

#if PIN_SDA >= 0 && PIN_SCL >= 0
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
#endif
    scale.begin(38, 2);

    if (veml.begin())
    {
        veml.setGain(VEML7700_GAIN_2);
        veml.setIntegrationTime(VEML7700_IT_400MS);
    }
    else
    {
        log("ALS sensor not found!");
    }

    motor_task_.setConfig(configs[0]);
    motor_task_.addListener(knob_state_queue_);
    // Start in legacy protocol mode
    plaintext_protocol_.init([this]()
                             { changeConfig(0); });
    SerialProtocol *current_protocol = &plaintext_protocol_;

    ProtocolChangeCallback protocol_change_callback = [this, &current_protocol](uint8_t protocol)
    {
        switch (protocol)
        {
        case SERIAL_PROTOCOL_LEGACY:
            current_protocol = &plaintext_protocol_;
            break;
        case SERIAL_PROTOCOL_PROTO:
            current_protocol = &proto_protocol_;
            break;
        default:
            log("Unknown protocol requested");
            break;
        }
    };

    plaintext_protocol_.setProtocolChangeCallback(protocol_change_callback);
    proto_protocol_.setProtocolChangeCallback(protocol_change_callback);

    // Interface loop:
    while (1)
    {
        PB_SmartKnobState state;
        if (xQueueReceive(knob_state_queue_, &state, 0) == pdTRUE)
        {
            current_protocol->handleState(state);
        }

        current_protocol->loop();

        std::string *log_string;
        while (xQueueReceive(log_queue_, &log_string, 0) == pdTRUE)
        {
            current_protocol->log(log_string->c_str());
            delete log_string;
        }

        updateHardware();

        delay(1);
    }
}

void InterfaceTask::log(const char *msg)
{
    // Allocate a string for the duration it's in the queue; it is free'd by the queue consumer
    std::string *msg_str = new std::string(msg);

    // Put string in queue (or drop if full to avoid blocking)
    xQueueSendToBack(log_queue_, &msg_str, 0);
}

void InterfaceTask::changeConfig(int32_t next)
{
    current_config_ = next;
    getConfigs(next);
    getconf_gui(next);

    char buf_[256];
    snprintf(buf_, sizeof(buf_), "Changing config to %d -- %s", current_config_, configs[current_config_].text);
    log(buf_);
    motor_task_.setConfig(configs[current_config_]);
}

void InterfaceTask::updateHardware()
{
    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;

    const float LUX_ALPHA = 0.005;
    static float lux_avg;
    float lux = veml.readLux();
    lux_avg = lux * LUX_ALPHA + lux_avg * (1 - LUX_ALPHA);
    static uint32_t last_als;
    if (millis() - last_als > 1000)
    {
        snprintf(buf_, sizeof(buf_), "millilux: %.2f", lux * 1000);
        log(buf_);
        last_als = millis();
    }

    if (scale.wait_ready_timeout(100))
    {
        int32_t reading = scale.read();

        static uint32_t last_reading_display;
        if (millis() - last_reading_display > 1000)
        {
            snprintf(buf_, sizeof(buf_), "HX711 reading: %d", reading);
            log(buf_);
            last_reading_display = millis();
        }

        // TODO: calibrate and track (long term moving average) zero point (lower); allow calibration of set point offset
        const int32_t lower = 950000;
        const int32_t upper = 1800000;
        // Ignore readings that are way out of expected bounds
        if (reading >= lower - (upper - lower) && reading < upper + (upper - lower) * 2)
        {
            long value = CLAMP(reading, lower, upper);
            press_value_unit = 1. * (value - lower) / (upper - lower);

            static bool pressed;
            if (!pressed && press_value_unit > 0.75)
            {
                motor_task_.playHaptic(true);
                if (!compatibleMode)
                {
                    delay(300);
                    reading = scale.read();
                    value = CLAMP(reading, lower, upper);
                    press_value_unit = 1. * (value - lower) / (upper - lower);
                    if (press_value_unit > 0.75)
                    {
                        motor_task_.playHaptic(true);
                        if (!compatibleMode)
                        {
                            pressed = true;
                            changeConfig(5);
                            inMenu = true;
                        }
                    }
                    else
                    {
                        pressed = false;
                        if (inMenu)
                        {
                            changeConfig(return_val());
                            inMenu = false;
                        }
                        else
                        {
                            int32_t tmp = clickAct();
                            if (tmp == 1)
                            {
                                changeConfig(4);
                            }
                            else if (tmp == 2)
                            {
                                compatibleMode = true;
                                changeConfig(return_val() + 6);
                            }
                        }
                    }
                }
                else
                {
                    sendpress();
                    while (press_value_unit > 0.75)
                    {
                        reading = scale.read();
                        value = CLAMP(reading, lower, upper);
                        press_value_unit = 1. * (value - lower) / (upper - lower);
                        delay(1);
                    }
                    sendrelease();
                }
            }
            else if (pressed && press_value_unit < 0.25)
            {
                motor_task_.playHaptic(false);
                pressed = false;
            }
        }
    }
    else
    {
        log("HX711 not found.");

        for (uint8_t i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB::Red;
        }
        FastLED.show();
    }

    uint16_t brightness = UINT16_MAX;
    // TODO: brightness scale factor should be configurable (depends on reflectivity of surface)
    brightness = (uint16_t)CLAMP(lux_avg * 13000, (float)1280, (float)32767);

    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        leds[i].setHSV(200 * press_value_unit, 255, brightness >> 8);

        // Gamma adjustment
        leds[i].r = dim8_video(leds[i].r);
        leds[i].g = dim8_video(leds[i].g);
        leds[i].b = dim8_video(leds[i].b);
    }
    FastLED.show();
}
