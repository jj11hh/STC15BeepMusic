#ifndef __MUSIC_PLAYER_H__
#define __MUSIC_PLAYER_H__

#include "defs.h"
#include "tone.h"

#define LOOP_STACK_DEPTH 	3
#define MAX_QUAVER 			4

/*
	example of the tape:

	!104!
	C
	_*3 *2_ [:2 _*3 #*2_ _*3 7_ _*2 *1_ | 6 _0 1_ _3 6_ | 7 _0 3_ _#5 7_ ]

	etc..

	STATES:

	Begin: 
		isspace 		-> Eatup -> Begin
		//'_'				-> Eatup -> ReadQuaver
		isdigit, [*.#b] -> ReadCrotchet	-> PlayCrotchet
		isalpha			-> ChangeTonic
		'['				-> MarkLoop
		']'				-> Loop
		'!'				-> Eatup -> ReadBPM
		'`'				-> Eatup -> DoubleBPM
		'\''			-> Eatup -> HalfBPM
		EOF				-> Exit
		Other			-> Error

	Eatup:
		Any				-> [next_state]

	//ReadQuaver:
	//	isspace			-> Eatup -> ReadQuaver
	//	isdigit, [#*]	-> ReadCrotchet -> ReadQuaver
	//	'_'				-> PlayQuaver ->Begin
	//	Other			-> Error

	PlayQuaver:
		Any				-> Begin

	ReadCrotchet:
		[*.#b]			-> ReadCrotchet
		isdigit			-> [next_state]
		Other			-> Error

	ChangeTonic:
		A-Z				-> Begin
		Other			-> Error

	MarkLoop:
		[				-> Begin

	Loop:
		Any				-> Begin

	ReadBPM:
		isdigit			-> CollectNumber

	CollectNumber:
		isdigit			-> CollectNumber
		'!'				-> Begin

	ChangeBPM:
		Any				-> Begin

	Exit:
		Any				-> Exit

	Error:
		Any				-> Exit

*/


#define STATES \
	DEF_STATE(Begin)\
	DEF_STATE(ReadCrotchet)\
	DEF_STATE(ChangeTonic)\
	DEF_STATE(MarkLoop)\
	DEF_STATE(Loop)\
	DEF_STATE(ReadBPM)\
	DEF_STATE(Exit)\
	DEF_STATE(PlayQuaver)\
	DEF_STATE(CollectNumber)\
	DEF_STATE(Error)

//	DEF_STATE(ReadQuaver)

#define DEF_STATE(X) X,
typedef enum {
	STATES
} State;
#undef DEF_STATE

typedef struct music_player{
	uint8_t 	* tape;
	uint16_t 	cursor;
	uint16_t 	loop[LOOP_STACK_DEPTH];
	uint8_t		loop_sp;
	uint16_t	bpm;
	uint16_t 	tonic_freq;
	uint8_t		quaver_size;
	uint16_t 	quaver[MAX_QUAVER];
	int8_t		octave;
	int8_t		sharp;
	int8_t		dotted;
	State		state;
	int8_t 		nobreak;
	int8_t		errno;
} MusicPlayer;


void MusicPlayerInit(MusicPlayer * player, uint8_t * tape);
void MusicPlayerStart(MusicPlayer * player);
#endif