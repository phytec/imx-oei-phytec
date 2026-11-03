OBJS += $(OUT)/lpddr5_timing_8gb.o \
	$(OUT)/lpddr5_timing.o

# 4GB timings are selected at runtime by the SOM detection code only
ifeq ($(PHYTEC_SOM_DETECTION), 1)
OBJS += $(OUT)/lpddr5_timing_4gb.o
endif

VPATH += boards/mx95libra/ddr
