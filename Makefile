include Makefile.inc

TARGET = nofs
OBJ = boot/*.o \
	  init/*.o \
	  kernel/asm/*.o \
	  kernel/*.o


.PHONY: all
all :
	@cd boot && make
	@cd init && make
	@cd kernel/asm && make
	@cd kernel && make
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ)
	$(COPY) -O binary $(TARGET) $(TARGET).bin
	@echo "---   SIZE   ---"
	@$(SIZE) $(TARGET)


.PHONY: clean
clean :
ifdef DEPS
	 rm boot/*.o boot/*.d \
	 init/*.o init/*.d \
	 kernel/asm/*.o kernel/asm/*.d \
	 kernel/*.o kernel/*.d \
	 *.map *.bin $(TARGET)
else
	 rm boot/*.o  init/*.o \
	 kernel/asm/*.o  kernel/*.o \
	 *.map *.bin $(TARGET)
endif