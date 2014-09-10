include deps/platform.mk

override CFLAGS+= -Ideps/$(PREFIX)/include/SDL2 -Ishared -Iengine -Ifpsgame -Wall -fsigned-char
override CXXFLAGS+= -Ideps/$(PREFIX)/include/SDL2 -Ishared -Iengine -Ifpsgame -std=gnu++0x -Wall -fsigned-char -fno-exceptions -fno-rtti
ifneq (,$(findstring -ggdb,$(CXXFLAGS)))
  STRIP=true
  UPX=true
else
  UPX=upx
endif

CLIENT_OBJS:= \
	shared/crypto.o \
	shared/geom.o \
	shared/stream.o \
	shared/tools.o \
	shared/zip.o \
	engine/3dgui.o \
	engine/bih.o \
	engine/blend.o \
	engine/blob.o \
	engine/client.o	\
	engine/command.o \
	engine/console.o \
	engine/cubeloader.o \
	engine/decal.o \
	engine/dynlight.o \
	engine/glare.o \
	engine/grass.o \
	engine/lightmap.o \
	engine/main.o \
	engine/material.o \
	engine/menus.o \
	engine/movie.o \
	engine/normal.o	\
	engine/octa.o \
	engine/octaedit.o \
	engine/octarender.o \
	engine/physics.o \
	engine/pvs.o \
	engine/rendergl.o \
	engine/rendermodel.o \
	engine/renderparticles.o \
	engine/rendersky.o \
	engine/rendertext.o \
	engine/renderva.o \
	engine/server.o	\
	engine/serverbrowser.o \
	engine/shader.o \
	engine/shadowmap.o \
	engine/sound.o \
	engine/texture.o \
	engine/water.o \
	engine/world.o \
	engine/worldio.o \
	fpsgame/ai.o \
	fpsgame/client.o \
	fpsgame/entities.o \
	fpsgame/fps.o \
	fpsgame/monster.o \
	fpsgame/movable.o \
	fpsgame/render.o \
	fpsgame/scoreboard.o \
	fpsgame/server.o \
	fpsgame/waypoint.o \
	fpsgame/weapon.o
MACOBJC:= \
	xcode/Launcher.o \
	xcode/main.o
MACOBJCXX:= xcode/macutils.o

ifdef WINDOWS
override LDFLAGS+= -mwindows
override LIBS+= -lenet -lSDL2 -lSDL2_image -ljpeg -lpng -lz -lSDL2_mixer -logg -lvorbis -lvorbisfile -lws2_32 -lwinmm -lopengl32 -ldxguid -lgdi32 -lole32 -limm32 -lversion -loleaut32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lpthread
endif

ifdef LINUX
override LIBS+= -lGL -lenet -lSDL2 -lSDL2_image -ljpeg -lpng -lz -lSDL2_mixer -logg -lvorbis -lvorbisfile -lm -ldl
ifneq (, $(findstring x86_64,$(PREFIX)))
override LDFLAGS+= -Wl,--wrap=__pow_finite,--wrap=__acosf_finite,--wrap=__log_finite,--wrap=__exp_finite,--wrap=__logf_finite,--wrap=__expf_finite,--wrap=__asin_finite,--wrap=__atan2f_finite,--wrap=__log10f_finite,--wrap=__atan2_finite,--wrap=__acos_finite,--wrap=memcpy
CLIENT_OBJS+= quirks/oldglibc64.o
else
override LDFLAGS+= -Wl,--wrap=__pow_finite
override CLIENT_OBJS+= quirks/oldglibc32.o
endif
endif

ifdef MAC
override LIBS+= -lenet -lSDL2 -lSDL2_image -ljpeg -lpng -lz -lSDL2_mixer -logg -lvorbis -lvorbisfile -liconv -framework IOKit -framework Cocoa -framework Carbon -framework CoreAudio -framework OpenGL -framework AudioUnit -lm -ldl
endif


quirks/oldglibc%: override CXXFLAGS += -fno-fast-math

default: all

all: client

clean:
	-$(RM) -r $(CLIENT_OBJS) $(MACOBJC) $(MACOBJCXX) quirks/*.o sauer_client sauerbraten.exe vcpp/mingw.res

ifdef WINDOWS
client: $(CLIENT_OBJS)
	$(WINDRES) -I vcpp -i vcpp/mingw.rc -J rc -o vcpp/mingw.res -O coff 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o sauerbraten.exe vcpp/mingw.res $(CLIENT_OBJS) -Wl,--as-needed -Wl,--start-group $(LIBS) -Wl,--end-group
	$(STRIP) sauerbraten.exe
	-$(UPX) sauerbraten.exe
endif

ifdef MAC
$(MACOBJCXX):
	$(CXX) -c $(CXXFLAGS) -o $@ $(subst .o,.mm,$@)
$(MACOBJC):
	$(CC) -c $(CFLAGS) -o $@ $(subst .o,.m,$@)

client:	$(CLIENT_OBJS) $(MACOBJCXX) $(MACOBJC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o sauerbraten $(CLIENT_OBJS) $(MACOBJCXX) $(MACOBJC) $(LIBS)
	$(STRIP) sauerbraten
	-$(UPX) sauerbraten
endif

ifdef LINUX
client:	$(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o sauer_client $(CLIENT_OBJS) -Wl,--as-needed -Wl,--start-group $(LIBS) -lrt -Wl,--end-group
	$(STRIP) sauer_client
ifneq ($(STRIP),true)
ifneq (, $(findstring x86_64,$(PREFIX)))
	./remove_symbol_version memcpy@GLIBC_2.2.5
endif
endif
	-$(UPX) sauer_client
endif

# DO NOT DELETE

engine/3dgui.o:		engine/engine.h engine/textedit.h
engine/bih.o:		engine/engine.h
engine/blend.o:		engine/engine.h
engine/blob.o:		engine/engine.h
engine/client.o:	engine/engine.h
engine/command.o:	engine/engine.h
engine/console.o:	engine/engine.h engine/sdl2_keymap_extrakeys.h
engine/cubeloader.o:	engine/engine.h
engine/decal.o:		engine/engine.h
engine/dynlight.o:	engine/engine.h
engine/engine.h:	shared/cube.h engine/world.h engine/octa.h engine/lightmap.h engine/bih.h engine/texture.h engine/model.h engine/varray.h
engine/glare.o:		engine/engine.h engine/rendertarget.h
engine/grass.o:		engine/engine.h
engine/lightmap.o:	engine/engine.h
engine/main.o:		engine/engine.h engine/sdosscripts.h
engine/master.o:	shared/cube.h
engine/material.o:	engine/engine.h
engine/menus.o:		engine/engine.h
engine/movie.o:		engine/engine.h
engine/normal.o:	engine/engine.h
engine/octa.o:		engine/engine.h
engine/octaedit.o:	engine/engine.h
engine/octarender.o:	engine/engine.h
engine/physics.o:	engine/engine.h engine/mpr.h
engine/pvs.o:		engine/engine.h
engine/rendergl.o:	engine/engine.h engine/varray.h
engine/rendermodel.o:	engine/engine.h engine/ragdoll.h engine/animmodel.h engine/vertmodel.h engine/skelmodel.h engine/md2.h engine/md3.h engine/md5.h engine/obj.h engine/smd.h engine/iqm.h
engine/renderparticles.o: engine/engine.h engine/rendertarget.h engine/depthfx.h engine/explosion.h engine/lensflare.h engine/lightning.h
engine/rendersky.o:	engine/engine.h
engine/rendertext.o:	engine/engine.h
engine/renderva.o:	engine/engine.h
engine/serverbrowser.o:	engine/engine.h
engine/server.o:	engine/engine.h
engine/shader.o:	engine/engine.h
engine/shadowmap.o:	engine/engine.h engine/rendertarget.h
engine/sound.o:		engine/engine.h
engine/texture.o:	engine/engine.h engine/scale.h
engine/water.o:		engine/engine.h
engine/world.o:		engine/engine.h
engine/worldio.o:	engine/engine.h
fpsgame/ai.o:		fpsgame/game.h
fpsgame/client.o:	fpsgame/game.h fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h
fpsgame/entities.o:	fpsgame/game.h
fpsgame/fps.o:		fpsgame/game.h
fpsgame/game.h:		shared/cube.h fpsgame/ai.h
fpsgame/monster.o:	fpsgame/game.h
fpsgame/movable.o:	fpsgame/game.h
fpsgame/render.o:	fpsgame/game.h
fpsgame/scoreboard.o:	fpsgame/game.h
fpsgame/server.o:	fpsgame/game.h fpsgame/capture.h fpsgame/ctf.h fpsgame/collect.h fpsgame/extinfo.h fpsgame/aiman.h
fpsgame/waypoint.o:	fpsgame/game.h
fpsgame/weapon.o:	fpsgame/game.h
shared/crypto.o:	shared/cube.h
shared/cube.h:		shared/tools.h shared/geom.h shared/ents.h shared/command.h shared/iengine.h shared/igame.h
shared/geom.o:		shared/cube.h
shared/stream.o:	shared/cube.h
shared/tools.o:		shared/cube.h
shared/zip.o:		shared/cube.h

xcode/Launcher.o:	xcode/Launcher.h
xcode/main.o:		xcode/Launcher.h

quirks/oldglibc32.o:	quirks/wrapper.hpp
quirks/oldglibc64.o:	quirks/wrapper.hpp

