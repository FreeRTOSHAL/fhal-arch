obj-$(CONFIG_ARCH_ARM)+=arm/
obj-$(CONFIG_ARCH_X86)+=x86/
obj-$(CONFIG_ARCH_C2000)+=c2000/
obj-$(CONFIG_ARCH_RISCV)+=riscv/

obj-$(CONFIG_GEN_VERSION) += version.o
BUILDID := $(USER)@$(shell hostname) $(shell date +'%Y-%m-%d %H:%M:%S')
$(obj)/version.c: FORCE
	@echo "  GEN     $(obj)/version.c"
	$(Q)version="const char *versionArch = \""; \
	dir=`pwd`; \
	cd `dirname $@`; \
	version=$$version`git log -n 1 --pretty="format:$(BUILDID) %H" --no-notes`; \
	if git diff-index --name-only HEAD | grep -qv "^scripts/package" > /dev/null; then \
		version="$$version-dirty"; \
	fi; \
	version="$$version\";"; \
	cd $$dir; \
	echo $$version > $@;
