all: pi mat_mul

OPENBLAS_PREFIX = $(shell brew --prefix openblas)

%: %.chpl
	chpl -I$(OPENBLAS_PREFIX)/include -L$(OPENBLAS_PREFIX)/lib -lblas $^

.PHONY: clean
clean:
	$(RM) pi mat_mul

.PHONY: run
run: pi mat_mul
	./pi --samples=1000
	./mat_mul --size=1000
