/*
 * QS_SCI.c
 *
 *  Created on: 2019/09/21
 *      Author: user
 */


/*******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include "iodefine.h"
#include "QS_SCI_CFG.h"
#include "QS_SCI.h"

//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* SSR - Serial Status Register
b7:b6  Reserved - The read value is undefined. The write value should be 1.
b5     ORER     - Overrun Error Flag    - An overrun error has occurred
b4     FER      - Framing Error Flag    - A framing error has occurred
b3     PER      - Parity Error Flag     - A parity error has occurred */
#define SSR_ERROR_FLAGS     (0x38)

/*******************************************************************************
Private variables and functions
*******************************************************************************/
/* **** SCI state **** */
static sci_state_t  state = {0x00};
#define B_TX_BUSY   state.bit.b_tx_busy     /* Transmission busy flag */
#define B_RX_BUSY   state.bit.b_rx_busy     /* Reception busy flag */
#define B_RX_ORER   state.bit.b_rx_orer     /* Overrun error flag */
#define B_RX_FER    state.bit.b_rx_fer      /* Framing error flag */

/* **** SCI transmit/receive buffer **** */
static const uint8_t * pbuf_tx;             /* Pointer to transmit buffer */
static       uint8_t   tx_cnt;              /* Transmit counter */

static       uint8_t * pbuf_rx;             /* Pointer to receive buffer */
static       uint8_t   rx_cnt;              /* Receive counter */

/* **** Pointer to callback function **** */
CallBackFunc pcb_sci_tx_end;
CallBackFunc pcb_sci_rx_end;
CallBackFunc pcb_sci_rx_error;

/* **** SCI interrupt service routine **** */
static void sci_txi_isr(void);
static void sci_tei_isr(void);
static void sci_rxi_isr(void);
static void sci_eri_isr(void);
static void Excep_SCIn_ERIn(void);
static void Excep_SCIn_RXIn(void);
static void Excep_SCIn_TXIn(void);
static void Excep_SCIn_TEIn(void);


/*******************************************************************************
* Outline      : User I/F function (Initialization of SCI)
* Header       : sci.h
* Function Name: SCI_Init
* Description  : Initialize SCI.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void SCI_Init (void)
{

    /* ---- SCI interrupt request is disabled ---- */
    IEN_SCIn_ERIn = 0;
    IEN_SCIn_RXIn = 0;
    IEN_SCIn_TXIn = 0;
    IEN_SCIn_TEIn = 0;

    /* ---- Initialization of SCI ---- */

    /* PRCR - Protect Register
    b15:b8 PRKEY    - PRC Key Code  - A5h (The write value should be A5h to permission writing PRCi bit)
    b7:b4  Reserved - The write value should be 0.
    b1     PRC1     - Protect Bit 1 - Write enabled */
    SYSTEM.PRCR.WORD = 0xA502;

    /* The module stop state of SCIn is canceled */
    MSTP_SCIn = 0;

    /* Enable write protection */
    SYSTEM.PRCR.WORD = 0xA500;

    /* SCR - Serial Control Register
    b7     TIE  - Transmit Interrupt Enable     - A TXI interrupt request is disabled
    b6     RIE  - Receive Interrupt Enable      - RXI and ERI interrupt requests are disabled
    b5     TE   - Transmit Enable               - Serial transmission is disabled
    b4     RE   - Receive Enable                - Serial reception is disabled
    b2     TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled */
    SCIn.SCR.BYTE = 0x00;

    while (0x00 != (SCIn.SCR.BYTE & 0xF0))
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Set the I/O port functions ---- */

    /* Set port output data - High level */
    TXDn_PODR = 1;

    /* Set port direction - TXDn is output port, RXDn is input port */
    TXDn_PDR = 1;
    RXDn_PDR = 0;

    /* Set port mode - Use pin as general I/O port */
    TXDn_PMR = 0;
    RXDn_PMR = 0;

    /* PWPR - Write-Protect Register
    b7     B0WI     - PFSWE Bit Write Disable   - Writing to the PFSWE bit is enabled
    b6     PFSWE    - PFS Register Write Enable - Writing to the PFS register is enabled
    b5:b0  Reserved - These bits are read as 0. The write value should be 0. */
    MPC.PWPR.BIT.B0WI  = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    /* PFS - Pin Function Control Register
    b3:b0  PSEL - Pin Function Select - RXDn, TXDn */
    MPC.TXDnPFS.BYTE = PSEL_SETTING;
    MPC.RXDnPFS.BYTE = PSEL_SETTING;

    /* Enable write protection */
    MPC.PWPR.BIT.PFSWE = 0;
    MPC.PWPR.BIT.B0WI  = 1;

    /* Use pin as I/O port for peripheral functions */
    RXDn_PMR = 1;

    /* ---- Initialization of SCI ---- */

    /* Select an On-chip baud rate generator to the clock source */
    SCIn.SCR.BIT.CKE = 0;

    /* SMR - Serial Mode Register
    b7     CM   - Communications Mode   - Asynchronous mode
    b6     CHR  - Character Length      - Selects 8 bits as the data length
    b5     PE   - Parity Enable         - When transmitting : Parity bit addition is not performed
                                          When receiving    : Parity bit checking is not performed
    b3     STOP - Stop Bit Length       - 2 stop bits
    b2     MP   - Multi-Processor Mode  - Multi-processor communications function is disabled
    b1:b0  CKS  - Clock Select          - PCLK clock (n = 0) */
//    SCIn.SMR.BYTE = 0x08;
    SCIn.SMR.BYTE = 0x00;

    /* SCMR - Smart Card Mode Register
    b6:b4  Reserved - The write value should be 1.
    b3     SDIR     - Transmitted/Received Data Transfer Direction - Transfer with LSB-first
    b2     SINV     - Transmitted/Received Data Invert  - TDR contents are transmitted as they are.
                                                          Receive data is stored as it is in RDR.
    b1     Reserved - The write value should be 1.
    b0     SMIF     - Smart Card Interface Mode Select  - Serial communications interface mode */
    SCIn.SCMR.BYTE = 0xF2;

    /* SEMR - Serial Extended Mode Register
    b7:b6  Reserved - The write value should be 0.
    b5     NFEN     - Digital Noise Filter Function Enable  - Noise cancellation function
                                                              for the RXDn input signal is disabled.
    b4     ABCS     - Asynchronous Mode Base Clock Select   - Selects 16 base clock cycles for 1-bit period
    b3:b1  Reserved - The write value should be 0. */
 //   SCIn.SEMR.BYTE = 0x00;
    SCIn.SEMR.BYTE = 0x10;

    /* BRR - Bit Rate Register
    Bit Rate: (25MHz/(64*2^(-1)*57600bps))-1=12.56 */
//    SCIn.BRR = 13;

    /*
     	 Bit Rate:
     	 ABCS=OFF
     	  (25MHz / (64/2* 9600bps)) -1 = 80.3802083333333
     	  (25MHz / (64/2*19200bps)) -1 = 39.6901041666667
     	  (25MHz / (64/2*38400bps)) -1 = 19.3450520833333

     	 ABCS=ON
     	  (25MHz / (32/2* 9600bps)) -1 = 161.760416666667
     	  (25MHz / (32/2*19200bps)) -1 = 80.3802083333333
     	  (25MHz / (32/2*38400bps)) -1 = 39.6901041666667
    */
        SCIn.BRR = 40;

    /* ---- Initialization of SCI interrupt ---- */

    /* SCI interrupt priority level is 1 */
    IPR_SCIn = 1;

    /* Interrupt request is cleared (Edge interrupt) */
    IR_SCIn_RXIn = 0;
    IR_SCIn_TXIn = 0;

}

/*******************************************************************************
* Outline      : User I/F function (SCI reception start)
* Header       : sci.h
* Function Name: SCI_StartReceive
* Description  : Start SCI reception.
* Arguments    : pbuf -
*                   Pointer to where to store the received data
*                num -
*                   Number of receive byte
*                pcb_rx_end -
*                   Pointer to callback function (SCI reception end)
*                pcb_rx_error -
*                   Pointer to callback function (SCI receive error)
* Return Value : SCI_NG    -
*                   Parameter error
*                SCI_BUSY  -
*                   Reception busy
*                SCI_OK    -
*                   Reception start
*******************************************************************************/
uint8_t SCI_StartReceive (uint8_t * pbuf, uint8_t num, CallBackFunc pcb_rx_end, CallBackFunc pcb_rx_error)
{

    uint8_t return_value = SCI_NG;

    /* **** Check the parameter **** */
    if (0 == num)
    {
        return (return_value);
    }

    /* **** Check the reception state **** */
    if (1 == B_RX_BUSY)
    {
        /* ==== Busy ==== */
        return_value = SCI_BUSY;
    }
    else
    {
        /* ==== Ready ==== */

        /* ---- Set the RAM ---- */
        B_RX_BUSY        = 1;
        B_RX_ORER        = 0;
        B_RX_FER         = 0;
        pbuf_rx          = pbuf;
        rx_cnt           = num;
        pcb_sci_rx_end   = pcb_rx_end;
        pcb_sci_rx_error = pcb_rx_error;

        /* ---- Enable serial reception ---- */

        /* RXI and ERI interrupt requests are enabled */
        SCIn.SCR.BIT.RIE = 1;

        /* Serial reception is enabled */
        SCIn.SCR.BIT.RE = 1;                /* Setting the RE bit allows RXDn to be used. */

        /* ---- Enable ERI and RXI interrupt requests ---- */
        IEN_SCIn_ERIn = 1;
        IEN_SCIn_RXIn = 1;

        return_value = SCI_OK;
    }

    return (return_value);

}

uint8_t SCI_GettReceiveNum()
{
	return rx_cnt;
}

/*******************************************************************************
* Outline      : User I/F function (SCI transmission start)
* Header       : sci.h
* Function Name: SCI_StartTransmit
* Description  : Start SCI transmission.
* Arguments    : pbuf -
*                   Pointer to where to store the transmit data
*                num  -
*                   Number of transmit byte
*                pcb_tx_end -
*                   Pointer to callback function (SCI transmission end)
* Return Value : SCI_NG   -
*                   Parameter error
*                SCI_BUSY -
*                   Transmission busy
*                SCI_OK   -
*                   Transmission start
*******************************************************************************/
uint8_t SCI_StartTransmit (const uint8_t * pbuf, uint8_t num, CallBackFunc pcb_tx_end)
{

    uint8_t return_value = SCI_NG;

    /* **** Check the parameter **** */
    if (0 == num)
    {
        return (return_value);
    }

    /* **** Check the reception state **** */
    if (1 == B_TX_BUSY)
    {
        /* ==== Busy ==== */
        return_value = SCI_BUSY;
    }
    else
    {
        /* ==== Ready ==== */

        /* ---- Set the RAM ---- */
        B_TX_BUSY      = 1;
        pbuf_tx        = pbuf;
        tx_cnt         = num;
        pcb_sci_tx_end = pcb_tx_end;

        /* ---- Enable serial transmission ---- */

        /* A TXI interrupt request is enabled */
        SCIn.SCR.BIT.TIE = 1;

        /* Serial transmission is enabled */
        SCIn.SCR.BIT.TE = 1;                /* Setting the TE bit allows TXDn to be used. */

        /* Use pin as I/O port for peripheral functions */
        TXDn_PMR = 1;

        /* ---- Enable TXI interrupt request ---- */
        IEN_SCIn_TXIn = 1;

        return_value = SCI_OK;
    }

    return (return_value);

}

/*******************************************************************************
* Outline      : User I/F function (Get SCI state)
* Header       : sci.h
* Function Name: SCI_GetState
* Description  : Return the SCI state.
* Arguments    : none
* Return Value : sci_state_t.bit.b_tx_busy - Transmission busy flag
*                   0: Transmission is ready
*                   1: Transmission is busy
*                sci_state_t.bit.b_rx_busy - Reception busy flag
*                   0: Reception is ready
*                   1: Reception is busy
*                sci_state_t.bit.b_rx_orer - Overrun error flag
*                   0: No error
*                   1: Overrun error
*                sci_state_t.bit.b_rx_fer  - Framing error flag
*                   0: No error
*                   1: Framing error
*******************************************************************************/
sci_state_t SCI_GetState (void)
{

    return (state);

}

/*******************************************************************************
* Outline      : Transmit buffer empty interrupt
* Header       : none
* Function Name: sci_txi_isr
* Description  : Called in TXI interrupt processing. Write transmit data.
*                When transmit the last data, Disable TXI and enable TEI interrupt request.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_txi_isr (void)
{

    /* ==== Write transmit data to TDR register ==== */
    SCIn.TDR = *pbuf_tx;

    if (0 == SCIn.SSR.BIT.TEND)
    {
        /* Dummy read the SSR register */
    }

    pbuf_tx++;
    tx_cnt--;

    /* ==== When transmit the last data ==== */
    if (0 == tx_cnt)
    {
        /* ---- Disable TXI interrupt request ---- */
        IEN_SCIn_TXIn    = 0;
        SCIn.SCR.BIT.TIE = 0;

        while (0 != SCIn.SCR.BIT.TIE)
        {
            /* Confirm that bit is actually 0 */
        }

        /* TXI interrupt request is cleared */
        IR_SCIn_TXIn = 0;

        /* ---- Enable TEI interrupt request ---- */
        SCIn.SCR.BIT.TEIE = 1;
        IEN_SCIn_TEIn     = 1;
    }

}

/*******************************************************************************
* Outline      : Transmit end interrupt
* Header       : none
* Function Name: sci_tei_isr
* Description  : Called in TEI interrupt processing. Disable serial transmission.
*                Call the Callback function (SCI transmission end).
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_tei_isr (void)
{

    /* ---- Use pin as general I/O port ---- */
    TXDn_PMR = 0;

    /* ---- Disable serial transmission ---- */

    /* Serial transmission is disabled */
    SCIn.SCR.BIT.TE = 0;

    /* A TXI interrupt request is disabled */
    SCIn.SCR.BIT.TIE = 0;

    /* ---- Disable TEI interrupt request ---- */
    IEN_SCIn_TEIn     = 0;
    SCIn.SCR.BIT.TEIE = 0;

    while (0 != SCIn.SCR.BIT.TEIE)
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Clear the transmission busy flag ---- */
    B_TX_BUSY = 0;

    /* ---- Callback function ---- */
    pcb_sci_tx_end();

}

/*******************************************************************************
* Outline      : Receive data full interrupt
* Header       : none
* Function Name: sci_rxi_isr
* Description  : Called in RXI interrupt processing. Store the received data.
*                When received the last data, disable serial reception and
*                Call the Callback function (SCI reception end).
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_rxi_isr (void)
{

    /* ==== Store the received data ==== */
    *pbuf_rx = SCIn.RDR;
    pbuf_rx++;
    rx_cnt--;

    /* ==== When received the last data ==== */
    if (0 == rx_cnt)
    {
        /* ---- Disable serial reception ---- */
        SCIn.SCR.BIT.RE = 0;

        while (0 != SCIn.SCR.BIT.RE)
        {
            /* Confirm that bit is actually 0 */
        }

        /* ---- Disable ERI and RXI interrupt requests ---- */
        IEN_SCIn_RXIn    = 0;
        IEN_SCIn_ERIn    = 0;
        SCIn.SCR.BIT.RIE = 0;

        while (0 != SCIn.SCR.BIT.RIE)
        {
            /* Confirm that bit is actually 0 */
        }

        /* RXI interrupt request is cleared */
        IR_SCIn_RXIn = 0;

        /* ---- Clear the reception busy flag ---- */
        B_RX_BUSY = 0;

        /* ---- Callback function ---- */
        pcb_sci_rx_end();
    }

}

/*******************************************************************************
* Outline      : Receive error interrupt
* Header       : none
* Function Name: sci_eri_isr
* Description  : Called in ERI interrupt processing. Disable serial reception.
*                Call the Callback function (SCI receive error).
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_eri_isr (void)
{

    volatile uint8_t dummy;

    /* ==== Error processing ==== */

    /* ---- Overrun error ---- */
    if (1 == SCIn.SSR.BIT.ORER)
    {
        B_RX_ORER = 1;
    }

    /* ---- Framing error ---- */
    if (1 == SCIn.SSR.BIT.FER)
    {
        B_RX_FER = 1;
    }

    /* Dummy-read the RDR register */
    dummy = SCIn.RDR;

    /* ---- Disable serial reception ---- */
    SCIn.SCR.BIT.RE = 0;

    while (0 != SCIn.SCR.BIT.RE)
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Clear the error flag ---- */
    SCIn.SSR.BYTE = (SCIn.SSR.BYTE & ~SSR_ERROR_FLAGS) | 0xC0;

    while (0x00 != (SCIn.SSR.BYTE & SSR_ERROR_FLAGS))
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Disable ERI and RXI interrupt requests ---- */
    IEN_SCIn_ERIn    = 0;
    IEN_SCIn_RXIn    = 0;
    SCIn.SCR.BIT.RIE = 0;

    while (0 != SCIn.SCR.BIT.RIE)
    {
        /* Confirm that bit is actually 0 */
    }

    /* RXI interrupt request is cleared */
    IR_SCIn_RXIn = 0;

    /* ---- Clear the reception busy flag ---- */
    B_RX_BUSY = 0;

    /* ---- Callback function ---- */
    pcb_sci_rx_error();

}

/*******************************************************************************
* Outline      : SCIn.ERIn interrupt processing (Level interrupt)
* Header       : none
* Function Name: Excep_SCIn_ERIn
* Description  : Perform SCIn.ERIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_ERIn (void)
{

    do
    {
        /* Confirm interrupt request source */
        while ((0 != SCIn.SCR.BIT.RIE) && (0x00 != (SCIn.SSR.BYTE & SSR_ERROR_FLAGS)))
        {
            /* Clear interrupt request source and Perform interrupt request source processing */
            sci_eri_isr();
        }
    }
    /* Confirm IR flag */
    while (0 != IR_SCIn_ERIn);

}

/*******************************************************************************
* Outline      : SCIn.RXIn interrupt processing (Edge interrupt)
* Header       : none
* Function Name: Excep_SCIn_RXIn
* Description  : Perform SCIn.RXIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_RXIn (void)
{

    sci_rxi_isr();

}

/*******************************************************************************
* Outline      : SCIn.TXIn interrupt processing (Edge interrupt)
* Header       : none
* Function Name: Excep_SCIn_TXIn
* Description  : Perform SCIn.TXIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_TXIn (void)
{

    sci_txi_isr();

}

/*******************************************************************************
* Outline      : SCIn.TEIn interrupt processing (Level interrupt)
* Header       : none
* Function Name: Excep_SCIn_TEIn
* Description  : Perform SCIn.TEIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_TEIn (void)
{

    do
    {
        /* Confirm interrupt request source */
        while ((0 != SCIn.SCR.BIT.TEIE) && (0 != SCIn.SSR.BIT.TEND))
        {
            /* Clear interrupt request source and Perform interrupt request source processing */
            sci_tei_isr();
        }
    }
    /* Confirm IR flag */
    while (0 != IR_SCIn_TEIn);

}

