CXX := mpic++

CPPFLAGS ?= -O3 -Wall -Wextra -pedantic
CXXFLAGS ?= -std=c++17

LDFLAGS += -lboost_mpi -lboost_serialization

ifneq ("$(wildcard $(HOME)/.local/include)", "")
	CPPFLAGS += -I"$(HOME)/.local/include"
endif

ifneq ("$(wildcard $(HOME)/.local/lib)", "")
	LDFLAGS += -L"$(HOME)/.local/lib" -Wl,-rpath,"$(HOME)/.local/lib"
endif

ifeq ($(DEBUG),1)
  CPPFLAGS += -DDEBUG
endif

TARGETS ?= $(sort $(patsubst %.cpp,%,$(wildcard *_seq.cpp *_mpi.cpp)))

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o
	$(CXX) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o

.PHONY: run
run: all
	@separator=''
	@for target in $(TARGETS); do \
		printf "$${separator}"; \
	  echo "$${target}:"; \
		if [[ "$${target}" = *_mpi ]]; then \
		  mpiexec ./$${target}; \
		else \
		  ./$${target}; \
		fi;\
		separator="\n"; \
	done
