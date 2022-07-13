#include "common.h"
#include "isr.h"
#include "global.h"
#include "uart.h"
#include "print.h"
#include "Monitor.h"
#include "sfr_ext.h"
#include "hardware.h"
#include "i2c_device.h"
#include "i2c.h"
#include "rom.h"
#include "msp_displayport.h"
#include "smartaudio_protocol.h"
#include "dm6300.h"
#include "camera.h"
#include "lifetime.h"
#include "8051.h"
#include "C8051F330.h"

#include "common.h"
#include "global.h"
#include "uart.h"
#include "isr.h"
#include "uart.h"

extern uint8_t int0_req;
extern uint8_t int1_req;
extern uint8_t btn1_tflg ;
extern uint8_t pwr_sflg ;    //power autoswitch flag
extern uint8_t pwr_tflg ;
extern uint8_t cfg_tflg ;
extern uint8_t temp_tflg ;
extern uint8_t timer_4hz ;
extern uint8_t timer_8hz ;
extern uint8_t timer_16hz ;
extern uint16_t __idata timer_ms10x ;
extern uint16_t seconds ;
extern uint8_t RS0_ERR ;

void CPU_init(void)
{
    SCON0 = 0x50;   // [7:6] uart0 mode: 0x01 = 8bit, variable baudrate(related to timer1)
                    // [4]   uart0 rx enable

    SCON1 = 0x50;   // [7:6] uart1 mode: 0x01 = 8bit, variable baudrate(related to timer1)
                    // [4]   uart1 rx enable

    PCON  = 0xC0;   // [7]   double rate for uart0
                    // [6]   double rate for uart1

    TMOD  = 0x20;   // [7]   timer1 enabled by pin gate1
                    // [6]   timer1: 0=as timer, 1=as counter
                    // [5:4] tmier1 mode: 0x10 = 8bit timer with 8bit auto-reload
                    // [3]   timer0 enabled by pin gate0
                    // [2]   timer0: 0=as timer, 1=as counter
                    // [1:0] tmier0 mode: 0x01 = 16bit timer, {TH0,TL0} cascaded

    CKCON = 0x1F;   // [4]   timer1 uses a divide-by-N of the system clock frequency. 0:N=12; 1:N=4
                    // [3]   timer0 uses a divide-by-N of the system clock frequency. 0:N=12; 1:N=4
    
    TH0   = 139;
    TL0   = 0;
    
    TH1   = 0xEC;   // [7:0] in timer mode 0x10:   ----------------->> 148.5MHz: 0x87; 100MHz: 0xAF; 54MHz: 0xD4; 27MHz: 0xEA
                    //	               f(clk)
                    //  BaudRate = --------------  (M=16 or 32, decided by PCON double rate flag)
                    //             N*(256-TH1)*M   (N=4 or 12, decided by CKCON [4])
                    // 19200  - 0x87
                    // 115200 - 0xEC    256 - 148.5M/64/115200 = 0xEC  0.7%
                    // 230400 - 0xF6    256 - 148.5M/64/230400 = 0xF6  0.7%
                    // 250000 - 0xF7    256 - 148.5M/64/250000 = 0xF7  3.125%

    TCON  = 0x50;   // [6]   enable timer1
                    // [4]   enable timer0

    IE    = 0xD2;   // [7]   enable global interupts  1
                    // [6]   enable uart1  interupt   1
                    // [5]   enable timer2 interupt   0
                    // [4]   enable uart0  interupt   1 
                    // [3]   enable timer1 interupt   0 
                    // [2]   enable INT1   interupt   0 
                    // [1]   enable timer0 interupt   0
                    // [0]   enable INT0   interupt   0
    IP    = 0x10;   // UART0=higher priority, Timer 0 = low
    
}

void Timer0_isr(void) __interrupt(1)
{
    TH0 = 138;

    #ifdef USE_SMARTAUDIO
    if(SA_config) {
        if(suart_tx_en) {
            //TH0 = 139;
            suart_txint();
        }
        else
            suart_rxint();
    }
    #endif
    
    timer_ms10x++;
}

void Timer1_isr(void) __interrupt(3 )
{
}		 


void Ext0_isr(void) __interrupt(0)
{
	int0_req = 1;
}

void Ext1_isr(void) __interrupt(2)
{
	int1_req = 1;
}

void UART0_isr() __interrupt(4)
{
	if( RI ) {			//RX int
		RI = 0;
		RS_buf[RS_in++] = SBUF0;
		if( RS_in>=BUF_MAX ) RS_in = 0;
        if(RS_in == RS_out) RS0_ERR = 1;
	}

	if( TI ) {			//TX int
		TI = 0;
		RS_Xbusy=0;
	}
}

#define RI1 (SCON1 & 0x01)
#define TI1 (SCON1 & 0x02)

void UART1_isr() __interrupt(6)
{
    
	if( RI1 ) {			//RX int
		SCON1 &= 0xfe;
		RS_buf1[RS_in1++] = SBUF1;
		if( RS_in1>=BUF_MAX ) RS_in1 = 0;
	}

	if( TI1 ) {			//TX int
		SCON1 &= 0xfd;
		RS_Xbusy1=0;
	}
}



uint8_t UNUSED = 0;

void timer_task();
#ifdef USE_SMARTAUDIO
void SA_Delay_init();
#endif

void main(void)
{
    // init
    CPU_init();
    WriteReg(0, 0xB0, 0x3E);
    WriteReg(0, 0xB2, 0x03);
    WriteReg(0, 0x80, 0xC8);
    //WAIT(100);

    #ifdef _DEBUG_MODE
    Printf("\r\n========================================================");
    Printf("\r\n     >>>             Divimath DM568X            <<<     ");
    Printf("\r\n========================================================");
    Printf("\r\n");
    #endif

    
    Init_HW(); // init
    fc_init(); // init displayport
    
    #ifdef USE_SMARTAUDIO
    SA_Init();
    #endif
    
    #ifdef _DEBUG_MODE
    Prompt();
    #endif

    // main loop
    while(1) {

        timer_task();

        #ifdef USE_SMARTAUDIO
        while(SA_task());
        #endif

        #ifdef _RF_CALIB
        CalibProc();
        #elif defined _DEBUG_MODE
        Monitor();
        #endif

        Video_Detect();
        if(!SA_lock) OnButton1();

        if((last_SA_lock && (seconds > WAIT_SA_CONFIG)) || (last_SA_lock == 0)){
            TempDetect(); // temperature dectect
            PwrLMT(); // RF power ctrl
            msp_task(); // msp displayport process
            Update_EEP_LifeTime();
        }
        #ifdef USE_SMARTAUDIO
        SA_Delay_init();
        #endif

    }
}

void timer_task()
{
    static uint16_t cur_ms10x_1sd16 = 0, last_ms10x_1sd16 = 0;
    static uint8_t  timer_cnt = 0;
    cur_ms10x_1sd16 = timer_ms10x;
    if(((cur_ms10x_1sd16 - last_ms10x_1sd16) >= TIMER0_1SD16) || (cur_ms10x_1sd16 < last_ms10x_1sd16)) {
        last_ms10x_1sd16 = cur_ms10x_1sd16;
        timer_cnt++;
        timer_cnt &= 15;
        if(timer_cnt == 15) {   //every second, 1Hz
            btn1_tflg = 1;
            pwr_tflg = 1;
            cfg_tflg = 1;
            seconds++;
            pwr_sflg = 1;
        }

        if((timer_cnt&7) == 7)  //every half second, 2Hz
            temp_tflg = 1;
        
        if((timer_cnt&3)== 3)   //every quater second, 4Hz
            timer_4hz = 1;
        
        if((timer_cnt&1)== 1)   //every octual second, 8Hz
            timer_8hz = 1;
        timer_16hz = 1;
    }
}

#ifdef USE_SMARTAUDIO
void SA_Delay_init()
{
    static uint8_t SA_saved = 0;
    static uint8_t SA_init_done = 0;
    
    if(SA_saved == 0){
        if(seconds >= WAIT_SA_CONFIG){
            I2C_Write(ADDR_EEPROM, EEP_ADDR_SA_LOCK, SA_lock, 0, 0);
            #ifdef _DEBUG_MODE
            Printf("\r\nSave SA_lock(%bx) to EEPROM",SA_lock);
            #endif
            SA_saved = 1;
        }
    }
    
    //init_rf
    if(SA_init_done==0){
        if(last_SA_lock && seconds >= WAIT_SA_CONFIG){
            #ifdef _DEBUG_MODE
            Printf("\r\nInit RF");
            #endif
            pwr_lmt_sec = PWR_LMT_SEC;
            SA_init_done = 1;
            if(SA_lock){
                if(pwr_init == POWER_MAX+2){ //0mW
                    #if(0)
                    SA_dbm = pwr_to_dbm(pwr_init);
                    RF_POWER = POWER_MAX + 1;
                    Init_6300RF(ch_init, RF_POWER);
                    PIT_MODE = PIT_0MW;
                    #ifdef _DEBUG_MODE
                    Printf("\r\n SA_dbm:%bx",SA_dbm);
                    Printf("\r\n ch%bx, pwr%bx", ch_init, pwr_init);
                    #endif
                    #else
                    RF_POWER = POWER_MAX + 2;
                    cur_pwr = POWER_MAX + 2;
                    #endif
                }else if(PIT_MODE){
                    Init_6300RF(ch_init, POWER_MAX + 1);
                    #ifdef _DEBUG_MODE
                    Printf("\r\n ch%bx, pwr%bx", ch_init, cur_pwr);
                    #endif
                }else{
                    Init_6300RF(ch_init, pwr_init);
                    #ifdef _DEBUG_MODE
                    Printf("\r\n ch%bx, pwr%bx", ch_init, cur_pwr);
                    #endif
                }
            }else if(PIT_MODE){
                Init_6300RF(RF_FREQ, POWER_MAX+1);
                #ifdef _DEBUG_MODE
                Printf("\r\n ch%bx, pwr%bx", RF_FREQ, cur_pwr);
                #endif
            }else{
                Init_6300RF(RF_FREQ, 0);
                #ifdef _DEBUG_MODE
                Printf("\r\n ch%bx, pwr%bx", RF_FREQ, 0);
                #endif
            }
            
            DM6300_AUXADC_Calib();
        }
    }
}
#endif