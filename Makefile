# Optimization level.  Change this -O2 to -Og or -O0 or whatever.
OPTIMIZE = -O2

# The C compiler and its options.
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -fanalyzer \
  -march=native -mtune=native -mrdrnd

# The archiver command, its options and filename extension.
TAR = tar
TARFLAGS = --gzip --transform 's,^,randall/,'
TAREXT = tgz

default: randall

randall: randall.c options.c output.c rand64-hw.c rand64-sw.c rand48.c
	$(CC) $(CFLAGS) $^ -o $@

assignment: randall-assignment.$(TAREXT)
assignment-files = COPYING Makefile randall.c
randall-assignment.$(TAREXT): $(assignment-files)
	$(TAR) $(TARFLAGS) -cf $@ $(assignment-files)

submission-tarball: randall-submission.$(TAREXT)
submission-files = $(assignment-files) \
  notes.txt # More files should be listed here, as needed.
randall-submission.$(TAREXT): $(submission-files)
	$(TAR) $(TARFLAGS) -cf $@ $(submission-files)

repository-tarball:
	$(TAR) -czf randall-git.tgz .git

.PHONY: default clean assignment submission-tarball repository-tarball

clean:
	rm -f *.o *.$(TAREXT) randall

check:
	make
	echo "General Cases: "
	./randall 10 | wc --bytes | grep -w "10" && echo "PASSED" || echo "FAILED"
	./randall 100 | wc --bytes | grep -w "100" && echo "PASSED" || echo "FAILED"
	./randall 1000000 | wc --bytes | grep -w "1000000" && echo "PASSED" || echo "FAILED"

	echo

	echo "Hardware Cases: "
	./randall 10 -i rdrand | wc --bytes | grep -w "10" && echo "PASSED" || echo "FAILED"
	./randall 100 -i rdrand | wc --bytes | grep -w "100" && echo "PASSED" || echo "FAILED"
	./randall 1000000 -i rdrand | wc --bytes | grep -w "1000000" && echo "PASSED" || echo "FAILED"

	echo

	echo "-i mrand48_r cases"
	./randall 10 -i mrand48_r | wc --bytes | grep -w "10" && echo "PASSED" || echo "FAILED"
	./randall 100 -i mrand48_r | wc --bytes | grep -w "100" && echo "PASSED" || echo "FAILED"
	./randall 1000000 -i mrand48_r | wc --bytes | grep -w "1000000" && echo "PASSED" || echo "FAILED"

	echo

	echo "-i /F cases: "
	./randall 3 -i /w/home.06/cs/ugrad/rolandy/lab5/lab5/test.txt | grep -w "123" && echo "PASSED" || echo "FAILED"
	./randall 8 -i /w/home.06/cs/ugrad/rolandy/lab5/lab5/test.txt | wc --bytes | grep -w "8" && echo "PASSED" || echo "FAILED"

	echo

	echo "-o stdio cases: "
	./randall 10 -o stdio | wc --bytes | grep -w "10" && echo "PASSED" || echo "FAILED"
	./randall 100 -o stdio | wc --bytes | grep -w "100" && echo "PASSED" || echo "FAILED"
	./randall 1000000 -o stdio | wc --bytes | grep -w "1000000" && echo "PASSED" || echo "FAILED"

	echo

	echo "-o N cases: "
	./randall 10 -o 5 | wc --bytes | grep -w "10" && echo "PASSED" || echo "FAILED"
	./randall 100 -o 10 | wc --bytes | grep -w "100" && echo "PASSED" || echo "FAILED"
	./randall 500000 -o 15 | wc --bytes | grep -w "500000" && echo "FAILED" || echo "PASSED"