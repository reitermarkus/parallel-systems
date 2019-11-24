LDFLAGS += -fopenmp
CXXFLAGS ?= -std=c++17 -fopenmp

ifeq ($(shell test $(THREADS) -gt 1; echo $$?), 0)
	CPPFLAGS += -DTHREADS=$(THREADS)
else
	CPPFLAGS += -DTHREADS=8
endif

include ../shared/shared.mk
