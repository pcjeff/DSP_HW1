.PHONY: all clean

CFLAGS+=-g
LDFLAGS+=-lm     # link to math library

TARGET=train

all: $(TARGET)
# type make/make all to compile test_hmm

clean:
	$(RM) $(TARGET)   # type make clean to remove the compiled file
