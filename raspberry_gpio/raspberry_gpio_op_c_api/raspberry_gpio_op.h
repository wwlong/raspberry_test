//#pragma once
//
#ifndef __RASPBERRY_GPIO_OP_H_
#define __RASPBERRY_GPIO_OP_H_

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif 
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define POUT            17  
#define BUFFER_MAX      3
#define DIRECTION_MAX   48
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int GPIOExport(int pin);
int GPIOUnexport(int pin);
int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

#ifdef __cplusplus
}
#endif 


#endif 

