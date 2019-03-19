CC=xlc
ALT_LIB_PATH=../lib
ALT_BIN_PATH=../bin
ALT_INCLUDE_PATH=../include

ALT_LIB_DEPS=-locfg -lomq

all:	lib server engine

lib:	compilelib packlib

compilelib:
	xlc -I$(ALT_INCLUDE_PATH) -c cfgfunc.c -o $(ALT_LIB_PATH)/cfgfunc.o
	xlc -I$(ALT_INCLUDE_PATH) -c mqmgr.c -o $(ALT_LIB_PATH)/mqmgr.o

packlib:
	ar -rcs $(ALT_LIB_PATH)/libocfg.a $(ALT_LIB_PATH)/cfgfunc.o
	ar -rcs $(ALT_LIB_PATH)/libomq.a $(ALT_LIB_PATH)/mqmgr.o
	rm -f $(ALT_LIB_PATH)/*.o

server:
	xlc -I$(ALT_INCLUDE_PATH) server1.c -o $(ALT_BIN_PATH)/server1
	xlc -I$(ALT_INCLUDE_PATH) server2.c -o $(ALT_BIN_PATH)/server2

engine:
	$(CC) -I$(ALT_INCLUDE_PATH) -L$(ALT_LIB_PATH) $(ALT_LIB_DEPS) enginemgr.c -o $(ALT_BIN_PATH)/engine

run:
	$(ALT_BIN_PATH)/engine
