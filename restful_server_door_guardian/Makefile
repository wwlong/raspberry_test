SRC += ./restful_server.c 
SRC += mongoose-6.11/mongoose.c
SRC += ../raspberry_gpio/raspberry_gpio_op_c_api/raspberry_gpio_op.c  
INC += -I../raspberry_gpio/raspberry_gpio_op_c_api
INC += -I./mongoose-6.11
CFLAGS +=  -g -W -Wall -I../..  -I./ -Wno-unused-function -Wunused-parameter
CFLAGS += ${INC} 
LIBS += -lpthread
CC = gcc
	
restful_server:${SRC}
	${CC}  ${CFLAGS} $^ -o $@ ${LIBS}
.PHONY:clean
clean:
	rm restful_server -rf
