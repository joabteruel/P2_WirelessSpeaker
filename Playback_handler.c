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

SemaphoreHandle_t sampleNow_sem;

TaskHandle_t samplerTask_handle;

uint16_t buffer_A[BUFFER_SIZE];
uint16_t buffer_B[BUFFER_SIZE];

bool flag=false;


void os_init(void)
{
	recvBuffptrQueue = xQueueCreate(2, sizeof(UDP_recvBuff*));
	bufferStatus_event = xEventGroupCreate();
	interruptStatus_event = xEventGroupCreate();
	sampleNow_sem = xSemaphoreCreateBinary();
}

void SampleInterrupt()
{
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
	xSemaphoreGiveFromISR(sampleNow_sem,pdFALSE);
	portYIELD_FROM_ISR(pdTRUE);
}

void UDP_receive(void *arg)
{
	struct netconn *conn;
	struct netbuf *buf;

	EventBits_t buffStatus_event;

	LWIP_UNUSED_ARG(arg);
	conn = netconn_new(NETCONN_UDP);
	netconn_bind(conn, IP_ADDR_ANY, UDP_PORT);

	xEventGroupSetBits(bufferStatus_event, BUFFER_A_SAMPLING);

	while (1)
	{

		buffStatus_event = xEventGroupGetBits(bufferStatus_event);
		if(((buffStatus_event & BUFFER_FULL)) != BUFFER_FULL)
		{
			netconn_recv(conn, &buf);

			/*If buffer B is being sampled, fill buffer A with data*/
			if((buffStatus_event & BUFFER_A_SAMPLING) == BUFFER_A_SAMPLING)
			{
				xEventGroupClearBits(bufferStatus_event, BUFFER_A_SAMPLING);
				netbuf_copy(buf, buffer_A, sizeof(buffer_A));
				netbuf_delete(buf);
			}
			/*Otherwise buffer A is being sampled, so fill buffer B with data*/
			else
			{
				xEventGroupSetBits(bufferStatus_event, BUFFER_A_SAMPLING);
				netbuf_copy(buf, buffer_B, sizeof(buffer_B));
				netbuf_delete(buf);
			}
			xEventGroupSetBits(bufferStatus_event, BUFFER_FULL);
		}

	}
}

void sample_Playback(void * params)
{
	samplerTask_handle = xTaskGetCurrentTaskHandle();

	EventBits_t buffStatus_event;

	uint8_t actualSampleCount = 0;
    PIT_StartTimer(PIT, kPIT_Chnl_0);

	while(1)
	{
		/*The sampling signal semaphore is now available*/
		if(pdTRUE == xSemaphoreTake(sampleNow_sem, portMAX_DELAY))
		{
			buffStatus_event = xEventGroupGetBits(bufferStatus_event);
			if((BUFFER_FULL & buffStatus_event) == BUFFER_FULL)
			{
				/*If current sampling buffer is A buffer keep sampling A buffer*/
				if((buffStatus_event & BUFFER_A_SAMPLING) != BUFFER_A_SAMPLING)
				{
					//PRINTF("Buffer A[%d] -> %d\r\n",actualSampleCount, buffer_A[actualSampleCount]);
					DAC_SetBufferValue(DAC0, 0U, ((buffer_A[actualSampleCount]+32768)>>4));
					buffer_A[actualSampleCount] = 0;
				}
				/*Otherwise is buffer B the one which is currently being samplingr*/
				else
				{
					//PRINTF("Buffer B[%d] -> %d\r\n", actualSampleCount, buffer_B[actualSampleCount]);
					DAC_SetBufferValue(DAC0, 0U, ((buffer_B[actualSampleCount]+32768)>>4));
					buffer_A[actualSampleCount] = 0;
				}

				if(BUFFER_SIZE > actualSampleCount)
				{
					actualSampleCount++;
				}else
				{
					actualSampleCount = 0;
					xEventGroupClearBits(bufferStatus_event, BUFFER_FULL);
				}
			}
		}
	}
}
