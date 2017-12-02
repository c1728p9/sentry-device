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
#include "mbed.h"
#include <string>

#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "AudioPlayer.h"
#include "FileIterator.h"
#include "Sentry.h"


SDBlockDevice sd(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("sd", &sd);

AnalogOut aout(DAC0_OUT);
AudioPlayer player(&aout);
DigitalIn btn_on_off(SW2);
Sentry sentry(D2, D3);

static bool sentry_activated = false;
FileIterator audio_on(&fs, "sentry/on");
FileIterator audio_off(&fs, "sentry/off");
FileIterator audio_found(&fs, "sentry/found");
FileIterator audio_lost(&fs, "sentry/lost");


int main() {

    sd.frequency(25000000);

    while(1) {
        bool power_press = false;
        bool target_detected = false;
        bool target_lost = false;
        while (!power_press && !target_detected && !target_lost) {
            sentry.process();
            power_press = !btn_on_off;
            target_detected = sentry.get_target_detected();
            target_lost = sentry.get_target_lost();
        }

        if (power_press) {
            sentry_activated = !sentry_activated;
            printf("Sentry %s\n", sentry_activated ? "on" : "off");
            const char *path = sentry_activated ? audio_on.next() : audio_off.next();
            if (path) {
                File file(&fs, path);
                player.play(&file);
            } else {
                printf("Error getting audio file\n");
            }
            if (!sentry_activated) {
                File file(&fs, "sentry/other/powerdown.wav");
                player.play(&file);
            }
            sentry.set_enabled(sentry_activated);
        }

        if (target_detected) {
            printf("Detection event\n");
            const char *path = audio_found.next();
            if (path) {
                File file(&fs, path);
                player.play(&file);
            }
        }

        if (target_lost) {
            printf("Lost event\n");
            const char *path = audio_lost.next();
            if (path) {
                File file(&fs, path);
                player.play(&file);
            }
        }

        // Wait after audio has played so the rangefinder
        // can accurately take a reading
        wait(0.2);
    }
    return 0;
}

