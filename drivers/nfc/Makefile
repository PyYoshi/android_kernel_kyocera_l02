#
# drivers/nfc/Kconfig
#
# This software is contributed or developed by KYOCERA Corporation.
# (C) 2013 KYOCERA Corporation
#

#
# Makefile for nfc devices
#

obj-$(CONFIG_PN544_NFC)		+= pn544.o
obj-$(CONFIG_NFC_PN533)		+= pn533.o
obj-$(CONFIG_NFCFELICA)		+= nfcfelica/
obj-$(CONFIG_NFC_WILINK)	+= nfcwilink.o
obj-$(CONFIG_BCM2079X)		+= bcm2079x-i2c.o

ccflags-$(CONFIG_NFC_DEBUG) := -DDEBUG
