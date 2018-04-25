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
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include "ms_hw_serial.h"
#include <sys/time.h>
#define MS_MODULE UART_HW
#ifndef NDEBUG
unsigned char dbgUART_HW = 1;
#endif

/*-------------------------------------
  init and open the serial device
---------------------------------------*/
tty_info_t *ms_hal_hw_uart_ready(char *id)
{
	tty_info_t *ptty;

	ptty = (tty_info_t *)malloc(sizeof(tty_info_t));
	if(ptty == NULL)
		return NULL;
	memset(ptty,0,sizeof(tty_info_t));
	memset(&ptty->ntm, 0,sizeof(ptty->ntm));

	pthread_mutex_init(&ptty->mt,NULL);
	sprintf(ptty->name,"/dev/%s",id);
    //sprintf(ptty->name,"/dev/ttyUSB%d",id);

	//open and set the uart
	ptty->fd = open(ptty->name, O_RDWR | O_NOCTTY  | O_NONBLOCK);//modify by gilbert
	if (ptty->fd <0)
	{
		printf("open %s error %s\n",ptty->name,strerror(errno));
		free(ptty);
		return NULL;
	}

	tcgetattr(ptty->fd,&ptty->otm);
	return ptty;

}


/*-------------------------------------
   clean and close the serial device
---------------------------------------*/
int ms_hal_hw_uart_clean(tty_info_t *ptty)
{
	//close the device
	if(ptty->fd>0)
	{
		tcsetattr(ptty->fd,TCSANOW,&ptty->otm);
		close(ptty->fd);
		ptty->fd = -1;
		free(ptty);
		ptty = NULL;
	}

	return 0;
}

/*-------------------------------------
      set the uart speed
---------------------------------------*/
int ms_hal_hw_uart_set_speed(tty_info_t *ptty, int speed,int hardware)
{
//	int i;

	tcgetattr(ptty->fd,&ptty->ntm);
	ptty->ntm.c_cflag = CLOCAL | CREAD;
	if(hardware)
		ptty->ntm.c_cflag |= CRTSCTS;

	switch(speed)
	{
		case 300:
			ptty->ntm.c_cflag |= B300;
			break;
		case 1200:
			ptty->ntm.c_cflag |= B1200;
			break;
		case 2400:
			ptty->ntm.c_cflag |= B2400;
			break;
		case 4800:
			ptty->ntm.c_cflag |= B4800;
			break;
		case 9600:
			ptty->ntm.c_cflag |= B9600;
			break;
		case 19200:
			ptty->ntm.c_cflag |= B19200;
			break;
		case 38400:
			ptty->ntm.c_cflag |= B38400;
			break;
		case 57600:
			ptty->ntm.c_cflag |= B38400;
			break;
		case 115200:
			ptty->ntm.c_cflag |= B115200;
			break;
		default:
			printf("Baudrate fail!current = %d \n",speed);
	}
	ptty->ntm.c_iflag = IGNPAR;
	ptty->ntm.c_oflag = 0;
	tcsetattr(ptty->fd,TCSANOW,&ptty->ntm);
	tcflush(ptty->fd, TCIFLUSH);

	return 0;
}

/*-------------------------------------
      set the tty parity
---------------------------------------*/
int ms_hal_hw_uart_set_parity(tty_info_t *ptty,int databits,int parity,int stopbits)
{

	//get the serial configure
	if( tcgetattr(ptty->fd,&ptty->ntm) != 0)
	{
		printf("SetupSerial [%s]\n",ptty->name);
		return 1;
	}

	ptty->ntm.c_cflag &= ~CSIZE;
	switch (databits)
	{ //set the data bits

	case 7:
		ptty->ntm.c_cflag |= CS7;
		break;
	case 8:
		ptty->ntm.c_cflag |= CS8;
		break;
	default:
		printf("Unsupported data size\n");
		return 5;
	}

	switch (parity)
	{ //set the parity

	case 'n':
	case 'N':
		ptty->ntm.c_cflag &= ~PARENB; /* Clear parity enable */
		ptty->ntm.c_iflag &= ~INPCK; /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		ptty->ntm.c_cflag |= (PARODD|PARENB); 
		ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		ptty->ntm.c_cflag |= PARENB; /* Enable parity */
		ptty->ntm.c_cflag &= ~PARODD; 
		ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
		break;
	case 'S':
	case 's': /*as no parity*/
		ptty->ntm.c_cflag &= ~PARENB;
		ptty->ntm.c_cflag &= ~CSTOPB;
		break;
	default:
		printf("Unsupported parity\n");
		return 2;
	}

	//set the stop bit
	switch (stopbits)
	{
	case 1:
		ptty->ntm.c_cflag &= ~CSTOPB;
		break;
	case 2:
		ptty->ntm.c_cflag |= CSTOPB;
		break;
	default:
		printf("Unsupported stop bits\n");
		return 3;
	}

	ptty->ntm.c_lflag = 0;
	ptty->ntm.c_cc[VTIME] = 0; // inter-character timer unused

	////ptty->ntm.c_cc[VMIN] = 1; // blocking read until 1 chars received
	ptty->ntm.c_cc[VMIN] = 0;//modify by gilbert

	if (tcsetattr(ptty->fd,TCSANOW,&ptty->ntm) != 0)
	{
		printf("SetupSerial \n");
		return 4;
	}
	tcflush(ptty->fd, TCIFLUSH);

	return 0;
}


int ms_hal_hw_uart_lock(tty_info_t*ptty)
{
	if(ptty->fd < 0)
	{
		return 1;
	}

	return flock(ptty->fd,LOCK_EX);
}


int ms_hal_hw_uart_unlock(tty_info_t *ptty)
{
	if(ptty->fd < 0)
	{
		return 1;
	}

	return flock(ptty->fd,LOCK_UN);
}


/*-------------------------------------
     receive the serial data
---------------------------------------*/
int ms_hal_hw_uart_read(tty_info_t *ptty,unsigned char *pbuf,int size, unsigned int timeout_ms)
{
    int ret,left;
    int retval = 0;
    fd_set fs_read;
    struct timeval time_out;

    if(NULL == ptty || NULL == pbuf || size <= 0) {
        printf("[%s] -- [%d] -- invalid params\r\n", __FUNCTION__, __LINE__);
        return 1;
    }
    FD_ZERO(&fs_read);
    FD_SET(ptty->fd, &fs_read);
    memset(&time_out, 0, sizeof(struct timeval));
    time_out.tv_sec = timeout_ms / 1000;
    time_out.tv_usec = (timeout_ms % 1000) * 1000;

    left = size;

    retval = select(ptty->fd + 1,&fs_read, NULL, NULL, &time_out);
    if(retval == 0) {
        //printf("[%s] -- [%d] -- select tiomeout\r\n", __FUNCTION__, __LINE__);
    }
    else if(retval < 0) {
        printf("[%s] -- [%d] -- select error\r\n", __FUNCTION__, __LINE__);
    }
    else {
        pthread_mutex_lock(&ptty->mt);
        while(left>0)
        {
            ret = 0;
            //		bytes = 0;
            //printf("-------------\n");
            //ioctl(ptty->fd, FIONREAD, &bytes);
            //if(bytes>0)
            {

                //printf("==============\n");
                ret = read(ptty->fd,pbuf,left);
                //printf("==================%d\n",ret);
            }
            //printf("+++++++++++++++\n");
            if(ret >0)
            {
                //printf("~~~~~~~~~~%d\n", ret);
                left -= ret;
                pbuf += ret;
            }
            else 
            {
                break;
            }
            usleep(5000);
        }
        pthread_mutex_unlock(&ptty->mt);

    }
    //printf("&");
    return size - left;
}

/*-------------------------------------
     send the serial data
---------------------------------------*/
int ms_hal_hw_uart_write(tty_info_t *ptty,unsigned char *pbuf,int size, unsigned int timeout_ms)
{
    int ret,nleft;
    unsigned char *ptmp;
    int retval = 0;
    fd_set fs_write;
    struct timeval time_out;

    ret = 0;
    nleft = size;
    ptmp = pbuf;

    FD_ZERO(&fs_write);
    FD_SET(ptty->fd, &fs_write);
    memset(&time_out, 0, sizeof(struct timeval));
    time_out.tv_sec = timeout_ms / 1000;
    time_out.tv_usec = (timeout_ms % 1000) * 1000;
    retval = select(ptty->fd + 1,NULL, &fs_write, NULL, &time_out);
    if(retval == 0) {
        //printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
    }
    else if(retval < 0) {
        printf("[%s] -- [%d] -- select error\r\n", __FUNCTION__, __LINE__);
    }
    else {
        while(nleft>0)
        {
            pthread_mutex_lock(&ptty->mt);
            ret = write(ptty->fd,ptmp,nleft);
            pthread_mutex_unlock(&ptty->mt);

            if(ret >0)
            {
                nleft -= ret;
                ptmp += ret;
            }
            //usleep(100);
        }

    }
    return size - nleft;
}


