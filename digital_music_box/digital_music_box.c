// ECE 5436 Advanced Microprocessors
// Final Project
// Team 2: Digital Music Box
// Jonathan Arc, Abdur Aziz, Len Shaji
/****************************************************************************
This program builds on the labs written by Jonathan Valvano to develop a real
time operating system that controls a Tiva C microcontroller connected to a
Educational Boosterpack MK II. The program is a digital music box that
contains common melodies which can be selected from a menu display on the LCD
screen and while playing the melody the LCD screenwill display colorful 
visuals.
****************************************************************************/
/* 
Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
You may use, edit, run or distribute this fileas long as the above copyright 
notice remains.
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
*/
#include <stdio.h>
#include <stdint.h>
#include "../inc/BSP.h"
#include "../inc/Profile.h"
#include "../inc/CortexM.h"
#include "os.h"
#include "../inc/pitch.h"
#include "../inc/songs.h"
#include "settings.h"
#include <math.h>
#include <stdlib.h>  // for rand()
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

//------------- Global variables shared between tasks ----------------

#define THREADFREQ 1000  			// System variable, shouldn't need to change.
int menuMove = 1;							//  Determines whether the menu can go higher or lower.
int32_t Play=0;								//	Begins playing song when set to 1. Stops when set to 0.
int32_t Beat=0;								//	Semaphore for controlling periodic note changes, checks every 10ms.	
int32_t Adjust=0;							//	Semaphore for slowing the speed of the option adjustments to 1 per 100ms.
int32_t notePlace = 0;				// 	Location in song arrays for note current being played.
int volume = 0;								//	Volume modifier, combines with song default volume for volume option adjustment.
int tempo = 0;								//	Tempo modifier, combines with song default tempo for tempo option adjustment.
int32_t msPerMinute = 60000;	//	MS per minute, used for calculating beat length base don tempo.
int32_t beatTime=0;						//	Counts the number of ms since last beat change. Increments by 10 every ~10ms
int32_t adjustVolume=0;				//	When volume option is selected it is set to 1 to allow volume changes.
int32_t adjustTempo=0;				//	When tempo option is selected it is set to 1 to allow tempo changes.
int32_t adjustVisuals=0;	
int screenType=0;								//	0 is the song menu, 1 is the options menu.
int menuChange = 1;						//	Controls various LCD functions that occur during menu change.
int menu1Pos = 0;							//	Position in the song menu.
int menu2Pos = 0;							//	Position in the options menu.
int songChoice=0;							//	Indicates which song has been selected to play.
int loop=0;										//  Controls whether song is looped
int wordI=0;									// Which letter in word it is on.
int wordN=0;									// Which word it is on.
int wordChange=0;
int curSyl=0;
int letLev=100;
int startStream=0;
int continueStream=0;
int visualOption=0;
int visOptHL=0;
char curWord[10];	
int curWordI=0;
int note;
//for Rain visual:
#define NUM_RAIN 16
static int rainY[NUM_RAIN];
static const int rainX[NUM_RAIN] = {
   8,  16,  24,  32,
  40,  48,  56,  64,
  72,  80,  88,  96,
 104, 112, 120,  4   // I've added 16 streams spaced across 128px
};

//------------ end of Global variables shared between tasks -------------


/*** Pattern #1: Field of random colored dots ***/
static void DrawDots(void){
			if(startStream==1 || continueStream==1){ 
				BSP_LCD_FillScreen(LCD_BLACK);
				startStream=0;
				continueStream=0;
				}							
			int x = rand();
			int y = rand(); 	
			int c = rand();
			for(int i=0;i<1000;i++){
				x = (y+c) % 128;
				y = (x+c) % 128;
				c = (y+x)*i % 128;
        uint16_t c = BSP_LCD_Color565(((c%132)+100), ((x%132)+100), ((y%132)+100));
        BSP_LCD_DrawPixel(x,y,c);
				}
}

// Draw Rain function
static void DrawRain(void){
	if(startStream==1 || continueStream==1){ 
				BSP_LCD_FillScreen(LCD_BLACK);
				startStream=0;
				continueStream=0;
			}	
//	uint16_t c = BSP_LCD_Color565(0,200,255);
  for(int i = 0; i < NUM_RAIN; i++){
    // advance drop, wrapping at bottom
    rainY[i] = (rainY[i] + 3) % 128;
    // We can draw a 2x2 rain drop for visibility
    BSP_LCD_FillRect(rainX[i], rainY[i], 6, 6, LCD_DARKBLUE);
		if(i>2){BSP_LCD_FillRect(rainX[i], rainY[i-2], 6, 6, LCD_BLACK);}
		else{BSP_LCD_FillRect(rainX[i], rainY[i], 6, 6, LCD_BLACK);}
  }
}

// Fade function
static uint16_t Wheel(int p){
  if(p < 85)      return BSP_LCD_Color565(255 - p*3, p*3,0);
  else if(p < 170){
    p -= 85;
    return BSP_LCD_Color565(0,255 - p*3, p*3);
  } else {
    p -= 170;
    return BSP_LCD_Color565(p*3, 0, 255 - p*3);
  }
}

static void DrawFade(void){
  static int phase = 0;
  BSP_LCD_FillScreen(Wheel(phase));
  phase = (phase + 1) & 0xFF;  // wrap 0–255
}



//------------Task1 handles switch input-------
// *********Task1*********
// Main thread scheduled by OS round robin preemptive scheduler
// checks the switches, updates the menu, and selects song to play 
// Inputs:  none
// Outputs: none
void Task1_Init(void){ 
	BSP_Button1_Init(); // Select button
  BSP_Button2_Init();	// Back/Options button
	BSP_RGB_D_Init(0, 0, 0);
	LoadSettingsFromFlash(&tempo, &volume, &loop, &visualOption);
}

void Task1(void){
	static uint8_t prev1 = 0, prev2 = 0;  // Tracks switch changes.
  uint8_t current;												
	
  while(1){
    current = BSP_Button1_Input();			// Check if select (top) button has been pushed
    if((current == 0) && (prev1 != 0)){	// If it has been pushed
			if(screenType==0){									// And currently on song menu				
				if(songChoice!=menu1Pos){				// New song has been selected
				notePlace=0;										// Start selected song from beginning
				wordN=0;												// Word number for lyrics
				wordI=0;												// Index for letter of lyrics character array.
				startStream=1;									// Triggers the LCD to be cleared in some cases
				songChoice=menu1Pos;						// Select current song
				Play=1;													// Play song
				}
				else if(songChoice==menu1Pos && Play==1){ // Selected song is already playing
					Play=0;																	// Pause the currently selected song
					if(visualOption!=0){menuChange=1;}			// Triggers the LCD to be cleared
					BSP_Buzzer_Set(0);											// Turns off buzzer sound
				}
				else{
					if(visualOption!=0){continueStream=1;}
					Play=1;
				}										// Unpause the current song and continue playing it				
			}			
			else if(screenType==1){ 						// Select button push while on options menu
				if(menu2Pos==1){adjustVolume=1;}// Allow volume adjustment and prevent changing selected option.
				else if(menu2Pos==2){						// Allow tempo adjustment and prevent changing selected option.
				adjustTempo=1;					
				}					
				else if(menu2Pos==3){						// Set loop option
				if(loop==0){loop=1;}
				else{loop=0;}
				SaveSettingsToFlash(tempo, volume, loop, visualOption);
				}
				else if(menu2Pos==4){						// Set visualization option
				adjustVisuals=1;
				visOptHL=LCD_BLUE;
				}
				else if(menu2Pos==5){						// Reset options to default	
				volume=0;
				tempo=0;
				loop=0;
				visualOption=0;	
				SaveSettingsToFlash(tempo, volume, loop, visualOption);
				}					
			}
    }
    prev1 = current;
    current = BSP_Button2_Input();			// Check back/option (bottom) button for input
    if((current == 0) && (prev2 != 0)){	// Button2 was pressed since last loop
			if(screenType==0){screenType=1; menuChange=1;} // Change from songs to options menu
			else if(screenType==1 && adjustVolume==1){ adjustVolume=0; } // Go back from volume adjustment.
			else if(screenType==1 && adjustTempo==1){ adjustTempo=0; }	 // Go back from tempo adjustment.
			else if(screenType==1 && adjustVisuals==1){ adjustVisuals=0; visOptHL=LCD_BLACK;}	 // Go back from visuals mode adjustment.
			else if(screenType==1 && adjustVolume==0 && adjustTempo==0 && adjustVisuals==0){ //	Go back to Song menu
					menuChange=1;
					screenType=0;
					continueStream=1;
				}		
		}
    prev2 = current;
    BSP_Delay1ms(5); // very inefficient, but does debounce the switches
  }
}	
/******************************************* */
/*          End of Task1 Section              */
/* ****************************************** */


//------------Task2 handles joystick input-------
// *********Task2*********
// Main thread scheduled by OS round robin preemptive scheduler
// non-real-time task
// checks the joystick and updates variables. 
// Inputs:  joystick movement
// Outputs: x and y position which varie between 0 and 1023.
uint16_t x, y;										
uint8_t select;								
void Task2_Init(void){ BSP_Joystick_Init();	}
void Task2(void){ 
	while(1){
	BSP_Joystick_Input(&x, &y, &select);
	}
}
/* ****************************************** */
/*          End of Task 2 Section              */
/* ****************************************** */



//------------Task3 Draws Screen Menu-------
// *********Task3*********
// Main thread scheduled by OS round robin preemptive scheduler
// non-real-time task
// draws the main menu screen. 
// Inputs:  
// Outputs: 
// Notes: joystick position value ranges from 0 to 1023. Normal position value 533.
void Task3_Init(void){
BSP_LCD_Init();
BSP_LCD_FillScreen(LCD_BLACK);
}
void Task3(void){ 
char *song1 = songList.songs[0].name; //"Twinkle Little Star";
char *song2 = songList.songs[1].name; //"Happy Birthday";
char *song3 = songList.songs[2].name; //"Home on the Range";
char *song4 = songList.songs[3].name; //"Old Town Road";
char *song5 = songList.songs[4].name; //"When the Saints Go Marching In";
		
char *option1 = "Volume";							// Menu text
char *option2 = "Tempo";
char *option3 = "Loop";
char *option4 = "Visuals";			
char *option5 = "Reset";		
//char *menu = "Song List";
//char *submenu = "Options";
//int titleSpace = 12;
int opt1=0; // Variables for highlighting
int opt2=0;
int opt3=0;
int opt4=0;
int opt5=0;	

	while(1){	
//  screen is actually 129 by 129 pixels, x 0 to 128, y goes from 0 to 128
	//if(screenState == 0){
	if(screenType==0 && (Play==0 || visualOption==0)){ // Draw Songs menu
		
		// Clear screen once
	if(menuChange == 1){
		BSP_LCD_FillScreen(LCD_BLACK);
		menuChange = 0;
	}
	
		
		// Draw the "Song Menu" title, for larger characters the had to be drawn individually to not overlap.
		BSP_LCD_DrawChar(12, 2, 'S', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(24, 2, 'o', LCD_WHITE, LCD_BLUE, 2);	
		BSP_LCD_DrawChar(36, 2, 'n', LCD_WHITE, LCD_BLUE, 2);		
		BSP_LCD_DrawChar(48, 2, 'g', LCD_WHITE, LCD_BLUE, 2);		
		BSP_LCD_DrawChar(60, 2, ' ', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(72, 2, 'M', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(84, 2, 'e', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(96, 2, 'n', LCD_WHITE, LCD_BLUE, 2);		
		BSP_LCD_DrawChar(108, 2, 'u', LCD_WHITE, LCD_BLUE, 2);		
		
	// Checks joystick to change selected menu option	
	if(y>563 && menu1Pos>0 && menuMove==1){menu1Pos--; menuMove=0;} // Go up 
	else if(y<503 && menu1Pos<4 && menuMove==1){menu1Pos++; menuMove=0;} // Go down
	else if(y>523 && y< 543){ menuMove=1;}	// Stay in place
	
		opt1=0;
		opt2=0;
		opt3=0;
		opt4=0;
		opt5=0;	
	
	switch(menu1Pos){ // Controls Song Menu changing selected option.
		
				case 0:
					opt1=LCD_BLUE;
					break;
				case 1:
					opt2=LCD_BLUE;
					break;
				case 2:
					opt3=LCD_BLUE;
					break;
				case 3:
					opt4=LCD_BLUE;
					break;
				case 4:
					opt5=LCD_BLUE;
					break;
			}
	
		BSP_LCD_DrawString( 2, 3, song1, LCD_WHITE, opt1);	
		BSP_LCD_DrawString( 2, 5, song2, LCD_WHITE, opt2);
		BSP_LCD_DrawString( 2, 7, song3, LCD_WHITE, opt3);
		BSP_LCD_DrawString( 2, 9, song4, LCD_WHITE, opt4);		
		BSP_LCD_DrawString( 2, 11, song5, LCD_WHITE, opt5);
		}
	
	else if(screenType==1){ // Draw Options menu
		if(menuChange==1){ // Screen must be filled black when changing between menus.
		BSP_LCD_FillScreen(LCD_BLACK); // Fill screen with black.
		menuChange=0; // Reset need to fill screen with black.
		menu2Pos=1;	// Select first option.
		}
		// Draw the "Options Menu" title, for larger characters the had to be drawn individually to not overlap.
		BSP_LCD_DrawChar(12, 2, 'O', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(24, 2, 'p', LCD_WHITE, LCD_BLUE, 2);	
		BSP_LCD_DrawChar(36, 2, 't', LCD_WHITE, LCD_BLUE, 2);		
		BSP_LCD_DrawChar(48, 2, 'i', LCD_WHITE, LCD_BLUE, 2);		
		BSP_LCD_DrawChar(60, 2, 'o', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(72, 2, 'n', LCD_WHITE, LCD_BLUE, 2);
		BSP_LCD_DrawChar(84, 2, 's', LCD_WHITE, LCD_BLUE, 2);
		
		// Checks joystick to change selected menu option	
			if(adjustVolume==0 && adjustTempo==0 && adjustVisuals==0){ // Prevent movement if options selected
			if(y>563 && menu2Pos>1 && menuMove==1){menu2Pos--; menuMove=0;} // Go up
			else if(y<503 && menu2Pos<5 && menuMove==1){menu2Pos++; menuMove=0;} //Go down
			else if(y>523 && y< 543){ menuMove=1;} // Stay in place
		}
			
		opt1=0;
		opt2=0;
		opt3=0;
		opt4=0;
		opt5=0;	
			switch(menu2Pos){ // Controls Options Menu changing selected option.	
				case 1:
					opt1=LCD_BLUE;
					break;
				case 2:
					opt2=LCD_BLUE;
					break;
				case 3:
					opt3=LCD_BLUE;
					break;
				case 4:
					opt4=LCD_BLUE;
					break;
				case 5:
					opt5=LCD_BLUE;
					break;
			}
		
		int volumePerc = (songList.songs[menu1Pos].baseVolume+volume)/5;
		
		BSP_LCD_DrawString( 2, 3, option1, LCD_WHITE, opt1);	// "Volume"
		if(volumePerc>99){ BSP_LCD_SetCursor(15, 3);}  // Volume number
		if(volumePerc<100){ BSP_LCD_SetCursor(16, 3); }// must be shifted if<100
		if(volumePerc<10){ BSP_LCD_SetCursor(17, 3); }// must be shifted if<10
		BSP_LCD_OutUDec(volumePerc, LCD_WHITE);// Write number
		if(volumePerc<100){ // Black square covers over hundreds position if volume<100
			BSP_LCD_FillRect(87, 30, 8, 8, LCD_BLACK);
		}
		if(volumePerc<10){ // Black square covers over tens position if volume<10
			BSP_LCD_FillRect(93, 30, 8, 8, LCD_BLACK);
		}

		if(adjustVolume){
			BSP_LCD_DrawChar(77, 26, '-', LCD_BLUE, LCD_BLACK, 2);
			BSP_LCD_DrawChar(110, 26, '+', LCD_BLUE, LCD_BLACK, 2);			
		}
		else{
			BSP_LCD_DrawChar(77, 26, ' ', LCD_BLUE, LCD_BLACK, 2);
			BSP_LCD_DrawChar(110, 26, ' ', LCD_BLUE, LCD_BLACK, 2);			
		}

		
		BSP_LCD_DrawString( 2, 5, option2, LCD_WHITE, opt2); // "Tempo"
		if((songList.songs[menu1Pos].baseTempo+tempo)>99){ BSP_LCD_SetCursor(15, 5);}  // Tempo number 
		if((songList.songs[menu1Pos].baseTempo+tempo)<100){ BSP_LCD_SetCursor(16, 5); }// must be shifted if<100
		BSP_LCD_OutUDec((songList.songs[menu1Pos].baseTempo+tempo), LCD_WHITE);// Write number
		if((songList.songs[menu1Pos].baseTempo+tempo)<100){ // Black square covers over hundreds position if tempo<100
			BSP_LCD_FillRect(87, 50, 8, 8, LCD_BLACK);
		}
		if(adjustTempo){
			BSP_LCD_DrawChar(77, 47, '-', LCD_BLUE, LCD_BLACK, 2);
			BSP_LCD_DrawChar(110, 47, '+', LCD_BLUE, LCD_BLACK, 2);			
		}
		else{
			BSP_LCD_DrawChar(77, 47, ' ', LCD_BLUE, LCD_BLACK, 2);
			BSP_LCD_DrawChar(110, 47, ' ', LCD_BLUE, LCD_BLACK, 2);			
		}
		
		

		BSP_LCD_DrawString( 2, 7, option3, LCD_WHITE, opt3); // "Loop"
		if(loop==0){
			BSP_LCD_DrawString( 15, 7, "Off", LCD_WHITE, LCD_BLACK); // "Loop"
		}
		else{
			BSP_LCD_FillRect(87, 70, 8, 8, LCD_BLACK);
			BSP_LCD_DrawString( 16, 7, "On", LCD_WHITE, LCD_BLACK); // "Loop"		
		}

		BSP_LCD_DrawString( 2, 9, option4, LCD_WHITE, opt4); // "Visuals"
		if(visualOption==0){
			BSP_LCD_FillRect(60, 90, 30, 8, LCD_BLACK);
			BSP_LCD_DrawString( 15, 9, "Off", LCD_WHITE, visOptHL); // "Visuals"
		}
		else if(visualOption==1){
			BSP_LCD_FillRect(55, 90, 16, 8, LCD_BLACK);
			BSP_LCD_DrawString( 12, 9, "Lyrics", LCD_WHITE, visOptHL); // "Visuals"	
		}
		else if(visualOption==2){
			BSP_LCD_FillRect(55, 90, 16, 8, LCD_BLACK);
			BSP_LCD_DrawString( 12, 9, "Meters", LCD_WHITE, visOptHL); // "Visuals"		
		}
		else if(visualOption==3){			
			BSP_LCD_DrawString( 11, 9, "Sparkle", LCD_WHITE, visOptHL); // "Visuals"		
		}
		else if(visualOption==4){			
			BSP_LCD_FillRect(55, 90, 29, 8, LCD_BLACK);
			BSP_LCD_DrawString( 14, 9, "Rain", LCD_WHITE, visOptHL); // "Visuals"		
		}
		else if(visualOption==5){			
			BSP_LCD_FillRect(55, 90, 29, 8, LCD_BLACK);
			BSP_LCD_DrawString( 14, 9, "Fade", LCD_WHITE, visOptHL); // "Visuals"		
		}

		
		BSP_LCD_DrawString( 2, 11, option5, LCD_WHITE, opt5); // "Reset"

	if(Play==1 && wordChange==1){
		curWordI=0;
		while(songList.songs[songChoice].lyrics[wordI]!=' '){
	//		if(songList.songs[songChoice].lyrics[wordI]=='-'){letLev-=20;}
			curWord[curWordI]=songList.songs[songChoice].lyrics[wordI];
			wordI++;
			curWordI++;
		}	
		wordI++;
		wordChange=0;
	}
	}
	
	else if(screenType==0 && Play==1){		// Visuals
		
		switch(visualOption){
			case 0: //No visuals
			break;
			
			case 1: // Lyrics
		if(startStream==1){letLev=100;}
		if(startStream==1 || continueStream==1){ 
		BSP_LCD_FillScreen(LCD_BLACK);
		startStream=0;
		continueStream=0;
		}
		if(wordChange==1){
		BSP_LCD_FillRect(0, 60, 128, 17, LCD_BLACK);
		curWordI=0;
		letLev=60;	
		//if(letLev>110){letLev=10; BSP_LCD_FillScreen(Black);}
		while(songList.songs[songChoice].lyrics[wordI]!=' '){
	//		if(songList.songs[songChoice].lyrics[wordI]=='-'){letLev-=20;}
			curWord[curWordI]=songList.songs[songChoice].lyrics[wordI];
			wordI++;
			curWordI++;
		}
		int letterLoc=64-(12*curWordI/2);
		for(int i=0; i<curWordI; i++){
			if(curWord[i]=='_'){
			BSP_LCD_DrawChar(letterLoc, letLev, ' ', LCD_WHITE, LCD_BLACK, 2);
			letterLoc+=12;
			i++;	
			}
//			if(curWord[i]=='-'){ letLev+=20; i++;}
		BSP_LCD_DrawChar(letterLoc, letLev, curWord[i], LCD_WHITE, LCD_BLACK, 2);
		letterLoc+=12;
		}
		wordI++;
		wordChange=0;
	}
		break;
			
			case 2:				
			  // Now Playing Screen (top half)
			if(startStream==1 || continueStream==1){ 
				BSP_LCD_FillScreen(LCD_BLACK);
				startStream=0;
				continueStream=0;
			}				
			
  BSP_LCD_DrawString(2, 2, "Now Playing:", LCD_WHITE, LCD_BLACK);
  BSP_LCD_DrawString(2, 4, songList.songs[songChoice].name, LCD_ORANGE, LCD_BLACK);
// ----- Bottom Half: Visualizer ----- 
	note = songList.songs[songChoice].notes[notePlace];
if(note >= 0 && note <= 14){
  int baseHeight = (note + 1) * 4;
  if(baseHeight > 64) baseHeight = 64;

  // Clear bottom half (y = 64 to 127)
  BSP_LCD_FillRect(0, 64, 128, 64, LCD_BLACK);

  // Draw 8 bars spaced across bottom half
  for(int i = 0; i < 8; i++){
    int x = 10 + i * 15;
    int mod = (i * 3 + note) % 10;
    int height = baseHeight - mod;
    if(height < 4) height = 4;
    int y = 128 - height;

    // Dynamic color: based on bar index and pitch
		int r = 200 + (note * 3 + i * 7) % 56;  // stays between 200–255
		int g = 100 + (note * 5 + i * 9) % 156; // stays between 100–255
		int b = 30 + (note * 2 + i * 4) % 50;   // stays low for warmth (30–80)

    BSP_LCD_FillRect(x, y, 8, height, BSP_LCD_Color565(r, g, b));
  }
}
	break;

			case 3:
			DrawDots();
			break;
			
			case 4:
			DrawRain();				
			break;

			case 5:
			DrawFade();				
			break;

			
}
		
}
	
	}
}
/* ****************************************** */
/*          End of Task 3 Section              */
/* ****************************************** */


// *********Task 4*********
// Drescription: Plays the song using the structures in songs.h
// Inputs:  none
// Outputs: none
void Task4_Init(void){ BSP_Buzzer_Init(0); } // Turn off buzzer

void Task4(void){ 
	int32_t beatLength; // Initialize variable for holding length of beat in ms
	
	while(1){ // Does not exit.
	OS_Wait(&Beat); // Runs every 10ms
	if(Play==1){ // If song has been selected and is set to play
	beatLength = (msPerMinute/(songList.songs[songChoice].baseTempo+tempo));// Calculate length of beat in ms.	
		
		if(wordN==0){curSyl=songList.songs[songChoice].syllables[wordN]; wordN++; wordChange=1;}
		
		if(beatTime>beatLength){ // If it has been longer than the length of a beat
		// Below the if conditional is for when there are sequences of the same note to make them sound distinct
		if(songList.songs[songChoice].notes[notePlace+1] == songList.songs[songChoice].notes[notePlace]){
				BSP_Buzzer_Set(0);
				BSP_Delay1ms(1);		
				}
		// Play the next note in the song array.
		if(songList.songs[songChoice].notes[notePlace]==-2){BSP_Buzzer_Set(0);}
		else if(songList.songs[songChoice].notes[notePlace]!=-2){			
		BSP_Buzzer_Pitch(frequencies[songList.songs[songChoice].notes[notePlace]], 
					songList.songs[songChoice].baseVolume+volume + songList.songs[songChoice].volumeChange[notePlace]);
					
			if(curSyl==0){curSyl=songList.songs[songChoice].syllables[wordN]; wordN++; wordChange=1;}
			curSyl--;
			}



		// If the note is longer than 1 beat, subtract them from beatTime which is 1. 
		if(songList.songs[songChoice].beats[notePlace]>1){ // ex. note is 4 beats long, 4-1=3, holds for 3 more
			beatTime -= ( songList.songs[songChoice].beats[notePlace] * beatLength );
		}
		else{	beatTime=0;} // Reset the time since the last beat was played
		notePlace++; // Go to the next note in the array
		}	
	else{ beatTime+=20;} // Add 10 ms to the time since the last be was played, (10ms because periodic scheduler)
	if(songList.songs[songChoice].notes[notePlace]==-1 && beatTime>=beatLength){ // end of the song		
		notePlace=0; // Set the note index back to the beginning
		BSP_Buzzer_Set(0); // Turn off buzzer
//		if(visualOption==1){
//			while(songList.songs[songChoice].wordCount<(wordN-1)){}
//		}
		wordN=0;
		wordI=0;
		if(loop==0){
		letLev=100;	
		//menuChange=1;
		Play=0; // Turn off the playback
		if(visualOption){
		continueStream=1;	
		menuChange=1;
		BSP_LCD_FillScreen(LCD_BLACK);
		}	
			//BSP_LCD_FillRect(0, 64, 128, 64, Black); // clear bottom half
		}
	}
}
}
}


// *********Task 5*********
// Drescription: This is periodic to slow the speed the options change
// Inputs:  none
// Outputs: none
void Task5(void){
		int visualMove;
		int xHigh=520;
		int xLow=420;
	
		while(1){			
		if(Adjust){ // Periodic semaphore allowing it to run every 100ms
	
		if(adjustVolume){ // Volume option is selected
		if(x>xHigh && ((songList.songs[menu1Pos].baseVolume+volume)<500)){volume+=5;SaveSettingsToFlash(tempo, volume, loop, visualOption);} //Increase to max total of 512
		else if(x<xLow && ((songList.songs[menu1Pos].baseVolume+volume)>0)){volume-=5;SaveSettingsToFlash(tempo, volume, loop, visualOption);}// Decrease to min total of 10
		}

		else if(adjustTempo){ // Tempo option is selected
		if(x>xHigh && ((songList.songs[menu1Pos].baseTempo+tempo)<400)){tempo++;SaveSettingsToFlash(tempo, volume, loop, visualOption);} //Increase to a max total of 400bpm
		else if(x<xLow && ((songList.songs[menu1Pos].baseTempo+tempo)>40)){tempo--;SaveSettingsToFlash(tempo, volume, loop, visualOption);}//Decrease to min total 40bpm
		}
		
		else if(adjustVisuals){
		if(x>xHigh && visualOption<5 && visualMove==1){visualOption++; visualMove=0; SaveSettingsToFlash(tempo, volume, loop, visualOption);} 
		else if(x<xLow && visualOption>0 && visualMove==1){visualOption--; visualMove=0; SaveSettingsToFlash(tempo, volume, loop, visualOption);}
		else if(x<xHigh && x>xLow){ visualMove=1; }
		}
		
		OS_Wait(&Adjust);
	}
	}
}
/* ****************************************** */
/*          End of Task 5 Section              */
/* ****************************************** */

// *********Task #*********
// Drescription:
// Inputs:  none
// Outputs: none
void Task99(void){
}
/* ****************************************** */
/*          End of Task # Section              */
/* ****************************************** */

int main(void){
  OS_Init();
  Profile_Init();  // initialize the 7 hardware profiling pins
	songListInit(); // Populates the array of song structures.

	Task1_Init(); // Buttons
	Task2_Init(); // Joystick
	Task3_Init(); // LCD Screen
	Task4_Init(); // Buzzer
	
	OS_InitSemaphore(&Beat,0); // Controls music note changes
	OS_InitSemaphore(&Adjust, 0); // Controls option adjustment speed
	OS_AddThreads(&Task1, 3, &Task2, 3, &Task3, 3, &Task4, 2, &Task5, 3); // Add threads to scheduler
	OS_PeriodTrigger0_Init(&Beat,20);  // Beat time counter runs every 10 ms
	OS_PeriodTrigger1_Init(&Adjust,100);  // Adjustments allowed 1 unit every 100 ms

	OS_Launch(BSP_Clock_GetFreq()/THREADFREQ); // doesn't return, interrupts enabled in here
  return 0;             // this never executes
}

