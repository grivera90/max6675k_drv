/*
 * max6675k.h
 *
 *  Created on: 20 feb. 2022
 *      Author: Gonzalo
 */

#ifndef COMPONENTS_MAX6675K_INCLUDE_MAX6675K_H_
#define COMPONENTS_MAX6675K_INCLUDE_MAX6675K_H_

#include <stdio.h>
#include <stdint.h>
/******************************************************************************
        Constants
 ******************************************************************************/
#define OK	0
#define ERR (-1)


/******************************************************************************
        Data types
 ******************************************************************************/
typedef enum
{
	MAX6675K_OK = 0,
	MAX6675K_ERROR = -1,

}max6675k_sts_t;

typedef struct
{
	uint16_t 	temp_raw;
	float 		temperature;

	/* functions pointer */
	int 	(*spi_init)(void);
	int 	(*spi_read)(uint16_t *rx_data);

}max6675k_t;
/******************************************************************************
        Public function prototypes
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

int max6675k_readtemperature(max6675k_t *max6675k);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* COMPONENTS_MAX6675K_INCLUDE_MAX6675K_H_ */
