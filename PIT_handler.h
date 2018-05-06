/*
 * PIT_handler.h
 *
 *  Created on: May 4, 2018
 *      Author: joab
 */

#ifndef PIT_HANDLER_H_
#define PIT_HANDLER_H_

#include "fsl_pit.h"

#define SAMPLE_PERIOD_US 22

void initPIT(void);

#endif /* PIT_HANDLER_H_ */
