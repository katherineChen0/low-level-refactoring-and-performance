# Make x86-64 random byte generators.

# Copyright 2015, 2020, 2021 Paul Eggert

# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

# Source files
SRCS = randall.c options.c output.c rand64-hw.c rand64-sw.c
OBJS = $(SRCS:.c=.o)
HEADERS = options.h output.h rand64-hw.h rand64-sw.h

default: randall

randall: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

randall.o: randall.c $(HEADERS)
options.o: options.c options.h
output.o: output.c output.h options.h
rand64-hw.o: rand64-hw.c rand64-hw.h
rand64-sw.o: rand64-sw.c rand64-sw.h

# test target to verify program functionality
check: randall
	@echo "Running tests..."
	@echo -n "Test 1: output length verification..."
	@./randall 100 | wc -c | grep -q 100 && echo "PASS" || (echo "FAIL"; exit 1)
	@echo -n "Test 2: Different output for different runs..."
	@./randall 100 > output1 && ./randall 100 > output2 && \
	cmp -s output1 output2 && echo "FAIL (outputs identical)" || echo "PASS"
	@echo -n "Test 3: mrand48_r input method..."
	@./randall -i mrand48_r 100 > output3 && echo "PASS" || (echo "FAIL"; exit 1)
	@echo -n "Test 4: block output method..."
	@./randall -o 64 100 > output4 && echo "PASS" || (echo "FAIL"; exit 1)
	@rm -f output1 output2 output3 output4
	@echo "All tests completed"

assignment: randall-assignment.$(TAREXT)
assignment-files = COPYING Makefile $(SRCS) $(HEADERS)
randall-assignment.$(TAREXT): $(assignment-files)
	$(TAR) $(TARFLAGS) -cf $@ $(assignment-files)

submission-tarball: randall-submission.$(TAREXT)
submission-files = $(assignment-files) \
  notes.txt README.md # More files should be listed here, as needed.
randall-submission.$(TAREXT): $(submission-files)
	$(TAR) $(TARFLAGS) -cf $@ $(submission-files)

repository-tarball:
	$(TAR) -czf randall-git.tgz .git

.PHONY: default clean check assignment submission-tarball repository-tarball

clean:
	rm -f *.o *.$(TAREXT) randall output*