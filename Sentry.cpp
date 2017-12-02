/* mbed Microcontroller Library
 * Copyright (c) 2017-2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Sentry.h"

static const float boot_time = 5.0;
static const float found_to_ready = 5.0;
static const float detect_range = 1000.0;

#define sentry_printf   printf

const char *state_str(sentry_state_t state)
{
    switch (state) {
    case SENTRY_STATE_OFF:
        return "off";
    case SENTRY_STATE_BOOTING:
        return "booting";
    case SENTRY_STATE_READY:
        return "on";
    case SENTRY_STATE_FOUND:
        return "found";
    default:
        return "invalid";
    }
}

Sentry::Sentry(PinName trigger, PinName echo):
        _srf(trigger, echo), _state(SENTRY_STATE_OFF),
        _detected(false), _lost(false)
{

}

void Sentry::set_enabled(bool enable)
{
    if (enable) {
        if (SENTRY_STATE_OFF == _state) {
            sentry_printf("State %s -> %s\n", state_str(_state), state_str(SENTRY_STATE_BOOTING));
            _state = SENTRY_STATE_BOOTING;
            _timer.reset();
            _timer.start();
        }
    } else {
        _timer.stop();
        sentry_printf("State %s -> %s\n", state_str(_state), state_str(SENTRY_STATE_OFF));
        _state = SENTRY_STATE_OFF;
    }
}

void Sentry::process()
{
    switch (_state) {

    case SENTRY_STATE_BOOTING:
        if (_get_detected()) {
            _timer.reset();
        }
        if (_timer.read() > boot_time) {
            sentry_printf("State %s -> %s\n", state_str(_state), state_str(SENTRY_STATE_READY));
            _timer.reset();
            _state = SENTRY_STATE_READY;
        }
        break;

    case SENTRY_STATE_READY:
        if (_get_detected()) {
            sentry_printf("State %s -> %s\n", state_str(_state), state_str(SENTRY_STATE_FOUND));
            _timer.reset();
            _state = SENTRY_STATE_FOUND;
            _detected = true;
        }
        break;
    case SENTRY_STATE_FOUND:
        if (_get_detected()) {
            _timer.reset();
        }
        if (_timer.read() > found_to_ready) {
            sentry_printf("State %s -> %s\n", state_str(_state), state_str(SENTRY_STATE_READY));
            _timer.reset();
            _state = SENTRY_STATE_READY;
            _lost = true;
        }
        break;

    default:
        break;
    }

}

bool Sentry::get_target_detected(bool clear)
{
    bool detected = _detected;
    if (clear) {
        _detected = false;
    }
    return detected;
}

bool Sentry::get_target_lost(bool clear)
{
    bool lost = _lost;
    if (clear) {
        _lost = false;
    }
    return lost;
}

bool Sentry::_get_detected()
{
    float range = _srf.read();
    //printf("Distance = %.1f\n", range);
    return range < detect_range;
}


