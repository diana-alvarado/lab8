/*
 * File:   Lab8.c
 * Author: Diana Alvarado
 *
 * Created on 27 de septiembre de 2021, 07:49 PM
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF       // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000
//Constantes
#define _t0_valor 250

/*
 Variables
 */
unsigned char bandera; 
unsigned char centena;
unsigned char decena;
unsigned char unidad;
int cont;
int volt;
int volt2;
int potenciometro2;

/*
 *Prototipos de funciones
 */
void setup(void);
void conversion(void);

/*
 tabla
 */
char tabla[10] = {
    0B00111111, //0
    0B00000110, //1
    0B01011011, //2
    0B01001111, //3
    0B01100110, //4
    0B01101101, //5
    0B01111101, //6
    0B00000111, //7
    0B01111111, //8
    0B01101111, //9
    };

/*
 * Interrupción
 */
void __interrupt() isr (void)
{
    if(PIR1bits.ADIF){
        if(ADCON0bits.CHS == 6)
            PORTA = ADRESH;
        else
            PORTD =  ADRESH;
        PIR1bits.ADIF =0;
        
        
    } 
    
    if( INTCONbits.T0IF )
    {
        INTCONbits.T0IF = 0; 
        TMR0 = _t0_valor;
        
        switch(bandera){
            case 0:
                PORTB = 0;
                PORTBbits.RB0 = 1;
                
                PORTC = (tabla[centena]);
                PORTCbits.RC7 = 1;
                bandera++;
                break;            
            case 1:
                PORTB = 0;
                PORTBbits.RB1 = 1;
                PORTC = (tabla[decena]);
                bandera++;
                break;
            case 2:
                PORTB = 0;
                PORTBbits.RB2 = 1;
                PORTC = (tabla[unidad]);
                bandera = 0;
                break; 
        }
        INTCONbits.T0IF = 0;
    } 
}

/*
 Main
 */
void main(void)
{
    setup();
    ADCON0bits.GO = 1;
    while(1)
    {
        if (ADCON0bits.GO ==0){
            if(ADCON0bits.CHS == 6)
                ADCON0bits.CHS =5;
            else 
                ADCON0bits.CHS =6;
            __delay_us(50);
            ADCON0bits.GO = 1;
        
        }
        conversion();
    }
   
}   
void conversion(void)
{
    volt2 = PORTD;
    potenciometro2 = (volt2 << 2)+4;
    cont = potenciometro2 >> 1;
    
    centena = (cont/100);
    decena = ((cont-(centena*100))/10);
    unidad = (cont-(centena*100 + decena*10));
    return;
}
void setup(void)
{
    //Configuración entradas y salidas
    ANSEL = 0b01100000;
    ANSELH = 0;
    
    TRISC = 0;
    TRISD = 0;
    TRISB = 0;
    TRISE = 0b0011;
    TRISA = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    //Configuracion del oscilador 
    OSCCONbits.IRCF = 0b0110; // 4MHz
    OSCCONbits.SCS = 1;
    
    //configuracion de ADc
    ADCON1bits.ADFM = 0;  //justificado a la i
    ADCON1bits.VCFG0 = 0; 
    ADCON1bits.VCFG0 = 0; 
    
    ADCON0bits.ADCS = 0b01;
    ADCON0bits.CHS = 5;
   
    ADCON0bits.ADON = 1;
    __delay_us(50);
    
    
    //configuracion de las interrupciones
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    
 
      //Configuración de tmr0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE =0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 1;
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = _t0_valor;
    
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.GIE = 1;

    return; 
    
}