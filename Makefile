# Makefile for randall

CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -march=native

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

check: $(TARGET)
	@echo "Running basic tests..."
	# Test 1: Generate 0 bytes (should produce no output)
	@if ./$(TARGET) 0 2>/dev/null | wc -c | grep -q '^0$$'; then \
		echo "Test 1 passed: 0 bytes"; \
	else \
		echo "Test 1 failed: 0 bytes"; \
	fi
	
	# Test 2: Generate 100 bytes (check output length)
	@if ./$(TARGET) 100 2>/dev/null | wc -c | grep -q '^100$$'; then \
		echo "Test 2 passed: 100 bytes"; \
	else \
		echo "Test 2 failed: 100 bytes"; \
	fi
	
	# Test 3: Test different input methods
	@if ./$(TARGET) -i rdrand 50 2>/dev/null | wc -c | grep -q '^50$$'; then \
		echo "Test 3a passed: rdrand"; \
	else \
		echo "Test 3a failed: rdrand"; \
	fi
	@if ./$(TARGET) -i mrand48_r 50 2>/dev/null | wc -c | grep -q '^50$$'; then \
		echo "Test 3b passed: mrand48_r"; \
	else \
		echo "Test 3b failed: mrand48_r"; \
	fi
	@if ./$(TARGET) -i /dev/urandom 50 2>/dev/null | wc -c | grep -q '^50$$'; then \
		echo "Test 3c passed: /dev/urandom"; \
	else \
		echo "Test 3c failed: /dev/urandom"; \
	fi
	
	# Test 4: Test block output
	@if ./$(TARGET) -o 1024 100 2>/dev/null | wc -c | grep -q '^100$$'; then \
		echo "Test 4 passed: block output"; \
	else \
		echo "Test 4 failed: block output"; \
	fi
	
	# Test 5: Invalid arguments should fail
	@if ! ./$(TARGET) -100 2>/dev/null; then \
		echo "Test 5a passed: negative bytes rejected"; \
	else \
		echo "Test 5a failed: negative bytes not rejected"; \
	fi
	@if ! ./$(TARGET) abc 2>/dev/null; then \
		echo "Test 5b passed: invalid bytes rejected"; \
	else \
		echo "Test 5b failed: invalid bytes not rejected"; \
	fi
	@if ! ./$(TARGET) -i invalid 100 2>/dev/null; then \
		echo "Test 5c passed: invalid input rejected"; \
	else \
		echo "Test 5c failed: invalid input not rejected"; \
	fi
	
	@echo "All tests completed."

clean:
	rm -f $(OBJECTS) $(TARGET) rand.data *.tgz

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