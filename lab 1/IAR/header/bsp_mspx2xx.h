#ifndef _bsp_mspx2xx_H_
#define _bsp_mspx2xx_H_


#define LEDsArrayPort     		P1OUT
#define LEDsArrayPortDir  		P1DIR
#define LEDsArrayPortSel  		P1SEL

// PB Port

#define PBsArrayPort			P2IN
#define PBsArrayIntPending		P2IFG
#define PBsArrayIntEnable		P2IE
#define PBsArrayIntEdgeSel		P2IES
#define PBsArrayPortSel   		P2SEL
#define PBsArrayPortDir  		P2DIR

// PWM Port

#define PWMPort                         P2OUT


// Init Program

extern void InitGPIO(void);

#endif