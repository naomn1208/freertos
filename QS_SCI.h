/*
 * QS_SCI.h
 *
 *  Created on: 2019/09/21
 *      Author: user
 */

#ifndef QS_SCI_H_
#define QS_SCI_H_




/*******************************************************************************
Macro definitions
*******************************************************************************/
/* **** Return value for SCI_StartReceive, SCI_StartTransmit function **** */
#define SCI_OK      (0x00)      /* Transmission/Reception start */
#define SCI_BUSY    (0x01)      /* Transmission/Reception busy */
#define SCI_NG      (0x02)      /* Parameter error */

/*******************************************************************************
Typedef definitions
*******************************************************************************/
/* **** SCI state **** */
#pragma bit_order left
#pragma unpack
typedef union
{
    uint8_t byte;
    struct
    {
        uint8_t b_tx_busy:1;    /* Transmission busy flag   0: Transmission is ready    1: Transmission is busy */
        uint8_t b_rx_busy:1;    /* Reception busy flag      0: Reception is ready       1: Reception is busy */
        uint8_t b_rx_orer:1;    /* Overrun error flag       0: No error                 1: Overrun error */
        uint8_t b_rx_fer :1;    /* Framing error flag       0: No error                 1: Framing error */
        uint8_t          :4;    /* not use */
    } bit;
} sci_state_t;
#pragma packoption
#pragma bit_order

/* **** Callback function **** */
typedef void (* CallBackFunc)(void);

/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/
/* **** User I/F function **** */
void SCI_Init(void);
uint8_t     SCI_StartReceive(uint8_t * pbuf, uint8_t num, CallBackFunc pcb_rx_end, CallBackFunc pcb_rx_error);
uint8_t     SCI_StartTransmit(const uint8_t * pbuf, uint8_t num, CallBackFunc pcb_tx_end);
sci_state_t SCI_GetState(void);

uint8_t SCI_GettReceiveNum();


#endif /* QS_SCI_H_ */
