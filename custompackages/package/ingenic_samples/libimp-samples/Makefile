CONFIG_UCLIBC_BUILD=y

CROSS_COMPILE ?= $(TARGET_CROSS)

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
LDFLAG += -muclibc
endif

ifeq ($(CONFIG_UCLIBC_BUILD), y)
SDK_LIB_DIR = uclibc
else
SDK_LIB_DIR = ../../lib/glibc
endif

SDK_INC_DIR = include

INCLUDES = -I$(SDK_INC_DIR)

LIBS = $(SDK_LIB_DIR)/libalog.so $(SDK_LIB_DIR)/libimp.so

LDFLAG += -Wl,-gc-sections

SAMPLES = sample-Encoder-h264 \
	sample-Encoder-jpeg \
	sample-Encoder-h264-jpeg \
	sample-OSD \
	sample-Audio \
	sample-Setfps \
	sample-ISP-flip \
	sample-Decoder-jpeg \
	sample-Encoder-h264-IVS-move \
	sample-Change-Resolution \
	sample-Snap-Raw

.PHONY: printvars

printvars:
	@$(foreach V,$(sort $(.VARIABLES)),$(if $(filter-out environment% default automatic,$(origin $V)),$(warning $V=$($V) ($(value $V)))))

all: 	$(SAMPLES)

sample-Encoder-h264: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Encoder-h264.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-jpeg: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Encoder-jpeg.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-h264-jpeg: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Encoder-h264-jpeg.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-OSD: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-OSD.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Audio: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Audio.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Setfps: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Setfps.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-ISP-flip: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-ISP-flip.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Decoder-jpeg: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Decoder-jpeg.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-h264-IVS-move: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Encoder-h264-IVS-move.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Change-Resolution: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-Change-Resolution-Common.o sample-Change-Resolution.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-Raw: $(SDK_LIB_DIR)/libimp.so $(SDK_LIB_DIR)/libalog.so sample-common.o sample-Snap-Raw.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
