CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = simulator

SRCS = main.c \
module1_algorithms/fifo.c \
module1_algorithms/lru.c \
module1_algorithms/optimal.c \
module2_display/display.c \
module3_ui/ui.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)