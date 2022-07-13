#include "isr.h"
uint8_t int0_req = 0;
uint8_t int1_req = 0;
uint8_t btn1_tflg = 0;
uint8_t pwr_sflg = 0;    //power autoswitch flag
uint8_t pwr_tflg = 0;
uint8_t cfg_tflg = 0;
uint8_t temp_tflg = 0;
uint8_t timer_4hz = 0;
uint8_t timer_8hz = 0;
uint8_t timer_16hz = 0;
uint16_t __idata timer_ms10x = 0;
uint16_t seconds = 0;
uint8_t RS0_ERR = 0;