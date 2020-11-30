obj-y += port.o
obj-y += portASM.o

$(obj)/%.o: $(src)/%.asm FORCE
	$(call if_changed,as_o_S)
