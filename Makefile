CC = gcc
CFLAGS += -Wall -Wextra -Werror
SRC = my_blockchain.c blockchain_interface.c blockchain_system.c blockchain_backup.c my_blockchain.h
TARGET = my_blockchain

all: $(TARGET)

$(TARGET) : $(SRC)
	gcc $(CFLAGS) -o $(TARGET) $(SRC) 

clean:
	rm -f *.o

fclean: clean
	rm -f $(TARGET)

re: fclean all