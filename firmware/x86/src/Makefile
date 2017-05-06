ccflags-y += ${PROJECTINCLUDE}
ccflags-y +=-I$(ZEPHYR_BASE)/include/drivers
ccflags-y +=-I$(ZEPHYR_BASE)/drivers -I$(ZEPHYR_BASE)/arch/x86

obj-y = main.o \
        ble.o \
        gatt/gap.o \
        gatt/dis.o \
        gatt/bas.o \
        gatt/hog.o
