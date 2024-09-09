CURRENT_PATH = $(shell pwd)
FINAL_OUTPUT = $(CURRENT_PATH)/arch.ko

include $(CURRENT_PATH)/toolchain.inc

KO_FILES = boot/boot.ko impl/impl.ko

default:
	$(MAKE) $(FINAL_OUTPUT)

clean:
	@if [ -f $(FINAL_OUTPUT) ]; then rm $(FINAL_OUTPUT); fi
	$(MAKE) -C ./boot/ clean
	$(MAKE) -C ./impl/ clean

$(FINAL_OUTPUT): $(KO_FILES)
	$(LD) -r $(KO_FILES) -o $(FINAL_OUTPUT)
	
boot/boot.ko:
	$(MAKE) -C ./boot/ default
	
impl/impl.ko:
	$(MAKE) -C ./impl/ default