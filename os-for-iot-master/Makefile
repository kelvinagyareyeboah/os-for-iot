CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -mcpu=cortex-a7 -Iinclude
LDFLAGS = -T linker.ld -nostdlib

SRCS = src/kernel/boot.S src/kernel/main.c src/drivers/gpio.c src/drivers/uart.c src/kernel/irq.c src/drivers/timer.c src/kernel/mem.c src/kernel/sched.c src/kernel/switch.S src/drivers/mailbox.c src/drivers/framebuffer.c src/drivers/dht11.c
OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.S=.o)

all: kernel7.img

kernel7.img: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel7.img

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/kernel/*.o src/drivers/*.o *.elf *.img

run: kernel.elf
	qemu-system-arm -M raspi2b -kernel kernel.elf -serial stdio -monitor none
