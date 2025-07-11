// Songs are written in this file by creating a song structure and initialization file for the song.
// You can copy and paste one of the previous structures and initializations and change the details.
// If adding songs, increase the song number. Also add the song initialization function to the 
// songListInit function at the bottom.
// To write the song, find the lowest note in the song and set that as 0 in the notes array.
// Total note range is 1 octave from Eb6 to Eb7
// For the beat length choose the shortest beat length in the song and set it as 1 then all
// the longer notes will be multiples of that.

#include "pitch.h"
#include <stdint.h>
#include "BSP.h"

#ifndef song_h
#define song_h

#define SONGARRAYSIZE 50
#define SONGNUMBER 5

struct song{
	char *name;
	char *lyrics;
	
	int32_t syllables[SONGARRAYSIZE];
	int32_t wordCount;
	
	int32_t baseVolume;
	int32_t baseTempo;
  int32_t noteCount;
  int32_t notes[SONGARRAYSIZE];
	int32_t beats[SONGARRAYSIZE];
	int32_t volumeChange[SONGARRAYSIZE];
};

struct songStruct{
	struct song songs[SONGNUMBER];
};

struct songStruct songList;

struct song Twinkle;

void twinkleInit(void);

struct song Happy;

void happyInit(void);

struct song Home;

void homeInit(void);

struct song Old;

void oldInit(void);

struct song Saints;

void saintsInit(void);

void songListInit(void);

#endif

void twinkleInit(void){
	int32_t TwinkleNotes[]=
	{0,0,7,7,9,9,7,5,5,4,4,2,2,0,7,7,5,5,4,4,2,7,7,5,5,4,4,2,0,0,7,7,9,9,7,5,5,4,4,2,2,0};
	int32_t TwinkleNoteBeats[]=
	{1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2};
	int32_t TwinkleVolumeChange[]=
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int32_t TwinkleSyllables[]={2,2,2,1,1,1,2,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,2,2,2,1,1,1,2,1,1,1};	
		
	Twinkle.name = "Twinkle Little Star";
		
	Twinkle.lyrics = "Twinkle Twinkle Little Star How I Wonder What You Are Up Above The World So High Like A Diamond In The Sky Twinkle Twinkle Little Star How I Wonder What You Are "; 	
	
	Twinkle.wordCount=32;
		
	Twinkle.baseVolume = 100;
	Twinkle.noteCount = 42;
	Twinkle.baseTempo=100;	
	
	for(int i=0; i<SONGARRAYSIZE; i++){
			Twinkle.notes[i] = -1;
			Twinkle.beats[i] = 1;
			Twinkle.volumeChange[i] = 0;
			Twinkle.syllables[i]=0;
	}
	
	for(int i=0; i<Twinkle.wordCount; i++){		
		Twinkle.syllables[i] = TwinkleSyllables[i];
	}
	
	for(int i=0; i<Twinkle.noteCount; i++){		
		Twinkle.notes[i] = TwinkleNotes[i];
		Twinkle.beats[i] = TwinkleNoteBeats[i];
		Twinkle.volumeChange[i] = TwinkleVolumeChange[i];
	}

songList.songs[0] = Twinkle;
	
}

void happyInit(void){
	int32_t HappyNotes[]=
	{0,0,2,0,5,4,0,0,2,0,7,5,0,0,12,9,5,4,2,10,10,9,5,7,5};
	int32_t HappyNoteBeats[]=
	{1,1,2,2,2,4,1,1,2,2,2,4,1,1,2,2,2,2,2,1,1,2,2,2,4};
	int32_t HappyVolumeChange[]=
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	int32_t HappySyllables[]={2,2,1,1,2,2,1,1,2,2,1,2,2,2,1,1};	

	Happy.lyrics = "Happy Birthday To You Happy Birthday To You Happy Birthday Dear Dr._Le Happy Birthday To You "; 	
	
	Happy.wordCount=16;
		
	Happy.name = "Happy Birthday";
	Happy.baseVolume = 100;
	Happy.noteCount = 25;
	Happy.baseTempo=200;	
									
	for(int i=0; i<SONGARRAYSIZE; i++){
			Happy.notes[i] = -1;
			Happy.beats[i] = 1;
			Happy.volumeChange[i] = 0;
			Happy.syllables[i]=0;
	}
		
		for(int i=0; i<Happy.wordCount; i++){		
		Happy.syllables[i] = HappySyllables[i];
	}		
		
	for(int i=0; i<Happy.noteCount; i++){		
		Happy.notes[i] = HappyNotes[i];
		Happy.beats[i] = HappyNoteBeats[i];
		Happy.volumeChange[i] = HappyVolumeChange[i];
	}

songList.songs[1] = Happy;
	
}


void homeInit(void){
	int32_t HomeNotes[]=
	{0,0,5,7,9,5,4,2,10,10,10,10,10,12,5,5,5,4,5,7,0,0,5,7,9,5,4,10,10,10,10,10,9,7,5,4,5,7,5};
	int32_t HomeNoteBeats[]=
	{2,2,2,2,4,1,1,2,2,2,4,1,1,4,1,1,2,2,2,10,2,2,2,4,1,1,2,2,2,4,1,1,3,1,2,2,2,2,4};
	int32_t HomeVolumeChange[]=
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		
	int32_t HomeSyllables[]={3,2,2,3,1,2,1,2,3,1,1,2,2,1,1,2,1,2,2,1,2,2};	

	Home.lyrics = "Oh_Give_Me A_Home Where_The Buffalo Roam And_The Deer And_The Antelope Play Where Seldom Is_Heard A Dis couraging Word And_The Skys_Are Not Cloudy All_Day "; 	
	
	Home.wordCount=22;		
		
	Home.name = "Home on the Range";
	Home.baseVolume = 100;
	Home.noteCount = 39;
	Home.baseTempo=250;	
		
	for(int i=0; i<SONGARRAYSIZE; i++){
			Home.notes[i] = -1;
			Home.beats[i] = 1;
			Home.volumeChange[i] = 0;
			Home.syllables[i]=0;
	}
		
	for(int i=0; i<Home.wordCount; i++){		
		Home.syllables[i] = HomeSyllables[i];
		}		
	
	for(int i=0; i<Home.noteCount; i++){		
		Home.notes[i] = HomeNotes[i];
		Home.beats[i] = HomeNoteBeats[i];
		Home.volumeChange[i] = HomeVolumeChange[i];
	}

songList.songs[2] = Home;
	
}


void oldInit(void){
	int32_t OldNotes[]=
	{9,9,9,9,9,7,5,2,2,9,7,7,2,2,0,-2,7,7,9,5,4,4,2,2,2,9,7,5,2,2,9,7,7,2,2,0,-2,7,7,9,5,2,2,-2};
	int32_t OldNoteBeats[]=
	{3,1,1,1,2,2,2,1,1,2,2,1,1,1,1,2,4,1,1,2,2,1,1,1,1,2,2,2,1,1,2,2,1,1,1,1,2,4,1,1,2,2,2,2};
	int32_t OldVolumeChange[]=
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		
	int32_t OldSyllables[]={ 1  , 2    ,1  ,1  ,1   ,1  ,1  ,1  ,1   ,1   ,1  ,3    ,1   ,1  ,1 ,1   ,1  ,1    ,1  ,2    ,1  ,1  ,1   ,1  ,1  ,1  ,1   ,1   ,1   ,2   ,1   ,1  ,1 ,1   ,1  ,1};	

	Old.lyrics = 					  "I'm gonna take my horse to the old town road I'm gonna ride till I can't no more. I'm gonna take my horse to the old town road I'm gonna ride till I can't no more. "; 	
	
	Old.wordCount=35;		
		
	Old.name = "Old Town Road";
	Old.baseVolume = 100;
	Old.noteCount = 44;
	Old.baseTempo=250;	
		
	for(int i=0; i<SONGARRAYSIZE; i++){
			Old.notes[i] = -1;
			Old.beats[i] = 1;
			Old.volumeChange[i] = 0;
			Old.syllables[i]=0;
	}
		
	for(int i=0; i<Old.wordCount; i++){		
		Old.syllables[i] = OldSyllables[i];
		}		
	
	for(int i=0; i<Old.noteCount; i++){		
		Old.notes[i] = OldNotes[i];
		Old.beats[i] = OldNoteBeats[i];
		Old.volumeChange[i] = OldVolumeChange[i];
	}

songList.songs[3] = Old;
	
}

void saintsInit(void){
	int32_t SaintsNotes[]=
	{0,4,5,7,-2,0,4,5,7,-2,0,4,5,7,4,0,4,2,-2,4,2,0,0,4,7,7,7,5,-2,4,5,7,4,2,2,0};
	int32_t SaintsNoteBeats[]=
	{1,1,1,4,1,1,1,1,4,1,1,1,1,2,2,2,2,4,2,1,1,3,1,2,1,1,1,3,2,1,1,2,2,2,2,4};
	int32_t SaintsVolumeChange[]=
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		
	int32_t SaintsSyllables[]={2,2,1,2,1,2,2,1,2,1,3,2,2,2,1,2,1,2,1};	

	Saints.lyrics = "Oh_When The_Saints Go Marching In Oh_When The_Saints Go Marching In How_I_Long To_Be In_That Number When The_Saints Go Marching In "; 	
	
	Saints.wordCount=sizeof(SaintsSyllables)/sizeof(SaintsSyllables[0]);		
		
	Saints.name = "When the Saints";
	Saints.baseVolume = 100;
	Saints.noteCount = 36;
	Saints.baseTempo=250;	
		
	for(int i=0; i<SONGARRAYSIZE; i++){
			Saints.notes[i] = -1;
			Saints.beats[i] = 1;
			Saints.volumeChange[i] = 0;
			Saints.syllables[i]=0;
	}
		
	for(int i=0; i<Saints.wordCount; i++){		
		Saints.syllables[i] = SaintsSyllables[i];
		}		
	
	for(int i=0; i<Saints.noteCount; i++){		
		Saints.notes[i] = SaintsNotes[i];
		Saints.beats[i] = SaintsNoteBeats[i];
		Saints.volumeChange[i] = SaintsVolumeChange[i];
	}

songList.songs[4] = Saints;
	
}

void songListInit(void){
	twinkleInit();
	happyInit();
	homeInit();
	saintsInit();
	oldInit();
}
