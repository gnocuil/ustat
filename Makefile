CC     := g++
CFLAGS := -O2 -std=c++0x -lpthread
TARGET := ustat
OBJS   := main.o eth.o

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
clean :
	rm -f $(TARGET)
	rm -f *.o
