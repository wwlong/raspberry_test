CC = g++
RM = rm

SRCCPP += ./cjson/jsoncpp.cpp 
SRCC += ./raspberry_gpio/raspberry_gpio_op_c_api/raspberry_gpio_op.c 
SRCC += ./test_uart1.c 
SRCC += ./ms_hw_serial.c
INC += -I. 
INC += -I./cjson
INC += -I./raspberry_gpio/raspberry_gpio_op_c_api 

CFLAGS += -g 
CFLAGS += ${INC}
CPPFLAGS += -g -std=c++11 -pthread
LIBS += 
ALL:serial

OBJSC := $(patsubst %.c,%.o,$(SRCC)) 
OBJSCPP := $(patsubst %.cpp,%.o,$(SRCCPP))
OBJS += ${OBJSC} 
OBJS += ${OBJSCPP}

serial:${OBJS}
	${CC} ${CFLAGS} ${CPPFLAGS} $^ -o $@ ${LIBS} 

.PHONY:clean
clean:
	${RM} *~ ${ALL} -rf ${OBJS}
