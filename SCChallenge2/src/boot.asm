global start                
extern kernel_main          

; Constants for the Multiboot header
ALIGN_MODULES    equ 1 << 0   ; Request memory modules to be aligned
MEMORY_INFO      equ 1 << 1   ; Request memory information from the bootloader

FLAGS            equ ALIGN_MODULES | MEMORY_INFO
MULTIBOOT_MAGIC  equ 0x1BADB002
CHECKSUM         equ -(MULTIBOOT_MAGIC + FLAGS)

section .text
align 4

; Multiboot header (required for GRUB or compatible bootloaders)
MultibootHeader:
    dd MULTIBOOT_MAGIC       ; Magic number to identify Multiboot-compliant bootloader
    dd FLAGS                 ; Flags indicating features we require
    dd CHECKSUM              ; Checksum to validate the header

start:
    call kernel_main          ; Jump to C code to start the kernel
    cli                      ; Disable interrupts (just for safety)

halt:
    hlt                      ; Halt the CPU (low-power mode)
    jmp halt                 ; Infinite loop to keep the CPU halted