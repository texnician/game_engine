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

prob_7_2_a: prob_7_2_a.c prob_7_2_a_2.c
	gcc -g -o $@.out $?

prob_7_2_b: prob_7_2_b_1.c prob_7_2_b_2.c
	gcc -g -o $@.out $?

clean:
	$(RM) *.o *.s *.i p *.out