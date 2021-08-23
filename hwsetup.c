/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Technology Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Technology Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* TECHNOLOGY CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THE THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2008. Renesas Technology Corp., All Rights Reserved.
*******************************************************************************	
* File Name    : hwsetup.c
* Version      : 1.00
* Description  : Power up hardware initializations
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 15.02.2010 1.00    First Release
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include "iodefine.h"							/* (xx) */

#include "rskrx210def.h"						/* (xx) */
//#include "../include/hd44780.h"  /* EZ-LCD include file */	/* (xx) rx210 v8	hd44780利用しない　*/
//#include "r_cg_sci.h"							/* (xx) r210 v */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
void io_set_cpg(void);
void ConfigurePortPins(void);
void EnablePeripheralModules(void);


/******************************************************************************
* Function Name: HardwareSetup
* Description  : This function does initial setting for CPG port pins used in
*              : the Demo including the MII pins of the Ethernet PHY connection.
* Arguments    : none
* Return Value : none
******************************************************************************/
void HardwareSetup(void)
{
	/* CPG setting */
	io_set_cpg();						/* (xx) clock 周りの処理等 */

	/* Setup the port pins */
	ConfigurePortPins();				/* (xx) 記述は残す。利用するなら関数編集する。 */

    /* Enables peripherals */
    EnablePeripheralModules();			/* (xx) INCLUDE_LCDを探してディセーブルする。 */
#if INCLUDE_LCD == 1						
    /* Initialize display */
    InitialiseDisplay();
#endif

#if USE_SCI0
//    R_SCI0_Create();					/* (xx) */
#endif
#if USE_SCI1
//    R_SCI1_Create();					/* (xx) */
#endif
#if USE_SCI6
//    R_SCI6_Create();					/* (xx) */
#endif

}

/******************************************************************************
* Function Name: EnablePeripheralModules
* Description  : Enables Peripheral Modules before use
* Arguments    : none
* Return Value : none
******************************************************************************/
void EnablePeripheralModules(void)
{
#if 0
	/*  Module standby clear */
    SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;		/* CMT0 */
#else	/* (xx) --- rx210 v8 --- */	
	SYSTEM.PRCR.WORD = 0x0A502;
	MSTP( CMT0 ) = 0;
	SYSTEM.PRCR.WORD = 0x0A500;
#endif

}

/******************************************************************************
* Function Name: ConfigurePortPins
* Description  : Configures port pins.
* Arguments    : none
* Return Value : none
******************************************************************************/
void ConfigurePortPins(void)
{
#if 0	/* (xx) --- rx210 v8 --- */	

/* Port pins default to inputs.
 * To ensure safe initialisation set the pin states　before changing the data direction registers.
 * This will avoid any unintentional　state changes on the external ports.
 * Many peripheral modules will override the setting of the port registers.
 * Ensure　that the state is safe for external devices
 * if the internal peripheral module is　disabled or powered down. */
    /* Configure LED 0-4 pin settings */
    PORT1.PODR.BIT.B4 = 1; 
    PORT1.PODR.BIT.B5 = 1;
    PORT1.PODR.BIT.B6 = 1;
    PORT1.PODR.BIT.B7 = 1;

    PORT1.PDR.BIT.B4 = 1; 
    PORT1.PDR.BIT.B5 = 1;
    PORT1.PDR.BIT.B6 = 1;
    PORT1.PDR.BIT.B7 = 1;

#else
    PORT5.PODR.BIT.B0 = 1;
    PORT5.PODR.BIT.B1 = 1;
    PORT5.PODR.BIT.B2 = 1;
    PORT5.PODR.BIT.B3 = 1;

    PORT5.PODR.BIT.B4 = 1;
    PORT5.PODR.BIT.B5 = 1;

    PORT5.PDR.BIT.B0 = 1;
    PORT5.PDR.BIT.B1 = 1;
    PORT5.PDR.BIT.B2 = 1;
    PORT5.PDR.BIT.B3 = 1;

    PORT5.PDR.BIT.B4 = 1;
    PORT5.PDR.BIT.B5 = 1;

#endif   


#if INCLUDE_LCD == 1
    /* Set LCD pins as outputs */
    /* LCD-RS */
    PORTJ.PDR.BIT.B1 = 1;
    /* LCD-EN */
    PORTJ.PDR.BIT.B3 = 1;
    /*LCD-data */
    PORTH.PDR.BYTE = 0x0F;

#endif
}

/******************************************************************************
* Function Name: io_set_cpg
* Description  : Sets up operating speed
* Arguments    : none
* Return Value : none
******************************************************************************/
void io_set_cpg(void)
{
	/* Set CPU PLL operating frequencies.
	 * Changes to the peripheral clock will require　changes to the debugger and flash kernel BRR settings. */

	/* ==== CPG setting ==== */

	unsigned int i;

#if 0
	SYSTEM.PRCR.WORD = 0xA503;				/* Protect off 						*/
#else	/* (xx) --- rx210 v8 --- */	
	SYSTEM.PRCR.WORD = 0xA507;				/* Protect off 						*/
											/* PRC0=1	; クロック発生回路関連レジスタへの書き込み許可
											 * PRC1=1	; 動作モード、消費電力低減機能、ソフトウェアリセット関連レジスタへの書き込み許可
											 * PRC2=1	; VRCRレジスタへの書き込み許可
											 * PRC3=0	; LVD関連レジスタへの書き込み許可
											 * */
#endif

/* -----------------------------------------------------------
		CLK_SRC_HOCO = 0 を想定する。
		CLK_SRC_HOCO = 1 は未検証
----------------------------------------------------------- */
#if (CLK_SRC_HOCO == 1)		/* (CLK_SRC_HOCO == 1) */
	SYSTEM.HOCOPCR.BYTE = 0x00;				/* HOCO power supply on				*/
	SYSTEM.HOCOCR2.BYTE = 0x03;				/* Select - 50MHz					*/
	SYSTEM.HOCOCR.BYTE  = 0x01;				/* HOCO is operating				*/

	for(i=0; i<10; i++){					/* wait over 60us					*/
	}

#else	/* (CLK_SRC_HOCO == 1) */

#if 0	/* (xx) --- rx210 v8 --- */
	SYSTEM.MOSCWTCR.BYTE = 0x0C;			/* Main Clock Oscillator Wait Control Register */
											/* 65536 states 						*/
											/* wait over 2 ms  @20MHz 				*/

	SYSTEM.PLLWTCR.BYTE = 0x0B;				/* PLL Wait Control Register 			*/
											/* 262144 states 						*/
											/* wait over 2.1 ms  @PLL = 80Hz		*/
											/*					(20/2x8*8) 			*/
	
	SYSTEM.PLLCR.WORD = 0x0701;				/* x8 @PLL 								*/
											/* Input to PLL (EXTAL in) / 2 			*/
											/* Therefore:
												PLL 	= EXTAL / 2
														= 20M / 2
														= 10MHz
												PLL * 8 = 80Mhz						*/
	
	SYSTEM.MOSCCR.BYTE = 0x02;				/* EXTAL ON								*/
											/* External oscillation input selection	*/
	SYSTEM.PLLCR2.BYTE = 0x00;				/* PLL ON								*/
	
	for(i = 0; i<263; i++){					/* wait over 2.1ms */

#else	/* (xx) --- rx210 v8 --- */	
    /* ---- Set the VRCR register ---- */
    SYSTEM.VRCR = 0x00;
    SYSTEM.MOFCR.BYTE = (0x30);				/* Drive capability : 20 MHz crystal resonator	*/

	SYSTEM.MOSCWTCR.BYTE = 0x0D;			/* Main Clock Oscillator Wait Control Register	*/
											/* 131072 cycles (approx. 6.55 ms).				*/
											/* wait over 2 ms  @20MHz						*/

	SYSTEM.PLLWTCR.BYTE = 0x0B;				/* PLL Wait Control Register					*/
											/* 262144 states								*/
											/* wait over 2.1 ms  @PLL = 80Hz				*/
											/*					(20/2x8*8)					*/
	
	SYSTEM.MOSCCR.BYTE = 0x00;				/* EXTAL OFF 									*/
    while (0x00 != SYSTEM.MOSCCR.BYTE)
    {
        /* Confirm that the written value can be read correctly. */
    }
    /* ---- Wait processing for the clock oscillation stabilization ---- */
    for(i=0;i<100;i++) {
    }



    SYSTEM.PLLCR.WORD = (0x0901);			/* Division ratio and multiplication factor : divide-by-2, multiply-by-10 */
											/* Input to PLL (EXTAL in) / 2 	*/
											/* Therefore:
													PLL = EXTAL / 2
														= 20M / 2
														= 10MHz
												PLL * 8 = 80Mhz
												PLL *10 = 100MHz	*/
    SYSTEM.PLLWTCR.BYTE = (0x09);			/* Wait control register : 65536 cycles (approx. 655.36 us) */
    SYSTEM.PLLCR2.BYTE = 0x00;				/* PLL enable			*/
    /* ---- Wait processing for the clock oscillation stabilization ---- */
    for(i=0;i<100;i++) {
    }

    SYSTEM.SCKCR.LONG = 0x21821211;			/* ICLK,PCLKD: divide-by-2 PCLKB,BCLK,FCLK: divide-by-4 */
    while (0x21821211 != SYSTEM.SCKCR.LONG)
    {
         /* Confirm that the written value can be read correctly. */
    }

    SYSTEM.BCKCR.BYTE = 0x01;				/* ---- Set the BCLK pin output ---- */
    										/* BCLK/2 */
    while (0x01 != SYSTEM.BCKCR.BYTE)
    {
        /* Confirm that the written value can be read correctly. */
    }
#endif		/* (xx) --- rx210 v8 --- */

#endif	/* (CLK_SRC_HOCO == 1) */
	
//	SYSTEM.SCKCR.LONG = 0x21823333;			/* ICK=PLL/2,FCK,PCK,BCL=PLL/4 */
/************************************************************************/
/*  If setting bits individually, rather than a single long write, 		*/
/*	set the BCK value before that of ICK 								*/
/************************************************************************/
/*
	SYSTEM.SCKCR.BIT 関連は、v8の時点で、(CLK_SRC_HOCO == 1)	の場合に設定する記述となっていた。
		割り当てのない　パラメータはコメントアウトし記述を残すことことにしている。

	SYSTEM.SCKCR = 0x 2182 1211					<< 前方にて、以下のように定義している。
													SYSTEM.SCKCR.LONG = 0x21821211

	SYSTEM.SCKCR.BIT.PCKD 	= 1;	[4]		PCKD[3:0]		PLL/2 = 100MHz / 2 = 50MHz
	--- 					= 1;	[4]		---[7:4]		0001b	rx210 v 予約
	SYSTEM.SCKCR.BIT.PCKB 	= 2;	[4]		PCKB[11:8]		PLL/4 = 100MHz /4 = 25MHzHz
	---						= 1;	[4]		---[15:12]		0001b	rx210 v 予約
	SYSTEM.SCKCR.BIT.BCK 	= 2;	[4]		BCK[19:16]		PLL/4 = 100MHz /4 = 25MHzHz
	---						= 0;	[3]		---[22:20]		0b		rx210 v 予約
	SYSTEM.SCKCR.BIT.PSTOP1	= 1;	[1]		PSTOP1[23:23]	BUS CLK OUT Disabled
	SYSTEM.SCKCR.BIT.ICK 	= 1;	[4]		ICK[27:24]		PLL/2 = 100MHz / 2 = 50MHz
	SYSTEM.SCKCR.BIT.FCK 	= 2;	[4]		FCK[31:28]		PLL/4 = 100MHz / 4 = 25MHz

*/
#if 0
	SYSTEM.SCKCR.BIT.PCKD 	= 3;			/* PLL/8 = 10MHz		*/
	SYSTEM.SCKCR.BIT.PCKC 	= 3;			/* PLL/8 = 10MHz		*/
	SYSTEM.SCKCR.BIT.PCKB 	= 3;			/* PLL/8 = 10MHz		*/
	SYSTEM.SCKCR.BIT.PCKA 	= 3;			/* PLL/8 = 10MHz		*/
	SYSTEM.SCKCR.BIT.BCK 	= 3;			/* PLL/8 = 10MHz		*/
	SYSTEM.SCKCR.BIT.PSTOP1 = 1;			/* BUS CLK OUT Disabled */
	SYSTEM.SCKCR.BIT.ICK 	= 1;			/* PLL/2 = 40MHz		*/
	SYSTEM.SCKCR.BIT.FCK 	= 2;			/* PLL/4 = 20MHz		*/
#endif
	
#if 0
	while(SYSTEM.OPCCR.BIT.OPCMTSF == 1);
	SYSTEM.OPCCR.BIT.OLPCM = 0;				/* (xx) rx210 v 予約 */
	while(SYSTEM.OPCCR.BIT.OPCMTSF == 1);
#else	/* (xx) --- rx210 v8 --- */	
	while(SYSTEM.OPCCR.BIT.OPCMTSF == 1);	/* 遷移中 */
	SYSTEM.OPCCR.BIT.OPCMTSF = 0;			/* High-speed operating mode */
	while(SYSTEM.OPCCR.BIT.OPCMTSF == 1);	/* 遷移中 */
#endif

#if (CLK_SRC_HOCO == 1)	
	SYSTEM.SCKCR3.WORD = 0x0100;			/* LOCO -> HOCO			*/
#else	/* (xx) --- rx210 v8 --- */	
	SYSTEM.SCKCR3.WORD = 0x0400;			/* LOCO -> set PLL		*/
#endif

#if 1	/* (xx) --- rx210 v8 --- */	
	SYSTEM.PRCR.WORD = 0xA500;				/* Protect on			*/
#endif

}
