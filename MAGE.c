#include "MAGE.h"

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

volatile word _millis = 0;

word rngSEED = 5;
word rng( void )
{
    rngSEED = (rngSEED*rngA +rngC) % rngM;
    return rngSEED;
}

void delay_ms( word ms )
{
    word ts = millis()+ms;
    
    while(millis() < ts) { }
}

/*void delay_us( word us )
{
   for (word i=0; i < us; i++)
   {
      _delay_us(1);
   }
}*/

word read_buttons()
{
    
    ADCSRA = 0xC7;
    
    while(ADCSRA & (1<<ADSC));
    
    byte low = ADCL;
    byte high = ADCH;
    
    return (high << 8) | low;
}

void initialise( void )
{
    DDRB = (1<<SDA) | (1<<DC) | (1<<SCL) | (1<<SND); // Configure Outputs
    
    ADMUX = 0x00 | BTNS;    // Configure analog input
    
    /* Initialise Timers */
    TCCR1 = 0x87;           // CTC Mode, CK/64 prescale (250)
    //TIMSK |= 0x40;          // Enable OCIE1A Compare Interrupt
                 //                     F_CPU   Prescale  Timer frequency (1 ms)
    OCR1A = 250; // Set compare value (16000000Hz / 64) / 1000Hz
    
    //Timer 0 - set to no prescaler
    TCCR0A = 0x02; // normal counter, CTC Mode
    TCCR0B = 0x01; // No prescale
    //Enable compare interrupt
    
    //compare value = 16000000 / 1000000 = 16
    // 16 = 1us, notes can update every 10 (not perfect but ok), compare value can be 160
    OCR0A = 160;
    
    TIMSK = 0x50;           // Enable OCIE1A and OCIE0A
    sei();                  // Enable interrupts
    
    initialise_oled();
    clear_display();
}

ISR(TIMER1_COMPA_vect)
{
    _millis += 1;
}

word duration_timer = 0;
byte period_timer = 0;
byte period_value = 0;

ISR(TIMER0_COMPA_vect)
{
    if (_millis < duration_timer)
    {
        if (period_timer == 0)
        {
            PORTB ^= 1 << SND;    // Toggle
            period_timer = period_value;
        }
    }
    period_timer -= 1;
}

void beep(byte tone, word duration)
{
    period_value = tone;
    duration_timer = _millis+duration;
}

void click( void )
{
    beep(20, 15);
    //crap_beep(_A9, 15);
}

word millis( void )
{
    return _millis;
}

/* OLED Functions */

void send_command(byte command)
{
    for (byte i = 0; i < 8; i++)  
    {
        if ( command & (1 << (7 - i)) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
    }
}

void shift_out_byte(byte b)
{
    if ( b & (1 << 0) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 1) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 2) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 3) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 4) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 5) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 6) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
    
    if ( b & (1 << 7) )
    {
        PORTB |= 1 << SDA;
    }
    else
    {
        PORTB &= ~(1 << SDA);
    }
    
    PORTB |= 1 << SCL;      // HIGH
    PORTB &= ~(1 << SCL);   // LOW
}

/* Un-rolling the loop makes this much faster */
void shift_out_block(const __memx byte *block, byte inverted)
{
    byte b;
    for (byte i = 0; i < 8; i++)  
    {
        b = block[i];
        if (inverted)
            b = ~b;
        
        if ( b & (1 << 0) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 1) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 2) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 3) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 4) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 5) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 6) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
        
        if ( b & (1 << 7) )
        {
            PORTB |= 1 << SDA;
        }
        else
        {
            PORTB &= ~(1 << SDA);
        }
        
        PORTB |= 1 << SCL;      // HIGH
        PORTB &= ~(1 << SCL);   // LOW
    }
}

void initialise_oled(void)
{
    PORTB &= ~(1 << DC);        // LOW (Command Mode)
    send_command(0xAE);         // DISPLAYOFF
    
    send_command(0xD5);         // SETDISPLAYCLOCKDIV
    send_command(0x80);         // the suggested ratio 0x80

    send_command(0xA8 );        // SSD1306_SETMULTIPLEX
    send_command(SCREEN_HEIGHT - 1);

    send_command(0xD3 );        // SETDISPLAYOFFSET
    send_command(0x0);          // no offset
    send_command(0x40  | 0x0);  // SETSTARTLINE line #0
  
    send_command(0x8D);        // CHARGEPUMP
    send_command(0x14);         // Not External Vcc
  
    send_command(0x20 );        // MEMORYMODE
    send_command(0x00);         // 0x0 act like ks0108
    send_command(0xA0  | 0x1);  // SEGREMAP
    send_command(0xC8 );        // COMSCANDEC


    send_command(0xDA);        // SETCOMPINS
    send_command(0x12);
  
    send_command(0x81 );        // SETCONTRAST
    send_command(0xCF);         // Not External Vcc


    send_command(0xD9 );        // SETPRECHARGE
    send_command(0xF1);         // Not External Vcc
  
    send_command(0xDB);        // SETVCOMDETECT
    send_command(0x40);
  
    send_command(0xA4 );        // DISPLAYALLON_RESUME
    send_command(0xA6 );        // NORMALDISPLAY

    //send_command(0x2E );        // DEACTIVATE_SCROLL

    send_command(0xAF);         // DISPLAYON
    
    send_command(0xB0 + 0);           // PAGEADDR (0 = reset)
    send_command(0 & 0x0F);           // Column start address (0 = reset)
    send_command(0x10 | (0 >> 4));    // LOW COL ADDR
    
    PORTB |= 1 << DC;           // DATA
}

void clear_display(void)
{
    for (byte row=0 ; row<SCREEN_ROWS ; row++)
    {
        for (byte col=0 ; col<SCREEN_WIDTH ; col++)
        {
            shift_out_byte(0x00);
        }
    }
}

void display_off(void)
{
    PORTB &= ~(1 << DC);    // COMMAND
    send_command(0xAE);     // DISPLAYOFF
    PORTB |= 1 << DC;       // DATA
}

void display_on(void)
{
    PORTB &= ~(1 << DC);    // COMMAND
    send_command(0xAF);     // DISPLAYON
    PORTB |= 1 << DC;       // DATA
}

void display_image(const __memx byte *img, byte col, byte row, byte width, byte height)
{
    for (byte h=0 ; h<height ; h++)
    {
        set_display_col_row(col, row+h);
        
        for (byte w=0 ; w<width ; w++)
            shift_out_block(&img[(width * h + w)*8], FALSE);
    }
}

void set_display_col_row(byte col, byte row)
{
    PORTB &= ~(1 << DC);                    // COMMAND
    
    send_command(0xB0 + row);               // PAGEADDR
    send_command(col & 0x0F);               // Column start address (0 = reset)
    send_command(0x10 | (col >> 4));        // LOW COL ADDR
    
    PORTB |= 1 << DC;                       // DATA
}


/* Sound Functions */

/*void crap_beep(word note, word dur)
{
    word ts = millis();
    while (millis() < ts + dur)
    {
        PORTB |= 1 << SND;    // HIGH
        delay_us(note);
        PORTB &= ~(1 << SND); // LOW
        delay_us(note);
    }
}*/