CC = gcc

SRC += ./raspberry_gpio_op.c
SRC += ./gpio_test.c

INC += -I./
CFLAGS += ${INC} -g -Wall -Werror -O
LIBS += 

all : gpio_test

gpio_test:${SRC}
	${CC} ${CFLAGS} $^ -o $@ ${LIBS}

.PHONY:clean
clean:
	rm *~ gpio_test -rf
