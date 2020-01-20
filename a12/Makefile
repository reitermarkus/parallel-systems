all: pi mat_mul

OPENBLAS_PREFIX = $(shell brew --prefix openblas)

CHPL_FLAGS = --fast --vectorize

ifneq ($(CHPL_COMM),gasnet)
	CHPL_FLAGS += --local
endif

%: %.chpl
	chpl $(CHPL_FLAGS) -I$(OPENBLAS_PREFIX)/include -L$(OPENBLAS_PREFIX)/lib $^

.PHONY: clean
clean:
	$(RM) pi mat_mul

.PHONY: run
run: pi mat_mul
	./pi --samples=1000
	./mat_mul --size=1000
