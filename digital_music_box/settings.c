// settings.c

#include <stdint.h>
#include "../FlashProgram/FlashProgram.h"
#include "settings.h"
//#include "songs.h"


#define SETTINGS_ADDR  0x0003F000  // Must be 1KB-aligned, near end of flash
void SaveSettingsToFlash(int tempo, int volume, int loop, int visual) {
  Flash_Erase(SETTINGS_ADDR);                     		// Erase 1KB block
  Flash_Write(SETTINGS_ADDR, (uint32_t)tempo);    		// Write tempo
  Flash_Write(SETTINGS_ADDR + 4, (uint32_t)volume);		// Write volume
	Flash_Write(SETTINGS_ADDR + 8, (uint32_t)loop); 		// Write loop
	Flash_Write(SETTINGS_ADDR + 12, (uint32_t)visual);	// Write visual
	
}

void LoadSettingsFromFlash(int* tempo, int* volume, int* loop, int* visual) {
  uint32_t rawTempo = *((uint32_t*)SETTINGS_ADDR);
  uint32_t rawVolume = *((uint32_t*)(SETTINGS_ADDR + 4));
  uint32_t rawLoop = *((uint32_t*)(SETTINGS_ADDR + 8));
	uint32_t rawVisual = *((uint32_t*)(SETTINGS_ADDR + 12));
	
  // If flash is erased (fresh chip), defaults to 0xFFFFFFFF
  *tempo = (rawTempo == 0xFFFFFFFF) ? 0 : (int32_t)rawTempo;
  *volume = (rawVolume == 0xFFFFFFFF) ? 0 : (int32_t)rawVolume;
	*loop = (rawLoop == 0xFFFFFFFF) ? 0 : (int32_t)rawLoop;
	*visual = (rawVisual == 0xFFFFFFFF) ? 0 : (int32_t)rawVisual;
}
