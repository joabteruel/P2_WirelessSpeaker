/*
 * PIT_handler.c
 *
 *  Created on: May 4, 2018
 *      Author: joab
 */

#include "PIT_handler.h"

void initPIT(void)
{
	pit_config_t pitConfig;
	PIT_GetDefaultConfig(&pitConfig);
	/* Init pit module */
	PIT_Init(PIT, &pitConfig);

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, USEC_TO_COUNT(SAMPLE_PERIOD_US, CLOCK_GetFreq(kCLOCK_BusClk)));
	NVIC_SetPriority(PIT0_IRQn, 5);
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    EnableIRQ(PIT0_IRQn);
}
