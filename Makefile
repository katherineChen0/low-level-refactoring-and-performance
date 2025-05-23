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
# Make x86-64 random byte generators

OPTIMIZE = -O2
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -fanalyzer \
  -march=native -mtune=native -mrdrnd

TAR = tar
TARFLAGS = --gzip --transform 's,^,randall/,'
TAREXT = tgz

SRCS = randall.c options.c output.c rand64-hw.c rand64-sw.c
OBJS = $(SRCS:.c=.o)
HEADERS = options.h output.h rand64-hw.h rand64-sw.h

default: randall

randall: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Explicit dependencies
randall.o: randall.c $(HEADERS)
options.o: options.c options.h
output.o: output.c output.h options.h
rand64-hw.o: rand64-hw.c rand64-hw.h
rand64-sw.o: rand64-sw.c rand64-sw.h

check: randall
	@echo "Running tests..."
	@echo -n "Test 1: output length verification..."
	@./randall 100 | wc -c | grep -q 100 && echo "PASS" || (echo "FAIL"; exit 1)
	@echo -n "Test 2: Different output..."
	@./randall 100 > out1 && ./randall 100 > out2 && \
	cmp -s out1 out2 && echo "FAIL" || echo "PASS"
	@rm -f out1 out2

clean:
	rm -f *.o *.$(TAREXT) randall

assignment: randall-assignment.$(TAREXT)
randall-assignment.$(TAREXT): $(SRCS) $(HEADERS) Makefile COPYING
	$(TAR) $(TARFLAGS) -cf $@ $^

submission-tarball: randall-submission.$(TAREXT)
randall-submission.$(TAREXT): $(SRCS) $(HEADERS) Makefile COPYING notes.txt test-llm.txt
	$(TAR) $(TARFLAGS) -cf $@ $^

repository-tarball:
	$(TAR) -czf randall-git.tgz .git

.PHONY: default clean check assignment submission-tarball repository-tarball