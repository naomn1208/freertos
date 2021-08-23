/*
 * QS_SCI_CFG.h
 *
 *  Created on: 2019/09/21
 *      Author: user
 */

#ifndef QS_SCI_CFG_H_
#define QS_SCI_CFG_H_


/*******************************************************************************
Macro definitions
*******************************************************************************/

/* **** Select SCI channel **** */
//#define SELECT_SCI0             /* SCI0 does not exist in products with 64 pins or 48 pins */
//#define SELECT_SCI1				/**/
//#define SELECT_SCI5
#define SELECT_SCI6					/**/
//#define SELECT_SCI8
//#define SELECT_SCI9             /* SCI9 does not exist in products with 48 pins */
//#define SELECT_SCI12


/* **** Configuration of SCIn **** */

/* ==== SCI0 ==== */
#ifdef SELECT_SCI0

    /* ---- I/O register settings ---- */
    #define SCIn                SCI0
    #define MSTP_SCIn           MSTP(SCI0)
    #define IPR_SCIn            IPR(SCI0, )
    #define IR_SCIn_ERIn        IR(SCI0,ERI0)
    #define IR_SCIn_RXIn        IR(SCI0,RXI0)
    #define IR_SCIn_TXIn        IR(SCI0,TXI0)
    #define IR_SCIn_TEIn        IR(SCI0,TEI0)
    #define IEN_SCIn_ERIn       IEN(SCI0,ERI0)
    #define IEN_SCIn_RXIn       IEN(SCI0,RXI0)
    #define IEN_SCIn_TXIn       IEN(SCI0,TXI0)
    #define IEN_SCIn_TEIn       IEN(SCI0,TEI0)
    #define RXDn_PDR            PORT2.PDR.BIT.B1
    #define RXDn_PMR            PORT2.PMR.BIT.B1
    #define RXDnPFS             P21PFS
    #define TXDn_PODR           PORT2.PODR.BIT.B0
    #define TXDn_PDR            PORT2.PDR.BIT.B0
    #define TXDn_PMR            PORT2.PMR.BIT.B0
    #define TXDnPFS             P20PFS
    #define PSEL_SETTING        (0x0A)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=214))
    #pragma interrupt (Excep_SCIn_RXIn(vect=215))
    #pragma interrupt (Excep_SCIn_TXIn(vect=216))
    #pragma interrupt (Excep_SCIn_TEIn(vect=217))

#endif

/* ==== SCI1 ==== */
#ifdef SELECT_SCI1

    /* ---- I/O register settings ---- */
    #define SCIn                SCI1
    #define MSTP_SCIn           MSTP(SCI1)
    #define IPR_SCIn            IPR(SCI1, )
    #define IR_SCIn_ERIn        IR(SCI1,ERI1)
    #define IR_SCIn_RXIn        IR(SCI1,RXI1)
    #define IR_SCIn_TXIn        IR(SCI1,TXI1)
    #define IR_SCIn_TEIn        IR(SCI1,TEI1)
    #define IEN_SCIn_ERIn       IEN(SCI1,ERI1)
    #define IEN_SCIn_RXIn       IEN(SCI1,RXI1)
    #define IEN_SCIn_TXIn       IEN(SCI1,TXI1)
    #define IEN_SCIn_TEIn       IEN(SCI1,TEI1)

//  #define RXDn_PDR            PORT1.PDR.BIT.B5
//  #define RXDn_PMR            PORT1.PMR.BIT.B5
//	#define RXDnPFS             P15PFS

// RX210
	#define RXDn_PDR            PORT3.PDR.BIT.B0
    #define RXDn_PMR            PORT3.PMR.BIT.B0
	#define RXDnPFS             P30PFS

//  #define TXDn_PODR           PORT1.PODR.BIT.B6
//  #define TXDn_PDR            PORT1.PDR.BIT.B6
//	#define TXDn_PMR            PORT1.PMR.BIT.B6
//	#define TXDnPFS             P16PFS

// RX210
	#define TXDn_PODR           PORT2.PODR.BIT.B6
	#define TXDn_PDR            PORT2.PDR.BIT.B6
    #define TXDn_PMR            PORT2.PMR.BIT.B6
	#define TXDnPFS             P26PFS

	#define PSEL_SETTING        (0x0A)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=218))
    #pragma interrupt (Excep_SCIn_RXIn(vect=219))
    #pragma interrupt (Excep_SCIn_TXIn(vect=220))
    #pragma interrupt (Excep_SCIn_TEIn(vect=221))

#endif

/* ==== SCI5 ==== */
#ifdef SELECT_SCI5

    /* ---- I/O register settings ---- */
    #define SCIn                SCI5
    #define MSTP_SCIn           MSTP(SCI5)
    #define IPR_SCIn            IPR(SCI5, )
    #define IR_SCIn_ERIn        IR(SCI5,ERI5)
    #define IR_SCIn_RXIn        IR(SCI5,RXI5)
    #define IR_SCIn_TXIn        IR(SCI5,TXI5)
    #define IR_SCIn_TEIn        IR(SCI5,TEI5)
    #define IEN_SCIn_ERIn       IEN(SCI5,ERI5)
    #define IEN_SCIn_RXIn       IEN(SCI5,RXI5)
    #define IEN_SCIn_TXIn       IEN(SCI5,TXI5)
    #define IEN_SCIn_TEIn       IEN(SCI5,TEI5)
    #define RXDn_PDR            PORTA.PDR.BIT.B3
    #define RXDn_PMR            PORTA.PMR.BIT.B3
    #define RXDnPFS             PA3PFS
    #define TXDn_PODR           PORTA.PODR.BIT.B4
    #define TXDn_PDR            PORTA.PDR.BIT.B4
    #define TXDn_PMR            PORTA.PMR.BIT.B4
    #define TXDnPFS             PA4PFS
    #define PSEL_SETTING        (0x0A)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=222))
    #pragma interrupt (Excep_SCIn_RXIn(vect=223))
    #pragma interrupt (Excep_SCIn_TXIn(vect=224))
    #pragma interrupt (Excep_SCIn_TEIn(vect=225))

#endif

/* ==== SCI6 ==== */
#ifdef SELECT_SCI6

    /* ---- I/O register settings ---- */
    #define SCIn                SCI6
    #define MSTP_SCIn           MSTP(SCI6)
    #define IPR_SCIn            IPR(SCI6, )
    #define IR_SCIn_ERIn        IR(SCI6,ERI6)
    #define IR_SCIn_RXIn        IR(SCI6,RXI6)
    #define IR_SCIn_TXIn        IR(SCI6,TXI6)
    #define IR_SCIn_TEIn        IR(SCI6,TEI6)
    #define IEN_SCIn_ERIn       IEN(SCI6,ERI6)
    #define IEN_SCIn_RXIn       IEN(SCI6,RXI6)
    #define IEN_SCIn_TXIn       IEN(SCI6,TXI6)
    #define IEN_SCIn_TEIn       IEN(SCI6,TEI6)

#if 0
	#define RXDn_PDR            PORTB.PDR.BIT.B0
    #define RXDn_PMR            PORTB.PMR.BIT.B0
    #define RXDnPFS             PB0PFS
    #define TXDn_PODR           PORTB.PODR.BIT.B1
    #define TXDn_PDR            PORTB.PDR.BIT.B1
    #define TXDn_PMR            PORTB.PMR.BIT.B1
    #define TXDnPFS             PB1PFS
#endif

	#define RXDn_PDR            PORT3.PDR.BIT.B3
    #define RXDn_PMR            PORT3.PMR.BIT.B3
    #define RXDnPFS             P33PFS
    #define TXDn_PODR           PORT3.PODR.BIT.B2
    #define TXDn_PDR            PORT3.PDR.BIT.B2
    #define TXDn_PMR            PORT3.PMR.BIT.B2
    #define TXDnPFS             P32PFS

	#define PSEL_SETTING        (0x0B)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=226))
    #pragma interrupt (Excep_SCIn_RXIn(vect=227))
    #pragma interrupt (Excep_SCIn_TXIn(vect=228))
    #pragma interrupt (Excep_SCIn_TEIn(vect=229))

#endif

/* ==== SCI8 ==== */
#ifdef SELECT_SCI8

    /* ---- I/O register settings ---- */
    #define SCIn                SCI8
    #define MSTP_SCIn           MSTP(SCI8)
    #define IPR_SCIn            IPR(SCI8, )
    #define IR_SCIn_ERIn        IR(SCI8,ERI8)
    #define IR_SCIn_RXIn        IR(SCI8,RXI8)
    #define IR_SCIn_TXIn        IR(SCI8,TXI8)
    #define IR_SCIn_TEIn        IR(SCI8,TEI8)
    #define IEN_SCIn_ERIn       IEN(SCI8,ERI8)
    #define IEN_SCIn_RXIn       IEN(SCI8,RXI8)
    #define IEN_SCIn_TXIn       IEN(SCI8,TXI8)
    #define IEN_SCIn_TEIn       IEN(SCI8,TEI8)
    #define RXDn_PDR            PORTC.PDR.BIT.B6
    #define RXDn_PMR            PORTC.PMR.BIT.B6
    #define RXDnPFS             PC6PFS
    #define TXDn_PODR           PORTC.PODR.BIT.B7
    #define TXDn_PDR            PORTC.PDR.BIT.B7
    #define TXDn_PMR            PORTC.PMR.BIT.B7
    #define TXDnPFS             PC7PFS
    #define PSEL_SETTING        (0x0A)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=230))
    #pragma interrupt (Excep_SCIn_RXIn(vect=231))
    #pragma interrupt (Excep_SCIn_TXIn(vect=232))
    #pragma interrupt (Excep_SCIn_TEIn(vect=233))

#endif

/* ==== SCI9 ==== */
#ifdef SELECT_SCI9

    /* ---- I/O register settings ---- */
    #define SCIn                SCI9
    #define MSTP_SCIn           MSTP(SCI9)
    #define IPR_SCIn            IPR(SCI9, )
    #define IR_SCIn_ERIn        IR(SCI9,ERI9)
    #define IR_SCIn_RXIn        IR(SCI9,RXI9)
    #define IR_SCIn_TXIn        IR(SCI9,TXI9)
    #define IR_SCIn_TEIn        IR(SCI9,TEI9)
    #define IEN_SCIn_ERIn       IEN(SCI9,ERI9)
    #define IEN_SCIn_RXIn       IEN(SCI9,RXI9)
    #define IEN_SCIn_TXIn       IEN(SCI9,TXI9)
    #define IEN_SCIn_TEIn       IEN(SCI9,TEI9)
    #define RXDn_PDR            PORTB.PDR.BIT.B6
    #define RXDn_PMR            PORTB.PMR.BIT.B6
    #define RXDnPFS             PB6PFS
    #define TXDn_PODR           PORTB.PODR.BIT.B7
    #define TXDn_PDR            PORTB.PDR.BIT.B7
    #define TXDn_PMR            PORTB.PMR.BIT.B7
    #define TXDnPFS             PB7PFS
    #define PSEL_SETTING        (0x0A)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=234))
    #pragma interrupt (Excep_SCIn_RXIn(vect=235))
    #pragma interrupt (Excep_SCIn_TXIn(vect=236))
    #pragma interrupt (Excep_SCIn_TEIn(vect=237))

#endif

/* ==== SCI12 ==== */
#ifdef SELECT_SCI12

    /* ---- I/O register settings ---- */
    #define SCIn                SCI12
    #define MSTP_SCIn           MSTP(SCI12)
    #define IPR_SCIn            IPR(SCI12, )
    #define IR_SCIn_ERIn        IR(SCI12,ERI12)
    #define IR_SCIn_RXIn        IR(SCI12,RXI12)
    #define IR_SCIn_TXIn        IR(SCI12,TXI12)
    #define IR_SCIn_TEIn        IR(SCI12,TEI12)
    #define IEN_SCIn_ERIn       IEN(SCI12,ERI12)
    #define IEN_SCIn_RXIn       IEN(SCI12,RXI12)
    #define IEN_SCIn_TXIn       IEN(SCI12,TXI12)
    #define IEN_SCIn_TEIn       IEN(SCI12,TEI12)
    #define RXDn_PDR            PORTE.PDR.BIT.B2
    #define RXDn_PMR            PORTE.PMR.BIT.B2
    #define RXDnPFS             PE2PFS
    #define TXDn_PODR           PORTE.PODR.BIT.B1
    #define TXDn_PDR            PORTE.PDR.BIT.B1
    #define TXDn_PMR            PORTE.PMR.BIT.B1
    #define TXDnPFS             PE1PFS
    #define PSEL_SETTING        (0x0C)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=238))
    #pragma interrupt (Excep_SCIn_RXIn(vect=239))
    #pragma interrupt (Excep_SCIn_TXIn(vect=240))
    #pragma interrupt (Excep_SCIn_TEIn(vect=241))

#endif


#endif /* QS_SCI_CFG_H_ */
