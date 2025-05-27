# Makefile for randall

CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -march=native

SOURCES = randall.c options.c output.c rand64-hw.c rand64-sw.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = options.h output.h rand64-hw.h rand64-sw.h
TARGET = randall

TEST_RNG_SOURCES = test_rng.c rand64-hw.c rand64-sw.c
TEST_RNG_OBJECTS = $(TEST_RNG_SOURCES:.c=.o)
TEST_RNG_TARGET = test-rng

.PHONY: all clean check submission-tarball repository-tarball distclean

all: $(TARGET) $(TEST_RNG_TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_RNG_TARGET): $(TEST_RNG_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

check:
	@echo "Running RNG implementation tests..."
	@$(MAKE) $(TEST_RNG_TARGET) >/dev/null 2>&1 || { echo "Failed to build RNG test program"; exit 1; }
	@./$(TEST_RNG_TARGET) || { echo "RNG tests failed"; exit 1; }
	@echo "RNG tests passed"
	
	@echo "Running randall functionality tests..."
	@$(MAKE) $(TARGET) >/dev/null 2>&1 || { echo "Failed to build randall"; exit 1; }
	
	@echo "Test 1: Checking if program compiles..."
	@test -f $(TARGET) && echo "Test 1 passed: compilation" || { echo "Test 1 failed: compilation"; exit 1; }
	
	@echo "Test 2: Generate 0 bytes..."
	@OUTPUT=$$(./$(TARGET) 0 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "0" ]; then \
		echo "Test 2 passed: 0 bytes"; \
	else \
		echo "Test 2 failed: 0 bytes (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	@echo "Test 3: Generate 100 bytes..."
	@OUTPUT=$$(./$(TARGET) 100 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "100" ]; then \
		echo "Test 3 passed: 100 bytes"; \
	else \
		echo "Test 3 failed: 100 bytes (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	@echo "Test 4: Basic mrand48_r functionality..."
	@OUTPUT=$$(./$(TARGET) -i mrand48_r 16 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "16" ]; then \
		echo "Test 4 passed: mrand48_r basic functionality"; \
	else \
		echo "Test 4 failed: mrand48_r (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	@echo "Test 5: Test /dev/urandom input..."
	@OUTPUT=$$(./$(TARGET) -i /dev/urandom 50 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "50" ]; then \
		echo "Test 5 passed: /dev/urandom"; \
	else \
		echo "Test 5 failed: /dev/urandom (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	@echo "Test 6: Test block output..."
	@OUTPUT=$$(./$(TARGET) -o 1024 100 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "100" ]; then \
		echo "Test 6 passed: block output"; \
	else \
		echo "Test 6 failed: block output (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	@echo "Test 7: Invalid arguments should fail..."
	@if timeout 5 ./$(TARGET) -100 >/dev/null 2>&1; then \
		echo "Test 7a failed: negative bytes not rejected"; \
		exit 1; \
	else \
		echo "Test 7a passed: negative bytes rejected"; \
	fi
	
	@if timeout 5 ./$(TARGET) abc >/dev/null 2>&1; then \
		echo "Test 7b failed: invalid bytes not rejected"; \
		exit 1; \
	else \
		echo "Test 7b passed: invalid bytes rejected"; \
	fi
	
	@echo "Test 7c: Non-existent input file should fail..."
	@if timeout 5 ./$(TARGET) -i input_non_existent 10 >/dev/null 2>&1; then \
		echo "Test 7c failed: non-existent file not rejected"; \
		exit 1; \
	else \
		echo "Test 7c passed: non-existent file rejected"; \
	fi
	
	@echo "Test 8: Test file input..."
	@echo "0123456789ABCDEF" > test_input.txt
	@OUTPUT=$$(./$(TARGET) -i ./test_input.txt 10 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "10" ]; then \
		echo "Test 8 passed: file input"; \
	else \
		echo "Test 8 failed: file input (got $$OUTPUT)"; \
		echo "Debug info:"; \
		echo "File contents:"; \
		cat test_input.txt; \
		echo "File size:"; \
		wc -c test_input.txt; \
		echo "Trying to read 10 bytes:"; \
		./$(TARGET) -i ./test_input.txt 10 2>/dev/null | head -c 10 | cat -A || true; \
		rm -f test_input.txt; \
		exit 1; \
	fi
	@rm -f test_input.txt
	@echo "All tests passed successfully"

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_RNG_OBJECTS) $(TEST_RNG_TARGET) \
	      rand.data *.tgz test_output.bin test_input.txt rng_test_output.txt

submission-tarball: $(TARGET) $(TEST_RNG_TARGET)
	rm -rf randall
	mkdir -p randall
	cp $(SOURCES) $(HEADERS) test_rng.c Makefile notes.txt randall/
	chmod +x test-basic.sh 2>/dev/null || true
	cp test-basic.sh randall/ 2>/dev/null || true
	tar -czf randall-submission.tgz randall/
	rm -rf randall

repository-tarball:
	tar -czf randall-git.tgz .git

distclean: clean
	rm -f *.tgz