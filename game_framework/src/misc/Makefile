CXX := g++

CXXFLAGS += -g -Wall -pipe -std=c++0x

THREAD_FLAG := -pthread

dep_dir := .deps

$(shell test -e $(dep_dir) || mkdir $(dep_dir))

CXXSUFFIX := cpp

srcs := $(wildcard *.$(CXXSUFFIX))

objs := $(subst .$(CXXSUFFIX),.o,$(srcs))

deps := $(addprefix $(dep_dir)/,$(subst .$(CXXSUFFIX),.Tpo,$(srcs)))
bin := misc

.PHONY: all clean

all: $(bin)

-include $(deps)

%.o: %.$(CXXSUFFIX) Makefile
	$(CXX) $(CXXFLAGS) $(THREAD_FLAG) -fPIC -MD -MP -MF $(dir $@)$(dep_dir)/$(notdir $*).Tpo -c -o $@ $<

$(bin): $(objs) ../log.o
	$(CXX) $(THREAD_FLAG) -o $@ $^

clean:
	rm -f *.o
	rm -f $(bin)


