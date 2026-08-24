#include timings
include $(ROOT_DIR)/boards/$(SOM)/ddr/Makefile

COMPONENTS_DIR = $(ROOT_DIR)/components

ifeq ($(TEST),1)
    FLAGS += -DDDR_MEM_TEST
    OBJS  += $(OUT)/memtest.o
endif

ifeq ($(IEE),1)
    FLAGS += -DDDR_IEE
    OBJS  += $(OUT)/iee.o
endif

INCLUDE += -I$(COMPONENTS_DIR)/phytec_som_detection

OBJS += \
	$(OUT)/main.o \
	$(OUT)/ddr_init.o \
	$(OUT)/ddrphy_utils.o \
	$(OUT)/helper.o \
	$(OUT)/phytec_som_detection.o

OBJS  += \
	$(OUT)/ddrphy_qb.o \
	$(OUT)/ddrphy_train.o \
	$(OUT)/ddrphy_qb_gen.o \
	$(OUT)/romapi.o

ifeq ($(PHYTEC_SOM_DETECTION), 1)
FLAGS += -DPHYTEC_SOM_DETECTION
endif

VPATH += \
	oei/ddr \
	$(COMPONENTS_DIR)/phytec_som_detection

IMG += oei-$(cpu)-ddr

include $(ROOT_DIR)/oei/makefiles/gcc_cross.mak
