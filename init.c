/*
 * init.c
 *
 *  Created on: 2016Äê4ÔÂ17ÈÕ
 *      Author: poiler
 */

#include "main.h"


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Init_Data_Cache( )                                             			*
 *      Enable D-Cache                                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void Init_Data_Cache(void)
{
    // Flush L1D
    L1DWB = 0x01;
    // Reconfigure L1D to default state (full cache)
    L1DCFG = 0x07;

}



/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Enable_Instruction_Cache( )                                             *
 *      Enable I-Cache                                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
/*
void Enable_Instruction_Cache(void)
{
    CPSR = 0x400000d3;              // Set to supervisor mode, disable IRQ/FIQ
    REG_CP15_I_CACHE = 1;           // Enable Instruction Cache

}
*/



/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  void Init_System_Config(void)                                           *
 *      Configure PINMUX and other system module registers                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void Init_System_Config(void)
{
    KICK0R	 = 0x83E70B13;  // Kick0 register + data (unlock)
    KICK1R	 = 0x95A4F1E0;  // Kick1 register + data (unlock)

    PINMUX0  = 0x111121F1;  // EMIFB, Reserved, GP7[14]
    PINMUX1  = 0x11111111;  // EMIFB
    PINMUX2  = 0xF1111111;  // Reserved, EMIFB
    PINMUX3  = 0xFFFFFFFF;  // Reserved
    PINMUX4  = 0xFFFFFFFF;  // Reserved
    PINMUX5  = 0x1111111F;  // EMIFB ,Reserved
    PINMUX6  = 0x11111111;  // EMIFB
    PINMUX7  = 0x11111111;  // SPI0, EMIFB
    PINMUX8  = 0x18822111;  // SPI1, GP5[11], GP5[10], I2C0_SCL, I2C0_SDA, SPI1
    PINMUX9  = 0x111111F1;  // McASP0, Reserved, SPI1
    PINMUX10 = 0x1111111F;  // McASP0(AXR0[6~0]), Reserved
    PINMUX11 = 0x82211188;  // GP4[10], EPWM0A, EPWM0B, AXR0[11], UART1_TXD, UART1_RXD, GP3[8], GP3[7]
    PINMUX12 = 0x8888288F;  // GP4[3~0], EHRPWMTZ, GP4[13~12], Reserved
    PINMUX13 = 0x22888888;  // SD, GP4[9~4]
    PINMUX14 = 0xFF222222;  // Reserved, SD
    PINMUX15 = 0x28FFFFFF;  // SD, GP1[0], Reserved,
    PINMUX16 = 0x88888882;  // GP1[9~3], SD
    PINMUX17 = 0xFFF88888;  // Reserved, GP1[14~10]
    PINMUX18 = 0xFF888F8F;  // Reserved, GP2[7~5], GP2[3]
    PINMUX19 = 0x00000008;  // GP2[10]

    /* PLL MMRs lock & TC1 32bits Burst Size (DBS) & TC0 32bits Burst Size (DBS) */
    CFGCHIP0 |= 0x00000005;

    /* All enabled eHRPWM module clocks are started with the first rising edge of TBCLK aligned. */
    CFGCHIP1 |= 0x00001000;

    KICK0R	 = 1; //Kick 0 Register (Write Protect)
    KICK1R	 = 1; //Kick 1 Register (Write Protect)
}



/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  void Init_PSC(void)                                                     *
 *      Enable all PSC modules on ALWAYSON and DSP power domains.           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
/*Enable Function for PSC0*/
void PSC0_lPSC_enable(unsigned int PD, unsigned int LPSC_num)
{
  *(unsigned int*) (PSC0_MDCTL+4*LPSC_num) = (*(unsigned int*) (PSC0_MDCTL+4*LPSC_num) & 0xFFFFFFE0) | 0x0003;
  PSC0_PTCMD = 0x1<<PD;
  while( (PSC0_PTSTAT & (0x1<<PD) ) !=0) ; /*Wait for power state transition to finish*/
  while( ((*(unsigned int*)(PSC0_MDSTAT+4*LPSC_num)) & 0x1F) !=0x3);
}


/*Enable Function for PSC1*/
void PSC1_lPSC_enable(unsigned int PD, unsigned int LPSC_num)
{
  *(unsigned int*) (PSC1_MDCTL+4*LPSC_num) = (*(unsigned int*) (PSC1_MDCTL+4*LPSC_num) & 0xFFFFFFE0) | 0x0003;
  PSC1_PTCMD = 0x1<<PD;
  while( (PSC1_PTSTAT & (0x1<<PD) ) !=0) ; /*Wait for power state transition to finish*/
  while( ((*(unsigned int*)(PSC1_MDSTAT+4*LPSC_num)) & 0x1F) !=0x3);
}


void Init_PSC(void)
{
    // PSC0
    PSC0_lPSC_enable(0, 0);	/*EDMA3 Channel Controller*/
    PSC0_lPSC_enable(0, 1);	/*EDMA3 Transfer Controller 0*/
    PSC0_lPSC_enable(0, 2);	/*EDMA3 Transfer Controller 1*/
    PSC0_lPSC_enable(0, 3);	/*EMIFA (BR7)*/
    PSC0_lPSC_enable(0, 4);	/*SPI0*/
    PSC0_lPSC_enable(0, 5);	/*MMC/SD0*/
    PSC0_lPSC_enable(0, 9);	/*UART0*/
    PSC0_lPSC_enable(0, 11);	/*SCR1 (BR4)*/
    PSC0_lPSC_enable(0, 12);	/*SCR2 (BR3, BR5, BR6)*/
    PSC0_lPSC_enable(0, 13);	/*PRU*/

    // PSC1
    PSC1_lPSC_enable(0, 1);	/*USB0 (USB2.0)*/
    PSC1_lPSC_enable(0, 2);	/*USB1 (USB1.1), no support the C6745 DSP*/
    PSC1_lPSC_enable(0, 3);	/*GPIO*/
    PSC1_lPSC_enable(0, 4);	/*HPI, no support  the C6745 DSP*/
    PSC1_lPSC_enable(0, 5);	/*EMAC*/
    PSC1_lPSC_enable(0, 6);	/*EMIFB (BR20)*/
    PSC1_lPSC_enable(0, 7);	/*McASP0 (+ McASP0 FIFO)*/
    PSC1_lPSC_enable(0, 8);	/*McASP1 (+ McASP1 FIFO)*/
    PSC1_lPSC_enable(0, 9);	/*McASP2 (+ McASP2 FIFO), no support the C6745 DSP*/
    PSC1_lPSC_enable(0, 10);	/*SPI1*/
    PSC1_lPSC_enable(0, 11);	/*I2C1*/
    PSC1_lPSC_enable(0, 12);	/*UART1*/
    PSC1_lPSC_enable(0, 13);	/*UART2*/
    PSC1_lPSC_enable(0, 16);	/*LCDC*/
    PSC1_lPSC_enable(0, 17);	/*eHRPWM0/1/2*/
    PSC1_lPSC_enable(0, 20);	/*eCAP0/1/2*/
    PSC1_lPSC_enable(0, 21);	/*eQEP0/1*/
    PSC1_lPSC_enable(0, 24);	/*SCR8 (BR15)*/
    PSC1_lPSC_enable(0, 25);	/*SCR7 (BR12)*/
    PSC1_lPSC_enable(0, 26);	/*SCR12 (BR18)*/
    PSC1_lPSC_enable(0, 31);	/*Shared RAM (BR13), no support the C6745 DSP*/
}



/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  void Init_PLL(void)                                                     *
 *      Configure PLL                                                       *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void Init_PLL(void)
{
	int i=0;

    /* DSP at 300MHz, EMIFs at 133MHz */
    unsigned int CLKMODE = 0;		//CLKMODE = 0, Internal oscillator(crystal)
    								//CLKMODE = 1, Square wave
    unsigned int PLLM = 29;		//PLL = 24M * (PLLM+1) = 24*30 = 720M
    unsigned int POSTDIV = 1;		//SYSCLK1 = 24M * (PLLM+1) / (POSTDIV+1) /1 = 360M for dsp
    unsigned int PLLDIV3 = 2;		//SYSCLK3 for EMIFA /3 = 100M
    unsigned int PLLDIV5 = 2;		//SYSCLK5 for EMIFB	/3 = 100M
    unsigned int PLLDIV7 = 5;		//SYSCLK7 for EMAC  /6 = 50M

    /*Set PLLEN=0 and PLLRST=1, PLL bypass mode*/
    PLL0_PLLCTL &= 0xFFFFFFFE;

    /*wait for 4 cycles to allow PLLEN mux switches properly to bypass clock*/
    for(i=0; i<PLLEN_MUX_SWITCH; i++) {;}

    /*Select the Clock Mode bit 8 as External Clock or On Chip Oscilator*/
    PLL0_PLLCTL &= 0xFFFFFEFF;
    PLL0_PLLCTL |= (CLKMODE<<8);

    /*Set PLLENSRC '0',bit 5, this bit must be cleared before PLLEN will have any effect*/
    PLL0_PLLCTL &=  0xFFFFFFDF;

    /*PLLCTL.EXTCLKSRC bit 9 should be left at 0 for Primus*/
    PLL0_PLLCTL &=  0xFFFFFDFF;

    /* Clear PLLRST bit to 0 -Reset the PLL */
    PLL0_PLLCTL &= 0xFFFFFFF7;

    /*Disable the PLL output*/
    PLL0_PLLCTL |= 0x10;

    /*Power up the PLL- PWRDN bit set to 0 to bring the PLL out of power down bit*/
    PLL0_PLLCTL &= 0xFFFFFFFD;

    /*Enable the PLL from Disable Mode PLLDIS bit to 0 - This is step is not required for Primus*/
    PLL0_PLLCTL &= 0xFFFFFFEF;

    /*PLL stabilisation time- take out this step , not required here when PLL in bypassmode*/
    //for(i=0; i<PLL_STABILIZATION_TIME; i++) {;}

    /*Program the required multiplier value in PLLM*/
    PLL0_PLLM = PLLM;

    /*If desired to scale all the SYSCLK frequencies of a given PLLC, program the POSTDIV ratio*/
    PLL0_POSTDIV = 0x8000 | POSTDIV;

    /*If Necessary program the PLLDIVx*/
    /*Check for the GOSTAT bit in PLLSTAT to clear to 0 to indicate that no GO operation is currently in progress*/
    while(PLL0_PLLSTAT & 0x1==1){}

    /*Program the RATIO field in PLLDIVx with the desired divide factors. In addition, make sure in this step you leave the PLLDIVx.DxEN bits set so clocks are still enabled (default).*/
    PLL0_PLLDIV3 = 0x8000 | PLLDIV3;
    PLL0_PLLDIV5 = 0x8000 | PLLDIV5;
    PLL0_PLLDIV7 = 0x8000 | PLLDIV7;

    /*Set the GOSET bit in PLLCMD to 1 to initiate a new divider transition.*/
    PLL0_PLLCMD |= 0x1;

    /*Wait for the GOSTAT bit in PLLSTAT to clear to 0 (completion of phase alignment).*/
    while(PLL0_PLLSTAT & 0x1==1) {}

    /*Wait for PLL to reset properly. See PLL spec for PLL reset time - This step is not required here -step11*/
    //for(i=0; i<PLL_RESET_TIME_CNT; i++) {;}

    /*Set the PLLRST bit in PLLCTL to 1 to bring the PLL out of reset*/
    PLL0_PLLCTL |= 0x8;

    /*Wait for PLL to lock. See PLL spec for PLL lock time*/
    for(i=0; i<PLL_LOCK_TIME_CNT; i++) {;}

    /*Set the PLLEN bit in PLLCTL to 1 to remove the PLL from bypass mode*/
    PLL0_PLLCTL |=  0x1;

    KICK0R = 0x83E70B13;	// Kick0 register + data (unlock)
    KICK1R = 0x95A4F1E0;	// Kick1 register + data (unlock)
    CFGCHIP3 |= 0x4;		// Enable 4.5 divider PLL
    CFGCHIP3 |= 0x1;		// Select 4.5 divider for EMIFB clock source only (not EMIFA)
    KICK0R = 1; //Kick 0 Register (Write Protect)
    KICK1R = 1; //Kick 1 Register (Write Protect)
}



/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  Init_EMIFB( )                                                           *
 *      Configure SDRAM.                                                    *
 *                                                                          *
 * ------------------------------------------------------------------------ */
void Init_EMIFB(void)
{
    // Micro MT48L4M16A2-75 SDRAM, 1Meg x 16 x 4banks (16-bit data path), 133MHz
    EMIFB_SDCFG = 0         // SDRAM Bank Config Register
        |( 1 << 15)         // Unlock timing registers
		|( 1 << 14)			// 16-bit SDR SDRAM
        |( 3 << 9 )         // CAS latency is 3
        |( 2 << 4 )         // 4 bank SDRAM devices
        |( 0 << 0 );        // PAGESIZE = 0, 256-word pages requiring 8 column address bits
    						// PAGESIZE = 1, 512-word pages requiring 9 column address bits

    EMIFB_SDRFC = 0         // SDRAM Refresh Control Register
        |( 0 << 31 )		// Low power mode disabled
        |( 0 << 30 )		// MCLK stoping disabled
        |( 0 << 23 )		// Selects self refresh instead of power down
        |( 1040 <<0);       // Refresh rate = 133M * 64ms / 8192 = 1039.06

    EMIFB_SDTIM1 = 0        // SDRAM Timing Register 1
        |( 8 << 25)         // (66ns / 7.5ns) - 1 = TRFC  @ 133MHz
        |( 2 << 22 )        // (20ns / 7.5ns) - 1 = TRP
        |( 2 << 19 )        // (20ns / 7.5ns) - 1 = TRCD
        |( 1 << 16 )        // (15ns / 7.5ns) - 1 = TWR
        |( 5 << 11 )        // (44ns / 7.5ns) - 1 = TRAS
        |( 8 <<  6 )        // (66ns / 7.5ns) - 1 = TRC
        |( 1 <<  3 );       // (15ns / 7.5ns) - 1 = TRRD
    						// *(((4 * 14ns) + (2 * 7.5ns)) / (4 * 7.5ns)) -1. = TRRD
                            // but it says to use this formula if 8 banks but only 4 are used here.
                            // and SDCFG1 register only suports upto 4 banks.

    EMIFB_SDTIM2 = 0        // SDRAM Timing Register 2
        |(14 << 27 )		// not sure how they got this number. the datasheet says value should be
                            // "Maximum number of refresh_rate intervals from Activate to Precharge command"
                            // but has no equation. TRASMAX is 120k.
        |( 9 << 16 )		// (75ns / 7.5ns) - 1 = TXSR
        |( 5 <<  0 );		// (44ns / 7.5ns) - 1 = TCKE

    EMIFB_SDCFG = 0         // SDRAM Bank Config Register
        |( 1 << 16 )		// SDRAM fully enabled
        |( 0 << 15 )		// Unlock timing registers
        |( 3 <<  9 )		// CAS latency is 3
        |( 2 <<  4 )		// 4 bank SDRAM devices
        |( 0 <<  0 );		// PAGESIZE = 0, 256-word pages requiring 8 column address bits
    						// PAGESIZE = 1, 512-word pages requiring 9 column address bits
}


void C6745_Init(void)
{
	Init_Data_Cache();
	Init_System_Config();
	Init_PLL();
	Init_PSC();
	Init_EMIFB();
}

