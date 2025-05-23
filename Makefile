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
	./$(TARGET) 0 | wc -c | grep -q '^0$$' && echo "Test 1 passed: 0 bytes" || echo "Test 1 failed"
	
	# Test 2: Generate 100 bytes (check output length)
	./$(TARGET) 100 | wc -c | grep -q '^100$$' && echo "Test 2 passed: 100 bytes" || echo "Test 2 failed"
	
	# Test 3: Test different input methods
	./$(TARGET) -i rdrand 50 | wc -c | grep -q '^50$$' && echo "Test 3a passed: rdrand" || echo "Test 3a failed"
	./$(TARGET) -i mrand48_r 50 | wc -c | grep -q '^50$$' && echo "Test 3b passed: mrand48_r" || echo "Test 3b failed"
	./$(TARGET) -i /dev/urandom 50 | wc -c | grep -q '^50$$' && echo "Test 3c passed: /dev/urandom" || echo "Test 3c failed"
	
	# Test 4: Test block output
	./$(TARGET) -o 1024 100 | wc -c | grep -q '^100$$' && echo "Test 4 passed: block output" || echo "Test 4 failed"
	
	# Test 5: Invalid arguments should fail
	! ./$(TARGET) -100 2>/dev/null && echo "Test 5a passed: negative bytes rejected" || echo "Test 5a failed"
	! ./$(TARGET) abc 2>/dev/null && echo "Test 5b passed: invalid bytes rejected" || echo "Test 5b failed"
	! ./$(TARGET) -i invalid 100 2>/dev/null && echo "Test 5c passed: invalid input rejected" || echo "Test 5c failed"
	
	@echo "All tests completed."

clean:
	rm -f $(OBJECTS) $(TARGET) rand.data *.tgz

submission-tarball: $(TARGET)
	tar -czf randall-submission.tgz $(SOURCES) $(HEADERS) Makefile notes.txt

repository-tarball:
	tar -czf randall-git.tgz .git

distclean: clean
	rm -f *.tgz