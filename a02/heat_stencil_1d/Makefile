CXX = mpic++
CXX_FLAGS = -O2 -std=c++17 -Wall -Wextra -pedantic

TARGETS = heat_stencil_1d_seq

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) $(TARGETS)

.PHONY: run
run: all
	@for stencil in "$(TARGETS)"; do \
	  ./$(TARGETS); \
	done
