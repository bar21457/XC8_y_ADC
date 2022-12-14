/*
 * File:   main.c
 * Author: byron
 *
 * Created on October 14, 2022, 6:37 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT        // Oscillator Selection bits 
                                            // (INTOSC oscillator: I/O function 
                                            // on RA6/OSC2/CLKOUT pin, I/O 
                                            // function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and 
                                // can be enabled by SWDTEN bit of the WDTCON 
                                // register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR 
                                // pin function is digital input, MCLR 
                                // internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code 
                                // protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code 
                                // protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
                                // (Internal/External Switchover mode disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit 
                                // (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin 
                                // has digital I/O, HV on MCLR must be used for 
                                // programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out 
                                // Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits 
                                // (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 4000000

void setup(void);
void setupADC(void);
void multiplexado(void);

//******************************************************************************
// C??digo Principal
//******************************************************************************
void main(void) {
    
    setup();
    setupADC();
    
    while(1){
        
        //PreLab
        
        if(!PORTBbits.RB0)
        {
            while(!RB0){multiplexado();}
            PORTD ++;           //Incrementamos en 1 el PORTC
        }
        if(!PORTBbits.RB1)
        {
            while(!RB1){multiplexado();}
            PORTD --;           //Incrementamos en 1 el PORTC
        }
        
        //Lab
        
        ADCON0bits.GO = 1;      //Iniciamos la conversi??n en el ADC
        while (ADCON0bits.GO == 1){};
        ADIF = 0;               //Bajamos la bandera del ADC
        multiplexado();
        //PORTC = ADRESH;         //El PORTC adquiere el valor de la conversi??n
        __delay_ms(10);
        
        //PostLab
        
        if(!PORTBbits.RB2)
        {
            while(!RB2){multiplexado();}
            
            if(ADRESH > PORTD){
                PORTEbits.RE2 = 1;      //Encendemos la alarma
            }
            else{
                PORTEbits.RE2 = 0;      //Apagamos la alarma
            }
        }
    }
    return;
}

//******************************************************************************
// Funci??n para configurar GPIOs
//******************************************************************************
void setup (void){
    
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0b00000111;     //Configuraci??n del PORTB como input
    TRISC = 0;              //Configuraci??n del PORTC como output
    TRISD = 0;              //Configuraci??n del PORTD como output
    TRISE = 0;              //Configuraci??n del PORTE como output
    
    OPTION_REGbits.nRBPU = 0;   //Habilitamos los pull-ups del PORTB
    WPUBbits.WPUB0 = 1;         //Habilitamos el pull-up del RB0
    WPUBbits.WPUB1 = 1;         //Habilitamos el pull-up del RB1
    WPUBbits.WPUB2 = 1;         //Habilitamos el pull-up del RB2
    
    PORTB = 0;              //Limpiamos el PORTB
    PORTC = 0;              //Limpiamos el PORTC
    PORTD = 0;              //Limpiamos el PORTD
    PORTE = 0;              //Limpiamos el PORTD
    
    //Configuraci??n del Oscilador Interno a 4MHz
    
    OSCCONbits.IRCF = 0b0110 ;
    OSCCONbits.SCS = 1;
 
//    IOCB = 1;
//    
//    INTCONbits.GIE = 1;     //Habilitamos las interrupciones globales (GIE)
//    INTCONbits.PEIE = 1;    //Habilitamos las interrupci??n del PEIE
//    INTCONbits.RBIF = 1;    //Habilitamos las interrupciones del PORTB (RBIF)
//    INTCONbits.RBIE = 0;    //Bajamos la bandera de interrupci??n del PORTB (RBIE)
    
}

void setupADC (void){
    
    //Paso 1: Selecci??n del puerto de entrada
    
    TRISAbits.TRISA0 = 1;       //Configuraci??n del RBA0 como input
    ANSELbits.ANS0 = 1;         //Configuraci??n del pin RBA0 como an??logo (AN0)
    
    //Paso 2: Configuraci??n del m??dulo ADC
    
    ADCON0bits.ADCS0 = 1;
    ADCON0bits.ADCS1 = 0;       //Fosc/8
    
    ADCON1bits.VCFG0 = 0;       //Ref VDD
    ADCON1bits.VCFG1 = 0;       //Ref VSS
    
    ADCON1bits.ADFM = 0;        //Justificado hacia la izquierda
    
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS3 = 0;        //Selecci??n del canal an??logo AN0
    
    ADCON0bits.ADON = 1;        //Habilitamos el ADC
    
    __delay_us(100);            //Delay para adquirir la lectura
}

void multiplexado (void){
    
    int unidades;
    int decenas;
    int numeros[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};
    
    unidades = ADRESH%10;
    decenas = ADRESH/10;
    
    PORTC = numeros[unidades];  //PORTC = unidades del ADRESH
    PORTEbits.RE0 = 1;          //Encendemos el display de unidades
    __delay_ms(1);
    PORTEbits.RE0 = 0;          //Apagamos el display de unidades
    
    PORTC = numeros[decenas];   //PORTC = decenas del ADRESH
    PORTEbits.RE1 = 1;          //Encendemos el display de decenas
    __delay_ms(1);
    PORTEbits.RE1 = 0;          //Apagamos el display de decenas
    
}