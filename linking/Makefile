srcs = $(wildcard *.c)
objs = $(subst .c,.o,$(srcs))

all: $(objs)
	ld -o p $?

%.i: %.c
	cpp $< $@

%.s: %.i
	cc -S $< -O2 -o $@

%.o: %.s
	as -o $*.o $<

clean:
	$(RM) *.o *.s *.i p