BOARD=stub

E_C = $(SRC_C:.c=.E) $(MAIN_SRC:.c=.o)
E_CXX = $(SRC_CXX:.cpp=.E) 
OBJ_C = $(SRC_C:.c=.o) $(MAIN_SRC:.c=.o)
OBJ_CXX = $(SRC_CXX:.cpp=.o) 

CC=gcc
CXX=g++
OBJ_CP=
AVRDUDE=
AR=ar
JAVAC=gcj

INC_FLAGS= -I$(SEARDUINO_INC_PATH)/arduino-sources/core \
           -I$(SEARDUINO_INC_PATH)/faked-arduino/include/arduino \
           -I$(SEARDUINO_INC_PATH)/faked-arduino/include \
           -I$(SEARDUINO_INC_PATH)/arduino \
           -I$(SEARDUINO_INC_PATH)/boards \
           -I$(SEARDUINO_INC_PATH)/

SEARDUINO_LIB=searduinostub

_CFLAGS=   -g $(USER_C_FLAGS) -Wall \
            $(LIB_FLAGS) \
            $(MODULE_C_FLAGS) \
            $(INC_FLAGS)

_CXXFLAGS=-g $(USER_CXX_FLAGS) \
             $(MODULE_CXX_FLAGS) \
             $(INC_FLAGS)

_LDFLAGS = $(USER_LD_FLAGS) -lpthread 

$(PROG): $(LIB) $(OBJ_C) $(OBJ_CXX)
	echo "OBJ_C  $(OBJ_C)"
	$(CC) $(LIB) $(OBJ_C) $(OBJ_CXX) -o $(PROG) $(LDFLAGS) 