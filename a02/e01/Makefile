CXX = mpic++
CXX_FLAGS = -O2 -std=c++17 -Wall -Wextra -pedantic -lboost_mpi -lboost_serialization

ifneq ("$(wildcard $(HOME)/.local/include)", "")
	CXX_FLAGS += -I"$(HOME)/.local/include"
endif

ifneq ("$(wildcard $(HOME)/.local/lib)", "")
	CXX_FLAGS += -L"$(HOME)/.local/lib" -Wl,-rpath,"$(HOME)/.local/lib"
endif

TARGETS = pi_mpi pi_seq

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) $(TARGETS)

.PHONY: run
run: all
	for target in $(TARGETS); do \
	  ./$${target}; \
	done
