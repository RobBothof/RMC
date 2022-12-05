SOLIB_EXT = so
PLATFORM = linux

LIBPD_DIR = /home/robber/development/lib/libpd
LIBPD = $(LIBPD_DIR)/libs/libpd.$(SOLIB_EXT)
RTMIDI_DIR = /home/robber/development/lib/rtmidi

SRC= src/main.cpp src/masterControl.cpp src/lpd.cpp src/audio.cpp src/gui.cpp src/functions.cpp src/midiUart.cpp src/midiLocal.cpp src/filesystem.cpp src/controller.cpp
#EXTERNALS = src/externals/freeverb~.c src/externals/vcf_filter~.c
EXTERNALS = src/externals/iemlib/src/vcf_filter~.c
#EXTERNALS = src/externals/freeverb~.c

RTMIDI_SRC = $(RTMIDI_DIR)/RtMidi.cpp
TARGET = rmc

RTMIDIFLAGS = -I$(RTMIDI_DIR) -Wall -D__LINUX_ALSA__ -lasound -lpthread

LIBPDFLAGS = -I$(LIBPD_DIR)/pure-data/src -I$(LIBPD_DIR)/libpd_wrapper \
           -I$(LIBPD_DIR)/libpd_wrapper/util -I$(LIBPD_DIR)/cpp \
           -I./src -std=c++11 -I$(RTMIDI_DIR) -DLIBPD_USE_STD_MUTEX -O3 

CXXFLAGS = -I$(LIBPD_DIR)/pure-data/src -I$(LIBPD_DIR)/libpd_wrapper \
           -I$(LIBPD_DIR)/libpd_wrapper/util -I$(LIBPD_DIR)/cpp \
           -I./src -std=c++11 -I$(RTMIDI_DIR) -DLIBPD_USE_STD_MUTEX -O3 

# CFLAGS = -I$(LIBPD_DIR)/pure-data/src -I$(LIBPD_DIR)/libpd_wrapper \
#            -I$(LIBPD_DIR)/libpd_wrapper/util -I$(LIBPD_DIR)/cpp \
# 		   -I./src/externals/iemlib/include \
#            -I./src -DPD -O3 

# EXTFLAGS = -I$(LIBPD_DIR)/pure-data/src -I$(LIBPD_DIR)/libpd_wrapper \
#            -I$(LIBPD_DIR)/libpd_wrapper/util -I$(LIBPD_DIR)/cpp \
# 		   -I./src/externals/iemlib/include \
#            -I./src -DPD -O3 

LDFLAGS= -g
LDLIBS= -lasound -lpthread -ljack -lSDL2 -lmraa -lSDL2_gpu

#$(TARGET): $(RTMIDI_SRC:.cpp=.o) $(SRC:.cpp=.o) $(EXTERNALS:.c=.o) $(LIBPD) 
#	g++ $(LDFLAGS) $(LIBPDFLAGS) -o $(TARGET) $(SRC:.cpp=.o) $(RTMIDI_SRC:.cpp=.o) $(EXTERNALS:.c=.o) $(LIBPD) $(LDLIBS)

$(TARGET): $(RTMIDI_SRC:.cpp=.o) $(SRC:.cpp=.o) $(LIBPD) 
	g++ $(LDFLAGS) $(LIBPDFLAGS) -o $(TARGET) $(SRC:.cpp=.o) $(RTMIDI_SRC:.cpp=.o) $(LIBPD) $(LDLIBS)

$(RTMIDI_SRC:.cpp=.o): $(RTMIDI_SRC)
	g++ $(RTMIDIFLAGS) -c $(RTMIDI_SRC) -o $(RTMIDI_SRC:.cpp=.o)

#src/externals/freeverb~.o: src/externals/freeverb~.c
#src/externals/vcf_filter~.o:src/externals/vcf_filter~.c

# .c.o:
# 	cc -c -o $@ $(EXTFLAGS) $<

#$(EXTERNALS:.c=.o):
#	g++ $(EXTFLAGS) -c $(EXTERNALS) -o $(EXTERNALS:.c=.o)

#$(SRC:.cpp=.o): $(SRC)
#	g++ $(LIBPDFLAGS) -c $(SRC) -o $(SRC:.cpp=.o)

$(LIBPD):
	cd $(LIBPD_DIR) && make UTIL=true EXTRA=true

clean:
	rm -f src/*.o
	rm -f src/externals/*.o