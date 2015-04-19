SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp = .o)
CC = g++
CCFLAGS = -g -Wall -O0

raptor_app : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LIBS) 

%.o : %.cpp %.h
	$(CC) -c $< $(CCFLAGS)
clean:
	rm *.o
.PHONY:clean

