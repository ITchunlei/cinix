//
//  klib.h
//  Hobby
//
//  Created by chunlei on 2018/10/3.
//  Copyright © 2018 los. All rights reserved.
//

#ifndef __LIB_H__
#define __LIB_H__

#include <stdarg.h>

#define load_gdt(gdt_ptr) __asm__ __volatile__ ("lgdt %0" : : "m" (gdt_ptr))

#define load_idt(idt_ptr) __asm__ __volatile__ ("lidt %0" : : "m" (idt_ptr))

#define close_irq() __asm__ __volatile__("cli")

#define open_irq() __asm__ __volatile__ ("sti")

#define out_byte(P,V) __asm__ __volatile__ ("outb %%al,	%%dx" : :"a" (V), "d" (P))

#define in_byte() __asm__ __volatile__ ()

void kputc(char c);

int kprintf(const char* format, ...);

int vsprintf(char* buf, const char *format, va_list args);

/*
 From => To memory copy Num bytes
 */

inline void * memcpy(void *From,void * To,long Num)
{
    int d0,d1,d2;
    __asm__ __volatile__    (    "cld    \n\t"
                             "rep    \n\t"
                             "movsq    \n\t"
                             "testb    $4,%b4    \n\t"
                             "je    1f    \n\t"
                             "movsl    \n\t"
                             "1:\ttestb    $2,%b4    \n\t"
                             "je    2f    \n\t"
                             "movsw    \n\t"
                             "2:\ttestb    $1,%b4    \n\t"
                             "je    3f    \n\t"
                             "movsb    \n\t"
                             "3:    \n\t"
                             :"=&c"(d0),"=&D"(d1),"=&S"(d2)
                             :"0"(Num/8),"q"(Num),"1"(To),"2"(From)
                             :"memory"
                             );
    return To;
}

/*
 FirstPart = SecondPart        =>     0
 FirstPart > SecondPart        =>     1
 FirstPart < SecondPart        =>    -1
 */

inline int memcmp(void * FirstPart,void * SecondPart,long Count)
{
    register int __res;
    
    __asm__    __volatile__    (    "cld    \n\t"        //clean direct
                                "repe    \n\t"        //repeat if equal
                                "cmpsb    \n\t"
                                "je    1f    \n\t"
                                "movl    $1,    %%eax    \n\t"
                                "jl    1f    \n\t"
                                "negl    %%eax    \n\t"
                                "1:    \n\t"
                                :"=a"(__res)
                                :"0"(0),"D"(FirstPart),"S"(SecondPart),"c"(Count)
                                :
                                );
    return __res;
}

/*
 set memory at Address with C ,number is Count
 */

inline void * memset(void * Address,unsigned char C,long Count)
{
    int d0,d1;
    unsigned long tmp = C * 0x0101010101010101UL;
    __asm__    __volatile__    (    "cld    \n\t"
                                "rep    \n\t"
                                "stosq    \n\t"
                                "testb    $4, %b3    \n\t"
                                "je    1f    \n\t"
                                "stosl    \n\t"
                                "1:\ttestb    $2, %b3    \n\t"
                                "je    2f\n\t"
                                "stosw    \n\t"
                                "2:\ttestb    $1, %b3    \n\t"
                                "je    3f    \n\t"
                                "stosb    \n\t"
                                "3:    \n\t"
                                :"=&c"(d0),"=&D"(d1)
                                :"a"(tmp),"q"(Count),"0"(Count/8),"1"(Address)
                                :"memory"
                                );
    return Address;
}

/*
 string copy
 */

inline char * strcpy(char * Dest,char * Src)
{
    __asm__    __volatile__    (    "cld    \n\t"
                                "1:    \n\t"
                                "lodsb    \n\t"
                                "stosb    \n\t"
                                "testb    %%al,    %%al    \n\t"
                                "jne    1b    \n\t"
                                :
                                :"S"(Src),"D"(Dest)
                                :"ax","memory"
                                );
    return     Dest;
}

/*
 string copy number bytes
 */

inline char * strncpy(char * Dest,char * Src,long Count)
{
    __asm__    __volatile__    (    "cld    \n\t"
                                "1:    \n\t"
                                "decq    %2    \n\t"
                                "js    2f    \n\t"
                                "lodsb    \n\t"
                                "stosb    \n\t"
                                "testb    %%al,    %%al    \n\t"
                                "jne    1b    \n\t"
                                "rep    \n\t"
                                "stosb    \n\t"
                                "2:    \n\t"
                                :
                                :"S"(Src),"D"(Dest),"c"(Count)
                                :"ax","memory"
                                );
    return Dest;
}

/*
 string cat Dest + Src
 */

inline char * strcat(char * Dest,char * Src)
{
    __asm__    __volatile__    (    "cld    \n\t"
                                "repne    \n\t"
                                "scasb    \n\t"
                                "decq    %1    \n\t"
                                "1:    \n\t"
                                "lodsb    \n\t"
                                "stosb    \n\r"
                                "testb    %%al,    %%al    \n\t"
                                "jne    1b    \n\t"
                                :
                                :"S"(Src),"D"(Dest),"a"(0),"c"(0xffffffff)
                                :"memory"
                                );
    return Dest;
}

/*
 string compare FirstPart and SecondPart
 FirstPart = SecondPart =>  0
 FirstPart > SecondPart =>  1
 FirstPart < SecondPart => -1
 */

inline int strcmp(char * FirstPart,char * SecondPart)
{
    register int __res;
    __asm__    __volatile__    (    "cld    \n\t"
                                "1:    \n\t"
                                "lodsb    \n\t"
                                "scasb    \n\t"
                                "jne    2f    \n\t"
                                "testb    %%al,    %%al    \n\t"
                                "jne    1b    \n\t"
                                "xorl    %%eax,    %%eax    \n\t"
                                "jmp    3f    \n\t"
                                "2:    \n\t"
                                "movl    $1,    %%eax    \n\t"
                                "jl    3f    \n\t"
                                "negl    %%eax    \n\t"
                                "3:    \n\t"
                                :"=a"(__res)
                                :"D"(FirstPart),"S"(SecondPart)
                                :
                                );
    return __res;
}

/*
 string compare FirstPart and SecondPart with Count Bytes
 FirstPart = SecondPart =>  0
 FirstPart > SecondPart =>  1
 FirstPart < SecondPart => -1
 */

inline int strncmp(char * FirstPart,char * SecondPart,long Count)
{
    register int __res;
    __asm__    __volatile__    (    "cld    \n\t"
                                "1:    \n\t"
                                "decq    %3    \n\t"
                                "js    2f    \n\t"
                                "lodsb    \n\t"
                                "scasb    \n\t"
                                "jne    3f    \n\t"
                                "testb    %%al,    %%al    \n\t"
                                "jne    1b    \n\t"
                                "2:    \n\t"
                                "xorl    %%eax,    %%eax    \n\t"
                                "jmp    4f    \n\t"
                                "3:    \n\t"
                                "movl    $1,    %%eax    \n\t"
                                "jl    4f    \n\t"
                                "negl    %%eax    \n\t"
                                "4:    \n\t"
                                :"=a"(__res)
                                :"D"(FirstPart),"S"(SecondPart),"c"(Count)
                                :
                                );
    return __res;
}

static inline int strlen(char * String)
{
    register int __res;
    __asm__    __volatile__    (    "cld    \n\t"
                                "repne    \n\t"
                                "scasb    \n\t"
                                "notl    %0    \n\t"
                                "decl    %0    \n\t"
                                :"=c"(__res)
                                :"D"(String),"a"(0),"0"(0xffffffff)
                                :
                                );
    return __res;
}

#endif /* __LIB_H__ */
