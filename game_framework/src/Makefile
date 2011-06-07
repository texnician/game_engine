CXX ?= g++

CXXFLAGS := -g -Wall -pipe -std=c++0x

GUILE := guile-2.0
GUILE_CFLAGS := $(shell pkg-config --cflags guile-2.0)
GUILE_LDFLAGS := -L/usr/local/libs -lguile-2.0
#GUILE_LDFLAGS := $(shell pkg-config --libs guile-2.0)

LDFLAGS += $(GUILE_LDFLAGS)

CXXFLAGS += $(GUILE_CFLAGS)

THREAD_FLAG := -pthread

dep_dir := .deps

$(shell test -e $(dep_dir) || mkdir $(dep_dir))

CXXSUFFIX := cpp

srcs := $(wildcard *.$(CXXSUFFIX))

objs := $(subst .$(CXXSUFFIX),.o,$(srcs))

deps := $(addprefix $(dep_dir)/,$(subst .$(CXXSUFFIX),.Tpo,$(srcs)))

lib_stem := gm

static_lib := libgm.a

bin := net_framework

.PHONY: all clean

all: $(bin)

-include $(deps)

# -DGC_NO_THREAD_REDIRECTS

%.o: %.$(CXXSUFFIX) Makefile
	$(CXX) $(CXXFLAGS) $(THREAD_FLAG) -fPIC -MD -MP -MF $(dir $@)$(dep_dir)/$(notdir $*).Tpo -c -o $@ $<

$(bin): $(objs)
	$(CXX) $(THREAD_FLAG) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o
	rm -f $(bin)


