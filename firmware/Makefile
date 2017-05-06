
# Sensor Subsystem
ARC_FW = arc

# Application Processor
X86_FW = x86

all: arc_fw x86_fw

arc_fw:
	$(MAKE) -C $(ARC_FW) O=$(CURDIR)/outdir/$(ARC_FW)

x86_fw:
	$(MAKE) -C $(X86_FW) O=$(CURDIR)/outdir/$(X86_FW)

flash: arc x86
	$(MAKE) -C $(ARC_FW) O=outdir/$(ARC_FW) flash
	$(MAKE) -C $(X86_FW) O=outdir/$(APP) flash

pristine:
	@rm -rf $(CURDIR)/outdir