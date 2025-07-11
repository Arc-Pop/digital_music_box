// settings.h

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

void SaveSettingsToFlash(int tempo, int volume, int loop, int visual);
void LoadSettingsFromFlash(int* tempo, int* volume, int* loop, int* visual);

#endif
