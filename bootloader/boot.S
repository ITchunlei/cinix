    .code16
    .section .text
    .global _start
_start:
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    movw $_start, %sp

    call disp_str

    call read_bs

    ljmp $0x0, $0x9100

read_bs:
	push %es
    movw $0x0201, %ax
    /** 读一个扇区 **/
    movb $0x2, %al
    movb $0x0, %ch
    /** 0-5 起始扇区 6-7 磁道**/
    movb $0x02, %cl
    /** dl 驱动器号 dh 磁头号 **/
    movw $0x0, %dx
    /** es:bx 数据缓冲区地址 **/
	movw $0x0, %bx
	movw %bx, %es
    movw $0x9100, %bx
    int $0x13
	pop %es
    ret


disp_str:
    movw $msg, %ax
    movw %ax, %bp
    movw msg_len, %cx
    movw $0x1301, %ax
    movw $0xc, %bx
    movb $0x0, %dl
    int $0x10
    ret
msg:
    .ascii "Booting..."
msg_len:
    .int msg_len - msg
    .org 510
    .word 0xAA55

