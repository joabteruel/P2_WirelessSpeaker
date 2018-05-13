/*
 * LWIP_handler.h
 *
 *  Created on: May 4, 2018
 *      Author: joab
 */

#ifndef PLAYBACK_HANDLER_H_
#define PLAYBACK_HANDLER_H_


#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "board.h"
#include "semphr.h"

#include "PIT_handler.h"

#define SampleInterrupt() PIT0_IRQHandler()

#define UDP_PORT 50005

#define BUFFER_SIZE 250

void os_init(void);
void UDP_receive(void *arg);
void sample_Playback(void * params);


typedef struct
{
	uint16_t len;
	uint16_t *msg;
}UDP_recvBuff;

typedef struct
{
	UDP_recvBuff recBuff_A;
	UDP_recvBuff recBuff_B;
}UDP_recvBuffArray;

typedef enum
{
	SAMPLE_INTERRUPT = 1<<0,
}InterruptStatusEvent;

typedef enum
{
	BUFFER_A_SAMPLING = 1<<0,
	BUFFER_B_SAMPLING = 1<<1,
	BUFFER_FULL = 1<<2,
}BufferStatusEvents;





#endif /* PLAYBACK_HANDLER_H_ */
