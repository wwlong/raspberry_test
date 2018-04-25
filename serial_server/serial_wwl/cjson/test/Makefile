CC=g++
#CPPFLAGS += -g

SRC += ../../cjson/jsoncpp.cpp 
SRC += ./test.cpp 

INC += -I../../cjson
INC += -I.

CPPFLAGS += ${INC}
CPPFLAGS += -g -std=c++11
all:test
test:${SRC}
	${CC}  $^ ${CPPFLAGS} -o  $@
.Phony:clean
clean:
	rm *.o *.~ test -rf
