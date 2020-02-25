#target you can change test to what you want
TARGET  := test_simple_sds_server


  
#compile and lib parameter
CC      := gcc
LIBS    := -L -lpthread -ljansson  -lev  -lpthread
DEFINES := 
INCLUDE := -I .
CFLAGS  := -g  -O0 -std=gnu99 -D_GNU_SOURCE $(DEFINES) $(INCLUDE)
BINFLAGS:= -g  -O0 -std=gnu99 -DGFS_TEST $(INCLUDE)
OBJECT_PATH := ./
SOURCE  := $(wildcard ./*.c ../common/*.c)
OBJS    := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))



#i think you should do anything here
.PHONY : everything objs clean rebuild
  
everything : $(TARGET) #$(TEST_BIN)
  
all : $(TARGET) #$(TEST_BIN)
  
objs : $(OBJS)
  
rebuild: everything
                
clean :
	rm -fr $(OBJECT_PATH)*.o $(OBJECT_PATH)*.o
	rm -rf $(TARGET)
	rm -rf $(OBJECT_PATH)bin/$(TARGET)

install:
	cp $(TARGET) $(OBJECT_PATH)/bin/
	rm -fr $(OBJECT_PATH)*.o $(OBJECT_PATH)*.o
 
  
$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)  $(LIBS)
	rm -fr $(OBJECT_PATH)*.o $(OBJECT_PATH)*.o


#$(TEST_BIN) : $(OBJS)
#	$(CC) $(BINFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)