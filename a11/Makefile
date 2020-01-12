TARGETS = real_seq real_omp

include ../shared/shared.mk

CFLAGS += -g3
LDFLAGS += -lm

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

%_seq.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^ -o $@

real_seq: $(patsubst %.o,%_seq.o,$(OBJS))
	$(CC) $^ $(LDFLAGS) -o $@

%_omp.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPENMP_CPPFLAGS) -c $^ -o $@

real_omp: $(patsubst %.o,%_omp.o,$(OBJS))
	$(CC) $^ $(LDFLAGS) $(OPENMP_LDFLAGS) -o $@
