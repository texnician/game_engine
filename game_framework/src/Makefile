CXX := g++

CXXFLAGS += -g -std=c++0x -Wall -pipe

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

%.o: %.$(CXXSUFFIX)
	$(CXX) $(CXXFLAGS) -fPIC -MD -MP -MF $(dep_dir)/$*.Tpo -c -o $@ $<

$(bin): $(objs)
	$(CXX) -o $@ $^

clean:
	rm -f *.o
	rm -f $(bin)


