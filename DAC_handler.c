/*
 * DAC_handler.c
 *
 *  Created on: May 4, 2018
 *      Author: joab
 */


#include "DAC_handler.h"

void DAC_init(void)
{
    dac_config_t dacConfigStruct;

	DAC_GetDefaultConfig(&dacConfigStruct);
	DAC_Init(DAC0, &dacConfigStruct);
	DAC_Enable(DAC0, true); /* Enable output. */
	DAC_SetBufferReadPointer(DAC0, 0U); /* Make sure the read pointer to the start. */
	DAC_SetBufferValue(DAC0, 0U, 2048);
}
