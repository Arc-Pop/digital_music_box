//#include <string.h>
#include <stdio.h>

#ifndef pitch_h
#define pitch_h


/* List of pitch frequencies*/
#define C1  33
#define Db1 35
#define D1  37
#define Eb1 39
#define E1  41
#define F1  44
#define Gb1 46
#define G1  49
#define Ab1 52
#define A1  55
#define Bb1 58
#define B1  62

#define C2  65
#define Db2 69
#define D2  73
#define Eb2 78
#define E2  82
#define F2  87
#define Gb2 92
#define G2  98
#define Ab2 104
#define A2  110
#define Bb2 117
#define B2  123

#define C3  131
#define Db3 139
#define D3  147
#define Eb3 156
#define E3  165
#define F3  175
#define Gb3 185
#define G3  196
#define Ab3 208
#define A3  220
#define Bb3 233
#define B3  247

#define C4  262
#define Db4 277
#define D4  294
#define Eb4 311
#define E4  330
#define F4  349
#define Gb4 370
#define G4  393
#define Ab4 415
#define A4  440
#define Bb4 466
#define B4  494

#define C5  523  
#define Db5 554
#define D5  587
#define Eb5 622
#define E5  659
#define F5  698
#define Gb5 740
#define G5  784
#define Ab5 831
#define A5  880
#define Bb5 932
#define B5  988

#define C6  1047
#define Db6 1109
#define D6  1175
#define Eb6 1245
#define E6  1319
#define F6  1397
#define Gb6 1480
#define G6  1568
#define Ab6 1661
#define A6  1760
#define Bb6 1865
#define B6  1980

#define C7  2093
#define Db7 2218
#define D7  2349
#define Eb7 2489
#define E7  2637
#define F7  2794
#define Gb7 2960
#define G7  3136
#define Ab7 3322
#define A7  3520
#define Bb7 3729
#define B7  3951

#define C8  4186
#define Db8 4435
#define D8  4699
#define Eb8 4978
#define E8  5274
#define F8  5588
#define Gb8 5920
#define G8  6272
#define Ab8 6645
#define A8  7040
#define Bb8 7459
#define B8  7902


static int frequencies[] = {  
//                        C1, Db1, D1, Eb1, E1, F1, Gb1, G1, Ab1, A1, Bb1, B1, 
//                       C2, Db2, D2, Eb2, E2, F2, Gb2, G2, Ab2, A2, Bb2, B2,
//                        C3, Db3, D3, Eb3, E3, F3, Gb3, G3, Ab3, A3, Bb3, B3,               
//                        C4, Db4, D4, Eb4, E4, F4, Gb4, G4, Ab4, A4, Bb4, B4,
//                        C5, Db5, D5, Eb5, E5, F5, Gb5, G5, Ab5, A5, Bb5, B5,              
//                        C6, Db6, D6, 
												Eb6, E6, F6, Gb6, G6, Ab6, A6, Bb6, B6, 
												C7, Db7, D7, Eb7
//	, E7, F7, Gb7, G7, Ab7, A7, Bb7, B7, C8, Db8, D8
//	, Eb8, E8, F8, Gb8, G8, Ab8, A8, Bb8, B8,
	
};

static char pitchNames[][4] ={  
                            {'C','1', '\0', '\0'},
                            {'C','#', '1', '\0'},
                            {'D','1', '\0', '\0'},
                            {'D','#', '1', '\0'},
                            {'E','1', '\0', '\0'},
                            {'F','1', '\0', '\0'},
                            {'F','#', '1', '\0'},
                            {'G','1', '\0', '\0'},
                            {'G','#', '1', '\0'},
                            {'A','1', '\0', '\0'},
                            {'A','#', '1', '\0'},
                            {'B','1', '\0', '\0'},
                            {'C','2', '\0', '\0'},
                            {'C','#', '2', '\0'},
                            {'D','2', '\0', '\0'},
                            {'D','#', '2', '\0'},
                            {'E','2', '\0', '\0'},
                            {'F','2', '\0', '\0'},
                            {'F','#', '2', '\0'},
                            {'G','2', '\0', '\0'},
                            {'G','#', '2', '\0'},
                            {'A','2', '\0', '\0'},
                            {'A','#', '2', '\0'},                            
                            {'B','2', '\0', '\0'},
                            {'C','3', '\0', '\0'},
                            {'C','#', '3', '\0'},
                            {'D','3', '\0', '\0'},
                            {'D','#', '3', '\0'},
                            {'E','3', '\0', '\0'},
                            {'F','3', '\0', '\0'},
                            {'F','#', '3', '\0'},
                            {'G','3', '\0', '\0'},
                            {'G','#', '3', '\0'},
                            {'A','3', '\0', '\0'},
                            {'A','#', '3', '\0'},
                            {'B','3', '\0', '\0'},
                            {'C','4', '\0', '\0'},
                            {'C','#', '4', '\0'},
                            {'D','4', '\0', '\0'},
                            {'D','#', '4', '\0'},
                            {'E','4', '\0', '\0'},
                            {'F','4', '\0', '\0'},
                            {'F','#', '4', '\0'},
                            {'G','4', '\0', '\0'},
                            {'G','#', '4', '\0'},
                            {'A','4', '\0', '\0'},
                            {'A','#', '4', '\0'},
                            {'B','4', '\0', '\0'},
                            {'C','5', '\0', '\0'},
                            {'C','#', '5', '\0'},
                            {'D','5', '\0', '\0'},
                            {'D','#', '5', '\0'},
                            {'E','5', '\0', '\0'},
                            {'F','5', '\0', '\0'},
                            {'F','#', '5', '\0'},
                            {'G','5', '\0', '\0'},
                            {'G','#', '5', '\0'},
                            {'A','5', '\0', '\0'},
                            {'A','#', '5', '\0'},
                            {'B','5', '\0', '\0'},
                            {'C','6', '\0', '\0'},
                            {'C','#', '6', '\0'},
                            {'D','6', '\0', '\0'},
                            {'D','#', '6', '\0'},
                            {'E','6', '\0', '\0'},
                            {'F','6', '\0', '\0'},
                            {'F','#', '6', '\0'},
                            {'G','6', '\0', '\0'},
                            {'G','#', '6', '\0'},
                            {'A','6', '\0', '\0'},
                            {'A','#', '6', '\0'},
                            {'B','6', '\0', '\0'}
                }
;

int getPitchName(float frequency);

int flatOrSharp(float frequency, int index);

#endif
								