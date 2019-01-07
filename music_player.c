#include <STC15.h>
#include "tone.h"
#include "defs.h"
#include "delay.h"
#include "music_player.h"

#define isspace(c) ((c) == ' ' || (c) == '\n' || (c) == '\t')
#define isdigit(c) ('0' <= (c) && (c) <= '9')

code float equal_temp[] = {
	1.0f,
	1.0594630943592953f,
	1.122462048309373f,
	1.1892071150027212f,
	1.2599210498948734f,
	1.3348398541700346f,
	1.4142135623730954f,
	1.498307076876682f,
	1.5874010519682f,
	1.6817928305074297f,
	1.7817974362806794f,
	1.887748625363388f,
};

code uint16_t tonic[] = {
	440, 494, 294, 262, 330, 349, 392
};

code uint8_t key2temp[] = {0, 2, 4, 5, 7, 9, 11};

void MusicPlayerInit(MusicPlayer * player, uint8_t * tape){
	player->tape   		= tape;
	player->cursor 		= 0;
	player->loop_sp 	= 0;
	player->bpm			= 104;
	player->tonic_freq	= 440;
	player->quaver_size = 0;
	player->octave		= 0;
	player->sharp		= 0;
	player->dotted		= 0;
	player->state		= Begin;
	player->errno		= 0;
	player->nobreak		= 0;
}

/*
	example of the tape:

	!104!
	C
	_*3 *2_ [ _*3 #*2_ _*3 7_ _*2 *1_ | 6 _0 1_ _3 6_ | 7 _0 3_ _#5 7_ ]

	etc..

	STATES:

	Begin: 
		isspace 		-> Eatup -> Begin
		//'_'			-> Eatup -> ReadQuaver
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

void MusicPlayerStart(MusicPlayer * player){
	uint8_t 	chr;
	uint16_t	uit;	// for Unsigned Int Int Temporary variable
//	int16_t		sit;	// for Signed Int Temporary variable
	int8_t		sct; 	// for Signed Char Temporary variable
	int8_t		sct2;
	float		ft;		// for Float Temporary
	uint8_t		i;
STATE_DISPATCH:
	switch( player->state ){
#define DEF_STATE(X)	case X: goto STATE_ ## X;
		STATES
#undef DEF_STATE
	}

STATE_Begin:
	chr = player->tape[player->cursor];
	if (chr == '\0'){
		goto STATE_Exit;
	}
	else if (isspace(chr) || chr == '|'){
		player->cursor ++;
		goto STATE_Begin;
	}
/*
	else if (chr == '_'){
		player->cursor ++;
		goto STATE_ReadQuaver;
	}
*/
	else if (isdigit(chr) || chr == '*' || chr == '#'
			 || chr == '.' || chr == 'b' || chr == '+'){
		player->state = PlayQuaver;
		player->sharp = 0;
		player->octave = 0;
		player->dotted = 0;
		goto STATE_ReadCrotchet;
	}
	else if ('A' <= chr && chr <= 'G'){
		goto STATE_ChangeTonic;
	}
	else if (chr == '['){
		goto STATE_MarkLoop;
	}
	else if (chr == ']'){
		goto STATE_Loop;
	}
	else if (chr == '!'){
		player->bpm = 0;
		player->cursor ++;
		goto STATE_ReadBPM;
	}
	else if (chr == '`'){
		player->bpm *= 2;
		player->cursor ++;
		goto STATE_Begin;
	}
	else if (chr == '\''){
		player->bpm /= 2;
		player->cursor ++;
		goto STATE_Begin;		
	}
	else if (chr == '~'){
		player->nobreak = ! player->nobreak;
		player->cursor ++;
		goto STATE_Begin;		
	}
	else {
		goto STATE_Error;
	}
/*
STATE_ReadQuaver:
	chr = player->tape[player->cursor];	
	if (isspace(chr)){	 
		player->cursor ++;
		goto STATE_ReadQuaver;
	}
	else if (isdigit(chr) || chr == '*' || chr == '#'
			 || chr == '.' || chr == 'b' || chr == '+'){
		player->state = ReadQuaver;
		player->sharp = 0;
		player->octave = 0;
		player->dotted = 0;
		goto STATE_ReadCrotchet;
	}
	else if (chr == '_'){
		player->cursor ++;
		goto STATE_PlayQuaver;
	}
	else {
		goto STATE_Error;
	}
*/
STATE_PlayQuaver: 
	if (!player->quaver_size){
		goto STATE_Begin;
	}
	uit = 60000 / player->bpm / player->quaver_size;
	uit = uit + uit * player->dotted / 2;
	//for (i = 0; i < player->quaver_size; i ++){
		//if (player->quaver[i])
			//tone(player->quaver[i]);
		if (player->quaver[0])
			tone(player->quaver[0]);
		else
			no_tone();

		if(player->nobreak){
			delayMs(uit);
		}
		else{
			delayMs(4 * uit / 5);
			no_tone();
			delayMs(uit / 5);
		}
	//}
	player->quaver_size = 0;
	goto STATE_Begin;

STATE_ReadCrotchet:
	chr = player->tape[player->cursor ++];
	if(chr == '#'){
		player->sharp ++;
		goto STATE_ReadCrotchet;
	}	
	else if(chr == 'b'){
		player->sharp --;
		goto STATE_ReadCrotchet;
	}
	else if(chr == '*'){
		player->octave ++;
		goto STATE_ReadCrotchet;
	}
	else if(chr == '.'){
		player->octave --;
		goto STATE_ReadCrotchet;
	}
	else if(chr == '+'){
		player->dotted ++;
		goto STATE_ReadCrotchet;
	}
	else if (chr == '0'){
		player->quaver_size ++;
		player->quaver[player->quaver_size - 1] = 0;
		goto STATE_DISPATCH;
	}

	else if ('1' <= chr && chr <= '7'){
		sct = key2temp[chr - '1'];
/*
		switch(chr){
			case '1' : sct = 0;	break;
			case '2' : sct = 2; break;
			case '3' : sct = 4; break;
			case '4' : sct = 5; break;
			case '5' : sct = 7; break;
			case '6' : sct = 9; break;
			case '7' : sct = 11;break;
		}
*/
		sct += player->octave * 12;
		sct += player->sharp;


		sct2 = 0;
		while(sct < 0){
			sct2 --;
			sct += 12;
		}
		while(sct >= 12){
			sct2 ++;
			sct -= 12;
		}

//	    sct2 = sct > 0 ? sct/12 + 1 : sct/12 - 1;
//		sct  = sct - sct2 * 12;
		

		ft = equal_temp[sct];
		if (sct2 < 0){
			for (i = 0; i < -sct2; i ++){
				ft /= 2.0f;
			}
		}
		else {	
			for (i = 0; i < sct2; i ++){
				ft *= 2.0f;
			}
		}
		player->quaver_size ++;
		chr = player->quaver_size - 1;
		player->quaver[chr] = player->tonic_freq * ft;

		goto STATE_DISPATCH;
	}

STATE_ChangeTonic:
	chr = player->tape[player->cursor ++];
	player->tonic_freq = tonic[chr - 'A'];
/*
	switch (chr){
		case 'A':	player->tonic_freq = 440; break;
		case 'B':	player->tonic_freq = 494; break;	
		case 'C':	player->tonic_freq = 262; break;
		case 'D':	player->tonic_freq = 294; break;
		case 'E':	player->tonic_freq = 330; break;
		case 'F':	player->tonic_freq = 349; break;
		case 'G':	player->tonic_freq = 392; break;

		default:	player->tonic_freq = 262;
	}
*/
	goto STATE_Begin;

STATE_MarkLoop:
	player->loop[player->loop_sp ++]
		 = player->cursor ++;

	goto STATE_Begin;

STATE_Loop:
	player->cursor ++;

	if (player->loop_sp){
		player->cursor
			= player->loop[-- player->loop_sp] + 1;
	}

	goto STATE_Begin;

STATE_ReadBPM:
	chr = player->tape[player->cursor++];
	if ('0' <= chr && chr <= '9'){
		player->bpm = chr - '0';
		goto STATE_CollectNumber;
	}
	else{
		goto STATE_Error;
	}

STATE_CollectNumber:
	chr = player->tape[player->cursor ++];
	if ('0' <= chr && chr <= '9'){
		player->bpm *= 10;
		player->bpm += chr - '0';
		goto STATE_CollectNumber;
	}
	else if (chr == '!'){
		goto STATE_Begin;
	}
	else {
		goto STATE_Error;
	}

STATE_Exit:
	no_tone();
	return;

STATE_Error:
	//player->errno = 1;
	tone(1200);
	delayMs(1000);
	goto STATE_Exit;

}
