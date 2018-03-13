SRC += ./restful_server.c 
SRC += mongoose/mongoose.c
INC += -I./ -Imongoose
CFLAGS +=  -g -W -Wall -I../..  -I./ -Wno-unused-function -Wunused-parameter
CFLAGS += ${INC} 
LIBS += -lpthread
CC = gcc
	
restful_server:${SRC}
	${CC}  ${CFLAGS} $^ -o $@ ${LIBS}
.PHONY:clean
clean:
	rm restful_server -rf
