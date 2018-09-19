///*
//* lab5b.c
//*
//* Created: 4/28/2014 4:30:04 PM
//*  Author: Usman Majid
//*  Partner: Thomas Le
//*/

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 8000000UL
#include "avr.h"
#include "notes.h"
#define DEFAULT_VOLUME 100


void InitMusic()
{
	// Configure OC1B pin as output

	DDRD |= _BV(DDD4); //OC1B as output

	// timer1 configuration (for PWM)
    TCCR1A |= _BV(COM1B1);  // Clear OC1A/OC1B on compare match

	
	TCCR1B |= _BV(WGM13) 	//mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
		   |_BV(CS11); 		//prescaler(8)
}


/**
	Plays music.
*/

void PlayMusic (const int* pMusicNotes, uint8_t tempo)
{
	int duration;
	int note;
	int i;
	uint16_t delay = tempo * 100;

	while (*pMusicNotes)
	{
		note = *pMusicNotes;
		pMusicNotes++;

		duration = *pMusicNotes;
		pMusicNotes++;

		if( p == note )
		{
			//pause, do not generate any sound
			OCR1B = 0;
		}
		else
		{
			//not a pause, generate tone
			OCR1B = DEFAULT_VOLUME;

			//set frequency
			ICR1H = (note >> 8);
			ICR1L = note;
		}

		//wait duration
		for(i=0;i<32-duration;i++)
		{
			_delay_loop_2( delay );
		}

	}
	

	//turn off any sound
	OCR1B = 0;
}

const int octave[] = {c4, 8, d4, 8, e4, 8, f4, 8, g4, 8, a4, 8, h4, 8, c5, 8, MUSIC_END};


// Star Wars
const int starwars[] = 
{
	Ais2,8, Ais2,8, P,16, F3,8, F3,8, P,16, Dis3,16, P,16, D3,16, P,16, C3,16, P,16, Ais3,8,
	Ais3,8, P,16, F3,8, P,16, Dis3,16, P,16, D3,16, P,16, C3,16, P,16, Ais3,8, Ais3,8, P,16,
	F3,8, P,16, Dis3,16, P,16, D3,16, P,16, Dis3,16, P,16, C3,8, C3,8, 
	MUSIC_END
};
//"Zelda1:d=4,o=5,b=125:a#,f.,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f.6,16f#6,16g#.6,2a#.6,16a#.6,16g#6,16f#.6,8g#.6,16f#.6,2f6,f6,8d#6,16d#6,16f6,2f#6,8f6,8d#6,8c#6,16c#6,16d#6,2f6,8d#6,8c#6,8c6,16c6,16d6,2e6,g6,8f6,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,a#,f.,8a#,16a#,16c6,16d6,16d#6,2f6,8p,8f6,16f.6,16f#6,16g#.6,2a#.6,c#7,c7,2a6,f6,2f#.6,a#6,a6,2f6,f6,2f#.6,a#6,a6,2f6,d6,2d#.6,f#6,f6,2c#6,a#,c6,16d6,2e6,g6,8f6,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f";
const int zelda[] = 
{
	Ais5, 4, f5, 4, ais5, 8, ais5, 16, c6, 16, d6, 16, dis6, 16, f6, 2, p, 8, f6, 8, f6,18, fis6, 16, gis6, 16, ais6, 16, gis6, 16, fis6, 16, gis6, 8, fis6, 16, f6, 2, f6,4, dis6,8,  MUSIC_END	
};
// Fur Elise
const int furelise[] = 
{
    e4, 8, d4x, 8, e4, 8, d4x, 8, e4, 8, b3, 8, d4, 8, c4, 8, a3,8, p, 8,
    c3, 8, e3, 8, a3, 8,  b3, 4, p, 8, e3, 8, g3x, 8, b3, 8, c4, 4, p, 8, e3, 8,
    e3, 8, d4x, 8, e4, 8, d4x, 8, e4, 8, b3, 8, d4, 8, c4, 8, a3, 8, p, 8, c3, 8,
    e3, 8, a3, 8, b3, 4, p, 8, e3, 8, c4, 8, b3, 8, a3, 4,
    MUSIC_END
};

// Beatles, Hey Jude
const int Jude[] = {
	G2,8, E2,8, P,16, E2,16, E2,16, G2,16, A2,16, D2,8, P,16, D2,16, E2,16, F2,8, 
	C3,8, C3,16, C3,16, H2,16, G2,16, A2,16, G2,16, F2,16, E2,8, P,16, G2,16, 
	A2,16, A2,8, A2,16, D3,16, C3,16, H2,16, H2,16, C3,16, A2,16, G2,8, P,16, 
	C2,16, D2,16, E2,16, A2,16, A2,16, G2,8,
	MUSIC_END
};


int main()
{
	InitMusic();
	CLR_BIT(DDRB,PINB1); // set pinB1 as input
	while(1)
	{
		if(GET_BIT(PINB,1))
		{
		
		PlayMusic( octave, 40 );
		_delay_ms(1000);
		//if(readKey() != '-')
		//PlayMusic( furelise, 20 );
		//_delay_ms(1000);
		//PlayMusic( zelda, 21);
		//_delay_ms(1000);
		////PlayMusic( starwars, 17 );
		////_delay_ms(1000);
		//if(readKey() != '-')
		//PlayMusic( starwars, 20 );
			
		}
	}
	return 0;
}
