/*
 * LWIP_handler.c
 *
 *  Created on: May 4, 2018
 *      Author: joab
 */

#include <Playback_handler.h>

QueueHandle_t recvBuffptrQueue;

EventGroupHandle_t bufferStatus_event;
EventGroupHandle_t interruptStatus_event;

uint16_t buffer_A[BUFFER_SIZE];
uint16_t buffer_B[BUFFER_SIZE];


void os_init(void)
{
	recvBuffptrQueue = xQueueCreate(2, sizeof(UDP_recvBuff*));
	bufferStatus_event = xEventGroupCreate();
	interruptStatus_event = xEventGroupCreate();
}

void SampleInterrupt()
{
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	//PIT_StopTimer(PIT, kPIT_Chnl_0);
	xEventGroupSetBitsFromISR(interruptStatus_event, SAMPLE_INTERRUPT, pdFALSE);
	portYIELD_FROM_ISR(pdFALSE);
}

void UDP_receive(void *arg)
{
	struct netconn *conn;
	struct netbuf *buf;

	EventBits_t buffStatus_event;

	LWIP_UNUSED_ARG(arg);
	conn = netconn_new(NETCONN_UDP);
	netconn_bind(conn, IP_ADDR_ANY, UDP_PORT);

	xEventGroupSetBits(bufferStatus_event, BUFFER_B_SAMPLING);

	while (1)
	{
		netconn_recv(conn, &buf);

		buffStatus_event = xEventGroupGetBits(bufferStatus_event);
		/*If buffer B is being sampled, fill buffer A with data*/
		if(((buffStatus_event & BUFFER_B_SAMPLING)) == BUFFER_B_SAMPLING)
		{
			//netbuf_data(buf, (void**)&recBuff_A.msg, &recBuff_A.len);
			netbuf_copy(buf, buffer_A, sizeof(buffer_A));
			netbuf_delete(buf);
		}
		/*Otherwise buffer A is being sampled, so fill buffer B with data*/
		else
		{
			netbuf_copy(buf, buffer_B, sizeof(buffer_B));
			netbuf_delete(buf);
		}
		xEventGroupSetBits(bufferStatus_event, BUFFER_FULL);
	    PIT_StartTimer(PIT, kPIT_Chnl_0);
	}
}

void sample_Playback(void * params)
{
	EventBits_t buffStatus_event;
	TickType_t elapsed = 0;

	uint8_t actualSampleCount = 0;

	while(1)
	{
		xEventGroupWaitBits(interruptStatus_event, SAMPLE_INTERRUPT, pdTRUE, pdTRUE, portMAX_DELAY);

		buffStatus_event = xEventGroupGetBits(bufferStatus_event);

		/*If we've sampled the whole buffer*/
		if(BUFFER_SIZE == actualSampleCount)
		{
			actualSampleCount = 0;
			if ((buffStatus_event & BUFFER_FULL) == BUFFER_FULL)
			{
				/*If prev sampling buffer was A buffer swap for B buffer*/
				if((buffStatus_event & BUFFER_A_SAMPLING) == BUFFER_A_SAMPLING)
				{
					xEventGroupClearBits(bufferStatus_event, BUFFER_A_SAMPLING);
					xEventGroupSetBits(bufferStatus_event, BUFFER_B_SAMPLING);
				}
				/*Otherwise, swap B buffer by A buffer*/
				else
				{
					xEventGroupClearBits(bufferStatus_event, BUFFER_B_SAMPLING);
					xEventGroupSetBits(bufferStatus_event, BUFFER_A_SAMPLING);
				}
//				elapsed = xTaskGetTickCount() - elapsed;
//				PRINTF("Buffer Swap! at %d ticks", elapsed);
			}
		}

		buffStatus_event = xEventGroupGetBits(bufferStatus_event);

		/*If current sampling buffer is A buffer keep sampling A buffer*/
		if((buffStatus_event & BUFFER_A_SAMPLING) == BUFFER_A_SAMPLING)
		{
			//PRINTF("Buffer A[%d] -> %d\r\n",actualSampleCount, buffer_A[actualSampleCount]);
			DAC_SetBufferValue(DAC0, 0U, buffer_A[actualSampleCount]);
		}
		/*Otherwise is buffer B the one which is currently being samplingr*/
		if((buffStatus_event & BUFFER_B_SAMPLING) == BUFFER_B_SAMPLING)
		{
			//PRINTF("Buffer B[%d] -> %d\r\n", actualSampleCount, buffer_B[actualSampleCount]);
			DAC_SetBufferValue(DAC0, 0U, buffer_B[actualSampleCount]);
		}
		actualSampleCount++;
	}
}
