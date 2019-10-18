CXX = mpic++
CXX_FLAGS = -O2 -std=c++17 -Wall -Wextra -pedantic

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
