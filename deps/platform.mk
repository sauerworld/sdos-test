DEPSDIR:= $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
PREFIX:= $(shell $(DEPSDIR)/config.guess)
ARCHFLAGS:= -I$(DEPSDIR)/$(PREFIX)/include
OPTFLAGS:= -ffast-math -O3 -fomit-frame-pointer -fvisibility=hidden

ifneq (, $(findstring mingw,$(PREFIX)))
WINDOWS:= 1
CXX:= $(PREFIX)-g++
CC:= $(PREFIX)-gcc
WINDRES:= $(PREFIX)-windres
ARCHFLAGS+= -pthread -I$(DEPSDIR)/extra/xaudio2

else ifneq (, $(findstring linux,$(PREFIX)))
LINUX:= 1
ARCHFLAGS+= -pthread

else ifneq (, $(findstring darwin,$(PREFIX)))
CXX:= clang++
CC:= clang
ARCHFLAGS+= -mmacosx-version-min=10.5
MAC:= 1

else
$(error Unknown architecture $(PREFIX))
endif

STRIP:= $(shell which $(PLATFORM)-$(STRIP) 2>/dev/null || echo strip)

ifneq (, $(findstring x86_64,$(PREFIX)))
ARCHFLAGS+= -m64 
else
ARCHFLAGS+= -m32 
endif


export CC
export CXX
override CPPFLAGS+= $(ARCHFLAGS) $(OPTFLAGS)
export CPPFLAGS
override CFLAGS+= $(ARCHFLAGS) $(OPTFLAGS)
export CFLAGS
override LDFLAGS+= $(ARCHFLAGS) $(OPTFLAGS) -L$(DEPSDIR)/$(PREFIX)/lib
export LDFLAGS
override CXXFLAGS+= $(ARCHFLAGS) $(OPTFLAGS) -fvisibility-inlines-hidden
export CXXFLAGS
export PKG_CONFIG_LIBDIR:= $(DEPSDIR)/$(PREFIX)/lib/pkgconfig
override PATH:= $(DEPSDIR)/$(PREFIX)/bin:$(PATH)
export PATH

