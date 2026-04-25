# ============================================================
#  Makefile — Page Replacement Simulator (C Core)
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c99
TARGET  = page_sim
SRC     = page_replacement.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC) page_replacement.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lm
	@echo "✔  Built: ./$(TARGET)"

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
