DEBUG = n

TITLE_ID = BSDVIEWER
TARGET   = BSDViewer
OBJS     = bsdViewer.o cppsample.o
PSVITAIP = 192.168.50.82
LIBS = -lvita2d -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lSceTouch_stub -lScePgf_stub -lScePvf_stub \
	-lSceCommonDialog_stub -lfreetype -lpng -ljpeg -lz -lm -lc

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX     = $(PREFIX)-g++
CFLAGS  = -Wl,-q -Wall -fno-lto
ASFLAGS = $(CFLAGS)

ifeq ($(DEBUG),y)
	DEBFLAGS = -DDEBUG
else
	DEBFLAGS = 
endif

CFLAGS += $(DEBFLAGS)
CXXFLAGS += -c -lstd=c++11

LIVEAREA = -a sce_sys/icon0.png=sce_sys/icon0.png \
		   -a sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		   -a sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		   -a sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
		   -a font.pgf=font.pgf

all: $(TARGET).vpk

%.vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin $(LIVEAREA) $@

eboot.bin: $(TARGET).velf
	vita-make-fself -s $< $@

%.velf: %.elf
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CFLAGS) $^ $(LIBS) -o $@

%.o: %.cpp
	${CXX}  -o $@ $< $(CXXFLAGS)

%.o: %.png
	$(PREFIX)-ld -r -b binary -o $@ $^

clean:
	@rm -rf $(TARGET).vpk $(TARGET).velf $(TARGET).elf $(OBJS) \
		eboot.bin param.sfo

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:mai/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
