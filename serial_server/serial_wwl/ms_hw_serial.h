#ifndef __MS_HW_SERIAL__
#define __MS_HW_SERIAL__

/*
 * Copyright (c) 2015 - 2016 M-SMART Research Institute of Midea Group.
 * All rights reserved.
 *
 * File Name 		: 
 * Introduction	: 
 *
 * Current Version: 
 * Author			: Gilbert <jianzhi.feng@midea.com.cn>
 * Create Time	: 2016/04/18
 * Change Log		: create this file
 *
 * All software, firmware and related documentation here in ("M-Smart Software") are
 * intellectual property of M-SMART Research Institute of Midea Group and protected 
 * by law, including, but not limited to, copyright law and international treaties.
 *
 * Any use, modification, reproduction, retransmission, or republication of all
 * or part of M-Smart Software is expressly prohibited, unless prior written
 * permission has been granted by M-Smart.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>

 
 typedef struct TTY_INFO
 {
	 int fd;//serial device ID
 
	 pthread_mutex_t mt;//mutex
 
	 char name[24]; //serial device name£¬such as£º"/dev/ttyS0"
 
	 struct termios ntm; //new serial device cofigure
 
	 struct termios otm; //old serial device configure
 } tty_info_t;

tty_info_t *ms_hal_hw_uart_ready(char * id);
int ms_hal_hw_uart_clean(tty_info_t *ptty);
int ms_hal_hw_uart_set_speed(tty_info_t *ptty, int speed,int hardware);
int ms_hal_hw_uart_set_parity(tty_info_t *ptty,int databits,int parity,int stopbits);
int ms_hal_hw_uart_lock(tty_info_t*ptty);
int ms_hal_hw_uart_unlock(tty_info_t *ptty);
int ms_hal_hw_uart_read(tty_info_t *ptty,unsigned char *pbuf,int size, unsigned int timeout_ms);
int ms_hal_hw_uart_write(tty_info_t *ptty,unsigned char *pbuf,int size, unsigned int timeout_ms);



#endif
