CXX := g++

CXXFLAGS += -g -Wall -pipe -std=c++0x

dep_dir := .deps

$(shell test -e $(dep_dir) || mkdir $(dep_dir))

CXXSUFFIX := cpp

srcs := $(wildcard *.$(CXXSUFFIX))

objs := $(subst .$(CXXSUFFIX),.o,$(srcs))

deps := $(addprefix $(dep_dir)/,$(subst .$(CXXSUFFIX),.Tpo,$(srcs)))
bin := net_framework

.PHONY: all clean

all: $(bin)

-include $(deps)

%.o: %.$(CXXSUFFIX) Makefile
	$(CXX) $(CXXFLAGS) -fPIC -MD -MP -MF $(dep_dir)/$*.Tpo -c -o $@ $<

$(bin): $(objs)
	$(CXX) -o $@ $^

clean:
	rm -f *.o
	rm -f $(bin)


