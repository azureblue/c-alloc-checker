CC_FLAGS = -Wall $(CC_O) -g -std=c99

SRC = $(wildcard ../*.c)
OBJ = $(SRC:.c=.o)	
TEST_SRC = $(wildcard *.c)
TEST_OBJ = $(TEST_SRC:.c=.o)	
TEST = alloc_check_test

test: build-test
	./$(TEST)

build-test: $(OBJ) $(TEST_OBJ)
	$(CC) $(CC_O) -g $(OBJ) $(TEST_OBJ) -o $(TEST)

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(TEST) $(OBJ) $(TEST_OBJ)
