
#include <NeoPixelBus.h>

#include "board.h"
#include "constants.h"
#include "leds.h"
#include "main.h"
#include "util.h"

namespace LEDs
{
    NeoPixelBus<NEOPIXEL_ORDERING, NeoWs2812xMethod> _strip(Airports::COUNT, Pins::NEOPIXEL);

    bool _forceRedraw = true;

    void setupStrip()
    {
        _strip.Begin();
        clearStrip();
    }

    void loopRedraw()
    {
        static inputs_t prevInputs = Main::system.inputs;
        static status_t prevStatus = Main::system.status;
        static unsigned long nextAnimateAt = 0;
        static unsigned long nextStatusFlashAt = 0;
        static bool statusFlash = false;

        // Redraw all if:
        // - METARs changed (force redraw)
        // - status changed
        // - LDR changed
        // - brightness knobs changed
        // - dimming setting changed
        bool statusChanged = prevStatus != Main::system.status;
        bool ldrChanged = prevInputs.ldr != Main::system.inputs.ldr;
        bool redrawAll = _forceRedraw || statusChanged || ldrChanged;

        // Only wipe LEDs on initial status change
        if (statusChanged)
        {
            clearStrip();
        }

        if (redrawAll)
        {
            if (Main::system.status == CONNECTED_WITH_DATA)
            {
                drawMETARs();
            }
        }

        // Flash status LED when we're in a nonfunctional state
        if (Main::system.status != CONNECTED_WITH_DATA && millis() > nextStatusFlashAt)
        {
            // Don't flash when trying to reconnect
            drawStatus(statusFlash && Main::system.status != DISCONNECTED);
            nextStatusFlashAt = millis() + 1000;
            statusFlash = !statusFlash;
        }

        // Animate:
        // - lightning flashes
        // - windy stations
        if (Main::system.status == CONNECTED_WITH_DATA)
        {
            if (millis() > nextAnimateAt)
            {
                drawAnimationFrame();
                nextAnimateAt = millis() + Config::ANIMATION_FRAME_DURATION;
            }
        }

        _strip.Show();

        prevInputs = Main::system.inputs;
        prevStatus = Main::system.status;
        _forceRedraw = false;
    }

    void drawMETARs()
    {
        for (int i = 0; i < Main::system.metarCount; i++)
        {
            metar_t metar = Main::system.metars[i];

            setStationPixel(metar.ledIndex, getCategoryColor(metar.category));
        }
    }

    void drawAnimationFrame()
    {
        // Windy stations
        for (int i = 0; i < Main::system.metarCount; i++)
        {
            metar_t metar = Main::system.metars[i];
            if (Main::system.settings.wind && max(metar.wind, metar.windGust) > Main::system.settings.windy_kts)
            {
                bool flicker = random(100) < Config::FLICKER_WIND_PERCENT;
                if (flicker)
                {
                    RgbColor flickerColor = getCategoryColor(metar.category).Dim(Config::FLICKER_WIND_DIMMING);
                    setStationPixel(metar.ledIndex, flickerColor);
                }
                else
                {
                    RgbColor color = getCategoryColor(metar.category);
                    setStationPixel(metar.ledIndex, color);
                }
            }
        }

        if (Main::system.settings.lightning)
        {
            for (int i = 0; i < Main::system.metarCount; i++)
            {
                metar_t metar = Main::system.metars[i];
                if (!metar.lightning || metar.category == NA)
                {
                    continue;
                }

                bool flash = random(100) < Config::FLASH_LIGHTNING_PERCENT;
                if (flash)
                {
                    setStationPixel(metar.ledIndex, Colors::LIGHTNING_FLASH);
                }
                else
                {
                    setStationPixel(metar.ledIndex, getCategoryColor(metar.category));
                }
            }
        }
    }

    void drawStatus(bool flash)
    {
        if (flash)
        {
            setStatusPixel(Colors::BLACK);
            return;
        }

        switch (Main::system.status)
        {
        case INITIALIZING:
            Serial.println("INITIALIZING");
            setStatusPixel(Colors::INITIALIZING);
            break;
        case CONNECTED_NO_DATA:
            Serial.println("CONNECTED_NO_DATA");
            setStatusPixel(Colors::CONNECTED_NO_DATA);
            break;
        case DISCONNECTED:
            Serial.println("DISCONNECTED");
            setStatusPixel(Colors::DISCONNECTED);
            break;
        case WIFI_SETUP:
            Serial.println("WIFI_SETUP");
            setStatusPixel(Colors::WIFI_SETUP);
            break;
        }
    }

    uint8_t getDesiredBrightness()
    {
        float userBrightness = (float)Main::system.settings.brightness / 100.0;
        float dimming = 0.0;

        if (false) // auto-dimming
        {
            float minBrightness = 0.05;
            // LDR:
            //   LDR high (1.0) -> room is bright -> less LED dimming -> bright LEDs
            //   LDR low (0.0) -> room is dark -> more LED dimming -> dim LEDs
            //
            // Min brightness knob:
            //   Min brightness high (1.0) -> less LED dimming -> bright LEDs
            //   Min brightness low (0.0) -> more LED dimming -> dim LEDs
            dimming = (1.0 - Main::system.inputs.ldr) * (1.0 - minBrightness);
        }

        return clamp((userBrightness - dimming) * (float)Features::MASTER_MAX_BRIGHTNESS, Features::MASTER_MIN_BRIGHTNESS, Features::MASTER_MAX_BRIGHTNESS);
    }

    void setStatus(status_t status)
    {
        _forceRedraw = true;
    }

    void clearStrip()
    {
        for (int i = 0; i < Airports::COUNT; i++)
        {
            _strip.SetPixelColor(i, Colors::BLACK);
        }
    }

    void setStationPixel(int index, RgbColor color)
    {
        _strip.SetPixelColor(index, color.Dim(getDesiredBrightness()));
    }

    void setStatusPixel(RgbColor color)
    {
        _strip.SetPixelColor(Config::STATUS_PIXEL_INDEX, color.Dim(Features::MASTER_MAX_BRIGHTNESS));
    }

    void previewBrightness(int brightness)
    {
        uint8_t dimness = (float)brightness / 100.0 * (float)Features::MASTER_MAX_BRIGHTNESS;

        for (int i = 0; i < Airports::COUNT; i++)
        {
            if (i == Config::STATUS_PIXEL_INDEX)
            {
                continue;
            }

            _strip.SetPixelColor(i, Colors::GREEN.Dim(dimness));
        }

        _strip.Show();
    }

    RgbColor getCategoryColor(const category_t category)
    {
        switch (category)
        {
        case VFR:
            return Colors::VFR;
        case MVFR:
            return Colors::MVFR;
        case IFR:
            return Colors::IFR;
        case LIFR:
            return Colors::LIFR;
        case NA:
            return Colors::BLACK;
        }

        return Colors::BLACK;
    }
}