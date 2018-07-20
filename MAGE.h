#ifndef MAGE_H
# define MAGE_H

#define F_CPU 16000000

#define ever ;;

#define LOW     0
#define HIGH    1

#define TRUE    0xFF
#define FALSE   0x00

#define SND   0
#define SCL   1
#define SDA   2
#define DC    3

#define BTNS  2 //ADC

#define BTN_DELAY       180
#define BTN_THRESHOLD   2

/* Button ADC Values */
#define _UP     698
#define _DOWN   854
#define _LEFT   932
#define _RIGHT  613
#define _A      516
#define _B      325
#define _C      262

#define SPLASH_DELAY    1500

#define CMD             LOW
#define DATA            HIGH

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

#define SCREEN_ROWS     8
#define SCREEN_COLUMNS  16

// http://www.soundoctor.com/freq.htm
//      NOTE    PERIOD   FREQ.  Note        
#define _A4    2270/20 // 440hz, Concert A(4)  0
#define _Bb4   2100/20 // 466hz, Bb(4)         1
#define _B4    2000/20 // 494hz, B(4)          2
#define _C5    1900/20 // 523hz, C(5)          3
#define _Cs5   1800/20 // 554hz, C#(5)         4
#define _D5    1700/20 // 587hz, D(5)          5
#define _Ds5   1600/20 // 622hz, D#(5)         6
#define _E5    1500/20 // 659hz, E(5)          7
#define _F5    1400/20 // 698hz, F(5)          8
#define _Fs5   1350/20 // 740hz, F#(5)         9
#define _G5    1280/20 // 784hz, G(5)          A
#define _Gs5   1200/20 // 831hz, G#(5)         B
#define _A5    1140/20 // 880 A(5)             C
#define _Bb5   1070/20 // 932, Bb(5)           D
#define _B5    1010/20 // 988, B(5)            E

#define _A8    142/20  // 7040                 F
#define _A9    71/20   // 14080hz             X

/* New Fast PWM frequency values
                        f   OCR0A   f_
_A4	    2270	1135	440	70	    440.1408451	Concert
_Bb4	2100	1050	466	66	    466.4179104	Bb(4)
_B4	    2000	1000	494	62	    496.031746	B(4)
_C5	    1900	950	    523	59	    520.8333333	C(5)
_Cs5	1800	900	    554	56	    548.245614	C#(5)
_D5	    1700	850	    587	52	    589.6226415	D(5)
_Ds5	1600	800	    622	49	    625	        D#(5)
_E5	    1500	750	    659	47	    651.0416667	E(5)
_F5	    1400	700	    698	44	    694.4444444	F(5)
_Fs5	1350	675	    740	41	    744.047619	F#(5)
_G5	    1280	640	    784	39	    781.25	    G(5)
_Gs5	1200	600	    831	37	    822.3684211	G#(5)
_A5	    1140	570	    880	34	    892.8571429	A(5)
_Bb5	1070	535	    932	32	    946.969697	Bb(5)
_B5	    1010	505	    988	31	    976.5625	B(5)

*/

typedef unsigned char byte;
typedef unsigned int word;

typedef struct note {
    word tone;
    word duration;
} note;

static const __flash note startup[] = { {.tone=_A4, .duration=120}, 
                           {.tone=_C5, .duration=90}, 
                           {.tone=_E5, .duration=60}, 
                           {.tone=0, .duration=0}
                         };

static const __flash byte LOGO[] = {
    0x00, 0x00, 0x04, 0x08, 0x18, 0x37, 0x3F, 0x0F,     0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // HAT (Top) 
    0x00, 0x00, 0x3F, 0x7E, 0x10, 0x38, 0x1C, 0x0E,     0x04, 0x02, 0x0E, 0x1C, 0x38, 0x10, 0x3F, 0x7E,     // M (Top)
    0x00, 0x00, 0x3F, 0x7E, 0x81, 0xC1, 0xC1, 0xC1,     0xC0, 0xC1, 0xC1, 0xC1, 0xC1, 0xC0, 0xBF, 0x7E,     // A
    0x00, 0x00, 0x3F, 0x7E, 0x80, 0xC0, 0xC0, 0xC0,     0xC0, 0xC1, 0xC1, 0xC1, 0xC1, 0xC0, 0x80, 0x00,     // G
    0x00, 0x00, 0x3F, 0x7E, 0x81, 0xC1, 0xC1, 0xC1,     0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x00,     // E    
        
    0x00, 0x38, 0x5C, 0x2C, 0xEC, 0xEC, 0xEC, 0xEC,     0xEC, 0xEC, 0x2C, 0x5C, 0x3C, 0x38, 0x00, 0x00,     // HAT (Bottom)  
    0x00, 0x00, 0x7C, 0xFE, 0x00, 0x00, 0x00, 0x00,     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0xFC,     // M
    0x00, 0x00, 0x7C, 0xFE, 0x80, 0x80, 0x80, 0x80,     0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x7E, 0xFC,     // A
    0x00, 0x00, 0x7C, 0xFE, 0x01, 0x03, 0x03, 0x03,     0x03, 0x03, 0x83, 0x83, 0x83, 0x81, 0x7E, 0xFC,     // G
    0x00, 0x00, 0x7C, 0xFE, 0x01, 0x83, 0x83, 0x83,     0x83, 0x03, 0x03, 0x03, 0x03, 0x02, 0x00, 0x00,     // E
};

#define rngM 7919
#define rngA 17
#define rngC 3
word rng( void );

void delay_ms( word ms );
//void delay_us( word us );


// http://www.oz4.us/2015/12/recoding-bare-millis-clock-on-avr.html
// https://github.com/sourceperl/millis/blob/master/millis.c
// https://www.avrfreaks.net/forum/tut-c-newbies-guide-avr-timers?page=all
void initialise( void );
word millis( void );

//https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/cores/tiny/wiring_analog.c
//http://www.technoblogy.com/show?TWD
word read_buttons();

/* OLED Function */

void shift_out_byte(byte val);
void shift_out_block(const __memx byte *block, byte inverted);

void send_command(byte command);
void initialise_oled(void);
void clear_display(void);
void display_off(void);
void display_on(void);

void display_image(const __memx byte *img, byte col, byte row, byte width, byte height);

void set_display_col_row(byte col, byte row);

void set_tune(const __memx note *tne);
void beep(byte note, word duration);
//void crap_beep(word note, word dur);
void click( void );

#endif