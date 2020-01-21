all: pi mat_mul

CHPL_FLAGS = --fast --vectorize

ifneq ($(CHPL_COMM),gasnet)
	CHPL_FLAGS += --local
endif

%: %.chpl
	chpl $(CHPL_FLAGS) $^

.PHONY: clean
clean:
	$(RM) pi mat_mul

.PHONY: run
run: pi mat_mul
	./pi --samples=1000
	./mat_mul --size=1000
