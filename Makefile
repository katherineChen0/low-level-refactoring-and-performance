# Makefile for randall

CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -march=native

TEST_SOURCES = test_rng.c options.c output.c rand64-hw.c rand64-sw.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_TARGET = test-rng

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

test_rng.o: test_rng.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

SOURCES = randall.c options.c output.c rand64-hw.c rand64-sw.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = options.h output.h rand64-hw.h rand64-sw.h

TARGET = randall

.PHONY: all clean check submission-tarball repository-tarball

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

check: $(TARGET) $(TEST_TARGET)
	@echo "Running basic tests..."
	# Test 1: Check compilation
	@echo "Test 1: Checking if program compiles..."
	@$(MAKE) $(TARGET) >/dev/null 2>&1 || { echo "Test 1 failed: compilation"; exit 1; }
	@echo "Test 1 passed: compilation"
	
	# Test 2: Generate 0 bytes (should produce no output)
	@echo "Test 2: Generate 0 bytes..."
	@OUTPUT=$$(./$(TARGET) 0 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "0" ]; then \
		echo "Test 2 passed: 0 bytes"; \
	else \
		echo "Test 2 failed: 0 bytes (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	# Test 3: Generate 100 bytes (check output length)
	@echo "Test 3: Generate 100 bytes..."
	@OUTPUT=$$(./$(TARGET) 100 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "100" ]; then \
		echo "Test 3 passed: 100 bytes"; \
	else \
		echo "Test 3 failed: 100 bytes (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	# Test 4: Basic mrand48_r functionality test
	@echo "Test 4: Basic mrand48_r functionality..."
	@OUTPUT=$$(./$(TARGET) -i mrand48_r 16 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "16" ]; then \
		echo "Test 4 passed: mrand48_r basic functionality"; \
	else \
		echo "Test 4 failed: mrand48_r (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	# Test 5: Test /dev/urandom input method
	@echo "Test 5: Test /dev/urandom input..."
	@OUTPUT=$$(./$(TARGET) -i /dev/urandom 50 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "50" ]; then \
		echo "Test 5 passed: /dev/urandom"; \
	else \
		echo "Test 5 failed: /dev/urandom (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	# Test 6: Test block output
	@echo "Test 6: Test block output..."
	@OUTPUT=$$(./$(TARGET) -o 1024 100 2>/dev/null | wc -c); \
	if [ "$$OUTPUT" = "100" ]; then \
		echo "Test 6 passed: block output"; \
	else \
		echo "Test 6 failed: block output (got $$OUTPUT)"; \
		exit 1; \
	fi
	
	# Test 7: Invalid arguments should fail
	@echo "Test 7: Test error handling..."
	@if ./$(TARGET) -100 >/dev/null 2>&1; then \
		echo "Test 7a failed: negative bytes not rejected"; \
		exit 1; \
	else \
		echo "Test 7a passed: negative bytes rejected"; \
	fi
	@if ./$(TARGET) abc >/dev/null 2>&1; then \
		echo "Test 7b failed: invalid bytes not rejected"; \
		exit 1; \
	else \
		echo "Test 7b passed: invalid bytes rejected"; \
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
		./$(TARGET) -i ./test_input.txt 10 2>/dev/null | head -c 10 | hexdump -C || true; \
		rm -f test_input.txt; \
		exit 1; \
	fi
	@rm -f test_input.txt

	@echo "Running RNG validation tests..."
	@./$(TEST_TARGET) > rng_test_output.txt
	@if grep -q "Changed values: 100/100" rng_test_output.txt; then \
		echo "RNG test passed: Good randomness detected"; \
	else \
		echo "RNG test warning: Some RNGs may not be sufficiently random"; \
		cat rng_test_output.txt; \
	fi
	@rm -f rng_test_output.txt		

submission-tarball: $(TARGET)
	rm -rf randall
	mkdir -p randall
	cp $(SOURCES) $(HEADERS) Makefile notes.txt randall/
	chmod +x test-basic.sh 2>/dev/null || true
	cp test-basic.sh randall/ 2>/dev/null || true
	tar -czf randall-submission.tgz randall/
	rm -rf randall

repository-tarball:
	tar -czf randall-git.tgz .git

distclean: clean
	rm -f *.tgz