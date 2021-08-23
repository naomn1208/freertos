/********************************************************************** */
/*                                                                      */
/*  FILE        : Main.c                                   				*/
/*  DATE        :Tue, Oct 31, 2006                                      */
/*  DESCRIPTION :Main Program                                           */
/*  CPU TYPE    :                                                       */
/*                                                                      */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                     */
/*                                                                      */
/********************************************************************** */
//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

/* Hardware specific includes. */
#include "iodefine.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//#include "r_cg_sci.h"	/* (xx) rx210 v */
#include "QS_SCI.h"		/* (xx) rx210 v */
#include "user_ral.h"	/* (xx) rx210 v */

/* Priorities at which the tasks are created. */
#if 0
/* (xx) rx210 v */
#define configQUEUE_RECEIVE_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define	configQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#else
#define configQUEUE_RECEIVE_TASK_PRIORITY	( tskIDLE_PRIORITY + 3 )	/* prvQueue */
#define	configQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )	/* prvQueue */

#define configQUEUE_SIO6_TASK1_PRIORITY		( tskIDLE_PRIORITY + 1 )	/* add */
#define configQUEUE_SIO6_TASK2_PRIORITY		( tskIDLE_PRIORITY + 2 )	/* add */
#define configQUEUE_SIO6_TASK3_PRIORITY		( tskIDLE_PRIORITY + 3 )	/* add */
#endif

/* The rate at which data is sent to the queue, specified in milliseconds. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 500 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.
 * This is 1 as the receive task will remove items
 * as they are added so the send task should always find the queue empty. */
#define mainQUEUE_LENGTH					( 1 )

/*
 * The tasks as defined at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* This variable is not used by this simple Blinky example.
 * It is defined purely to allow the project to link
 * as it is used by the full build configuration. */
volatile unsigned long ulHighFrequencyTickCount = 0UL;

/* --- vTask1 --- */
/* **** Buffer **** */
#define TX_BUF_SIZE     (18)		//(24)	/* Buffer size */
#define RX_BUF_SIZE     (18)		//(12)	/* Buffer size */
#define MSG_BUF_SIZE    (20)		//(12)	/* Buffer size */
#define RX_BUF_RET_NUM	(18)		//(10)	/* Buffer get size */
#define NULL_SIZE       (1)                 /* Null code size */


/* **** SCI state **** */
static sci_state_t      sci_state = {0x00};
#define SCI_B_TX_BUSY   sci_state.bit.b_tx_busy     /* Transmission busy flag */
#define SCI_B_RX_BUSY   sci_state.bit.b_rx_busy     /* Reception busy flag */
#define SCI_B_RX_ORER   sci_state.bit.b_rx_orer     /* Overrun error flag */
#define SCI_B_RX_FER    sci_state.bit.b_rx_fer      /* Framing error flag */


//static uint8_t tx_buf[] = "Hello world!\r\n";
//static uint8_t rx_buf[BUF_SIZE];

// ------------------------ //
// --- value for task 1 --- //
// ------------------------ //
// start of value for task 1 <<
uint8_t g_result;
char g_tx_buf[TX_BUF_SIZE];
//uint8_t g_uc_tx_buf[TX_BUF_SIZE]	="0123456789012345678901\r\n";
//uint8_t g_uc_tx_buf[TX_BUF_SIZE]	="AT01234567------------\r\n";
uint8_t g_uc_tx_buf[TX_BUF_SIZE];	/* receive buffer*/
uint8_t g_uc_rx_buf[RX_BUF_SIZE];	/* send buffer */
uint8_t g_uc_msg_buf[MSG_BUF_SIZE];	/* message buffer */

uint8_t g_uc_start_buf0[] =	"/* ******************************************************\r\n";
uint8_t g_uc_start_buf1[] =	" * FreeRTOS Kernel V10.4.1\r\n";
uint8_t g_uc_start_buf2[] =	" * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.\r\n";
uint8_t g_uc_start_buf3[] =	" *                                   All Rights Reserved.\r\n";
uint8_t g_uc_start_buf4[] =	" * http://www.FreeRTOS.org\r\n";
uint8_t g_uc_start_buf5[] =	" * http://aws.amazon.com/freertos\r\n";
uint8_t g_uc_start_buf6[] =	" * \r\n";
uint8_t g_uc_start_buf7[] =	" * Renesas Electronics Corporation.\r\n";
uint8_t g_uc_start_buf8[] =	" * 		       RX210 R5F5218ADFP\r\n";
uint8_t g_uc_start_buf9[] =	" * *************************************************** */\r\n";


int g_rx_cnt = 0;
int g_cnt = 0;
int g_i = 0;
int g_tx_index;
// >> end of value for task 1
GPIO_DATA_DEF_TYPE RalGpio;
SEQ_MAIN_STATUS_DEF_TYPE RalMainCore;
SEQ_SUB_STATUS_DEF_TYPE RalSubCore1;
SEQ_SUB_STATUS_DEF_TYPE RalSubCore2;

/*
 	 [AT][FUNC][DAT0][DAT1][DAT2][DAT3][DAT4][BCC][CRLF]
 	 []=2byte
 	 total=18byte
*/
void msg_banner(void);

void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);

static void cb_sci_tx_end (void);
static void cb_sci_rx_end (void);
static void cb_sci_rx_error (void);
/* --- vTask1 --- */

/*-----------------------------------------------------------*/

void main(void)
{
extern void HardwareSetup( void );

	/* Renesas provided CPU configuration routine.
	 * The clocks are configured in here. */
	HardwareSetup();

	/* Turn all LEDs off. */
	vParTestInitialise();

	RAL_InitVar(8, RalGpio.XData);		/* (xx) */
	RAL_InitVar(8, RalGpio.YData);		/* (xx) */
	RAL_MainCore_Init(&RalMainCore);	/* (xx) */
	RAL_SubCore_Init(&RalSubCore1);		/* (xx) */
	RAL_SubCore_Init(&RalSubCore2);		/* (xx) */

	SCI_Init();							/* (xx) */

	msg_banner();						/* (XX) */

	xTaskCreate(vTask1,"Task1", configMINIMAL_STACK_SIZE, NULL, configQUEUE_SIO6_TASK1_PRIORITY, NULL);
	xTaskCreate(vTask2,"Task2", configMINIMAL_STACK_SIZE, NULL, configQUEUE_SIO6_TASK2_PRIORITY, NULL);
	xTaskCreate(vTask3,"Task3", configMINIMAL_STACK_SIZE, NULL, configQUEUE_SIO6_TASK3_PRIORITY, NULL);


//	vStartComTestStringsTasks( configQUEUE_SIO6_TASK_PRIORITY, 38400, 4 );		/* (xx) rx210 v ; add */

#if 1
	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( unsigned long ) );

	if( xQueue != NULL )
	{
		/* Start the two tasks as described at the top of this file. */
		xTaskCreate( prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE, NULL, configQUEUE_RECEIVE_TASK_PRIORITY, NULL );
		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, configQUEUE_SEND_TASK_PRIORITY, NULL );

		/* Start the tasks running. */
		vTaskStartScheduler();
	}
#endif

//	vTaskStartScheduler();	/* (xx) */

	/* If all is well the next line of code will not be reached as the scheduler will be running.
	 * If the next line is reached then it is likely that there was insufficient heap available
	 * for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 100UL;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
		The block state is specified in ticks, the constant used converts ticks
		to ms. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to flash its LED.
		 * 0 is used so the send does not block
		 *  - it shouldn't need to as the queue should always be empty here
		 *  (it should always be empty because the task removing items
		 *  from the queue has a higher priority than the task adding
		things to the queue). */
		xQueueSend( xQueue, &ulValueToSend, 0UL );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{

#if 0
unsigned long ulReceivedValue;

	for( ;; )
	{
		/* Wait until something arives in the queue
		 *  - this will block indefinitely provided INCLUDE_vTaskSuspend
		 *  is set to 1 in FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have arrived,
		 * but is it the expected value?
		 * If it is, toggle the LED. */
		if( ulReceivedValue == 100UL )
		{
			vParTestToggleLED( 0 );
		}
	}

#else
/* (xx) rx210 v */
	unsigned long ulReceivedValue;

		for( ;; )
		{
			/* Wait until something arives in the queue
			 *  - this will block indefinitely provided INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h. */
			xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

			/*  To get here something must have arrived,
			 * but is it the expected value?
			 * If it is, toggle the LED. */
			if( ulReceivedValue == 100UL )
			{
				vParTestToggleLED_Four( 0 );
			}
		}

#endif

}
/*-----------------------------------------------------------*/

/* A callback function named vApplicationSetupTimerInterrupt() must be defined to configure a tick interrupt source,
 *  and configTICK_VECTOR set in FreeRTOSConfig.h to install the tick interrupt handler
 *  in the correct position in the vector table.
 *  This example uses a compare match timer.
 *  It can be into any FreeRTOS project, provided the same compare match timer is available. */
void vApplicationSetupTimerInterrupt( void )
{
	/* Enable compare match timer 0. */
	MSTP( CMT0 ) = 0;

	/* Interrupt on compare match. */
	CMT0.CMCR.BIT.CMIE = 1;

	/* Set the compare match value. */
	CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ ) -1 ) / 8 );

	/* Divide the PCLK by 8. */
	CMT0.CMCR.BIT.CKS = 0;

	/* Enable the interrupt... */
	_IEN( _CMT0_CMI0 ) = 1;

	/* ...and set its priority to the application defined kernel priority. */
	_IPR( _CMT0_CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;

	/* Start the timer. */
	CMT.CMSTR0.BIT.STR0 = 1;
}
/*-----------------------------------------------------------*/

/* If configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h,
 * then this function will be called if pvPortMalloc() returns NULL
 * because it has exhausted the available FreeRTOS heap space.
 * See http://www.freertos.org/a00111.html. */
void vApplicationMallocFailedHook( void )
{
	for( ;; );
}
/*-----------------------------------------------------------*/

/* If configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2 in FreeRTOSConfig.h,
 * then this function will be called if a task overflows its stack space.
 * See http://www.freertos.org/Stacks-and-stack-overflow-checking.html. */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	for( ;; );
}
/*-----------------------------------------------------------*/

/* If configUSE_IDLE_HOOK is set to 1 in FreeRTOSConfig.h,
 * then this function will be called on each iteration of the idle task.
 * See http://www.freertos.org/a00016.html */
void vApplicationIdleHook( void )
{
	/* Just to prevent the variable getting optimised away. */
	( void ) ulHighFrequencyTickCount;
}

/*-----------------------------------------------------------*/

void msg_banner(void){

	do {	g_result = SCI_StartTransmit(g_uc_start_buf0, sizeof(g_uc_start_buf0), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf1, sizeof(g_uc_start_buf1), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf2, sizeof(g_uc_start_buf2), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf3, sizeof(g_uc_start_buf3), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf4, sizeof(g_uc_start_buf4), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf5, sizeof(g_uc_start_buf5), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf6, sizeof(g_uc_start_buf6), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf7, sizeof(g_uc_start_buf7), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf8, sizeof(g_uc_start_buf8), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

	do {	g_result = SCI_StartTransmit(g_uc_start_buf9, sizeof(g_uc_start_buf9), cb_sci_tx_end);
	} while (SCI_BUSY == g_result);

}

/* -----------------------------------------------------------
 * 	vTask1
 * ----------------------------------------------------------- */
void vTask1(void *pvParameters)
{
	uint8_t s_rx_cnt;
	uint8_t s_rx_c[2];
	int i;
	int s_msg_num = 0;
	int s_msg_complete = 0;
	int s_msg_err = 0;

	g_uc_tx_buf[0]	=	'A';
	g_uc_tx_buf[1]	=	'T';

	g_uc_tx_buf[14]	=	'-';
	g_uc_tx_buf[15]	=	'-';

	g_uc_tx_buf[16]	=	0x0d;
	g_uc_tx_buf[17]	=	0x0a;

	for( i=0 ; i<MSG_BUF_SIZE ; i++ ){
		g_uc_msg_buf[i] = 0x0;
	}

	s_rx_c[0] = 0x0;
	s_rx_c[1] = 0x0;
    while(1) {

//		vTaskDelay(100/portTICK_PERIOD_MS);

		PORT5.PODR.BIT.B0 = 0x01;

		/*
			g_uc_tx_buf[ 0.. 1] ( 2)	="AT"
			g_uc_tx_buf[ 2.. 3] ( 2)	="(FUNC)"
			g_uc_tx_buf[ 4..11] ( 8)	="12345678"
			g_uc_tx_buf[12..13] ( 2)	="OK"or"ER"
			g_uc_tx_buf[14..15] ( 2)	="--"
			g_uc_tx_buf[16..17] ( 2)	="\r\n"
		*/

		/* --- receive --- */
		do
	    {
	        g_result = SCI_StartReceive(g_uc_rx_buf, 1, cb_sci_rx_end, cb_sci_rx_error);
	    }
	    while (SCI_BUSY == g_result);

		s_rx_cnt = SCI_GettReceiveNum();
		s_msg_complete = 0;
		s_msg_err = 0;

		for( i=0 ; i<s_rx_cnt ; i++ ){
			s_rx_c[0] = s_rx_c[1];
			s_rx_c[1] = g_uc_rx_buf[i];

			// check start code
			if(s_msg_num<1){
				if( (s_rx_c[0]=='A') && (s_rx_c[1]=='T') ){
					g_uc_msg_buf[0] = 'A';
					g_uc_msg_buf[1] = 'T';
					s_msg_num = 2;
				}
			}

			// start ok
			else{
				g_uc_msg_buf[s_msg_num] = g_uc_rx_buf[i];
				// check end code
				if( (s_rx_c[0]=='\r') && (s_rx_c[1]=='\n') ){
					s_msg_complete = 1;
				}
				s_msg_num++;

				// over length
				if( s_msg_num > 18 ){
					s_msg_err = 1;
					s_msg_complete = 1;
				}
			}

			if(s_msg_complete == 1){
				s_msg_num = 0;
				break;
			}

		}

		/* --- message --- */
		if(s_msg_complete == 1){
			if(s_msg_err==1){
				g_uc_tx_buf[2]='E';
				g_uc_tx_buf[3]='R';
				memcpy(&g_uc_tx_buf[4],"OVER0000ER",10);

			}
			else{

				// version
				//  ATVR01000000OK\r\n
				if( (g_uc_msg_buf[2]=='V') && (g_uc_msg_buf[3]=='R') )
				{
					g_uc_tx_buf[2]='V';
					g_uc_tx_buf[3]='R';
					memcpy(&g_uc_tx_buf[4],"01000000OK",10);
				}
				// X port get
				//  ATXN--------OK\r\n
				else if( (g_uc_msg_buf[2]=='X') && (g_uc_msg_buf[3]=='N') )
				{
					RAL_GetYdat(&g_uc_msg_buf[4], RalGpio.XData, &g_uc_tx_buf[4]);
					g_uc_tx_buf[2]='X';
					g_uc_tx_buf[3]='N';
					g_uc_tx_buf[12]='O';
					g_uc_tx_buf[13]='K';
				}
				// Y port get
				//  ATYN--------OK\r\n
				else if( (g_uc_msg_buf[2]=='Y') && (g_uc_msg_buf[3]=='N') )
				{
					RAL_GetYdat(&g_uc_msg_buf[4], RalGpio.YData, &g_uc_tx_buf[4]);
					g_uc_tx_buf[2]='Y';
					g_uc_tx_buf[3]='N';
					g_uc_tx_buf[12]='O';
					g_uc_tx_buf[13]='K';
				}
				// SY port set
				//  ATSY--------OK\r\n
				else if( (g_uc_msg_buf[2]=='S') && (g_uc_msg_buf[3]=='Y') )
				{
					RAL_SetYdat(&g_uc_msg_buf[4], RalGpio.YData);
					g_uc_tx_buf[2]='S';
					g_uc_tx_buf[3]='Y';
					memcpy(&g_uc_tx_buf[4], &g_uc_msg_buf[4],8);
					g_uc_tx_buf[12]='O';
					g_uc_tx_buf[13]='K';
				}
				// error
				//  ATER00000000ER\r\n
				else{
					g_uc_tx_buf[2]='E';
					g_uc_tx_buf[3]='R';
					memcpy(&g_uc_tx_buf[4],"00000000ER",10);
				}

			}

//			PORT5.PODR.BIT.B0 = 0x00;

			/* --- transmit --- */
			do
			{
				g_result = SCI_StartTransmit(g_uc_tx_buf, sizeof(g_uc_tx_buf), cb_sci_tx_end);
			}
			while (SCI_BUSY == g_result);

		}
    }
}

/*******************************************************************************
* Outline      : Callback function (SCI transmission end)
* Header       : sci.h
* Function Name: cb_sci_tx_end
* Description  : This function to be called when transmission has done.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void cb_sci_tx_end (void)
{

    /* LED0 ON (SCI transmission end) */
//    LED0_REG_PODR = LED_ON;

}

/*******************************************************************************
* Outline      : Callback function (SCI reception end)
* Header       : sci.h
* Function Name: cb_sci_rx_end
* Description  : This function to be called when reception has done.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void cb_sci_rx_end (void)
{

    /* LED1 ON (SCI reception end) */
//    LED1_REG_PODR = LED_ON;

}

/*******************************************************************************
* Outline      : Callback function (SCI receive error)
* Header       : sci.h
* Function Name: cb_sci_rx_error
* Description  : This function to be called when reception error occurred.
*                This sampling code does not include error process.
*                Add the program when it needed.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void cb_sci_rx_error (void)
{

    /* LED2 ON (SCI receive error) */
//    LED2_REG_PODR = LED_ON;

    /* ==== Get SCI state ==== */
    sci_state = SCI_GetState();

    /* ---- Overrun error ---- */
    if (0 != SCI_B_RX_ORER)
    {
        /* This sampling code does not include any process. */
        /* Add the program when it needed. */
    }

    /* ---- Framing error ---- */
    if (0 != SCI_B_RX_FER)
    {
        /* This sampling code does not include any process. */
        /* Add the program when it needed. */
    }

}

/* -----------------------------------------------------------
 * 	vTask2
 * ----------------------------------------------------------- */
void vTask2(void *pvParameters)
{
	while(1) {
		PORT5.PODR.BIT.B4 = ~PORT5.PODR.BIT.B4;
		vTaskDelay(200/portTICK_PERIOD_MS);
	}
}

/* -----------------------------------------------------------
 * 	vTask3
 * ----------------------------------------------------------- */
void vTask3(void *pvParameters)
{

	while(1) {
		PORT5.PODR.BIT.B5 = ~PORT5.PODR.BIT.B5;
		vTaskDelay(300/portTICK_PERIOD_MS);
	}
}


#ifdef __cplusplus
void abort(void)
{

}
#endif
