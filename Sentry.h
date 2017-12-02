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
#ifndef SENTRY_H
#define SENTRY_H

#include "mbed.h"

#include "SRF05.h"

enum sentry_state_t {
    SENTRY_STATE_OFF,
    SENTRY_STATE_BOOTING,
    SENTRY_STATE_READY,
    SENTRY_STATE_FOUND,
};

class Sentry
{
public:
    /**
     * Construct a sentry
     *
     * @param trigger   Trigger pin of rangefinder
     * @param echo      Echo pin of rangefinder
     */
    Sentry(PinName trigger, PinName echo);

    /**
     * Enable or disable the sentry
     *
     * @param enable    mode
     */
    void set_enabled(bool enable);

    /**
     * Run the sentry's state machine
     *
     * This function must be called periodically
     */
    void process();

    /**
     * Check if a target has been detected
     *
     * @param clear     Clears the flag after read when set to true
     * @return          true if a target was detected, false otherwise
     */
    bool get_target_detected(bool clear=true);

    /**
     * Check if a target has been lost
     *
     * @param clear     Clears the flag after read when set to true
     * @return          true if a target was lost, false otherwise
     * @note            This event only occurs after a target detected event
     */
    bool get_target_lost(bool clear=true);

private:
    SRF05 _srf;
    Timer _timer;
    sentry_state_t _state;
    bool _detected;
    bool _lost;

    bool _get_detected();

};

#endif
