/*
 * GCC linker script for STM32 (ARM Cortex-M).
 */

/* 定义memory，根据实际设备调整起始地址和大小 */
MEMORY
{
    flash (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/*
 * The stack starts at the end of RAM and grows downwards. Full-descending
 * stack; decrement first, then store.
 */
/* 根据memory计算栈顶，因为ARM Cortex-M是满减栈，所以是ram最高地址 */
_estack = ORIGIN(ram) + LENGTH(ram);
/* 输出段 */
SECTIONS
{
    /* 最开始是异常向量，其中有栈顶和入口点等 */
    .isr_vector :
    {
        __isr_vector_start__ = .;
        KEEP(*(.isr_vector)) /* 使用'KEEP'是为了防止链接器发现没人直接使用本段而不链接 */
        ASSERT(. != __isr_vector_start__, "The .isr_vector section is empty");
    } >flash


    /* Text section (code and read-only data) */
    .text :
    {
	/* 4字节对齐地址 */
        . = ALIGN(4);
        _stext = .; /* 当前地址赋值给 _xtext 变量*/
        *(.text*)   /* code ,所有 输入段为".text*"的段都被链接到这里  */
        *(.rodata*) /* read only data */
        . = ALIGN(4);
        _etext = .;
    } >flash



    /* 已经初始化的 data section. 启动代码将本段从FLASH 拷贝到 RAM */
    _sidata = .;
    .data : AT(_sidata) /* 链接到FLASH 的地址 _sidata，但需要拷贝的RAM中运行 */
    {
        . = ALIGN(4);
        _sdata = .; /* 这里就是RAM中的偏移了，从0x20000000开始 */
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >ram /* 最终运行在ram中 */


    /* 未初始化的 data section. 启动代码统一置0 */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >ram

    /* heap 在bss后面开始 
    */
    ._heap :
    {
    . = ALIGN(32); 
    _heap_base = .;
/*    . = . + conf_HEAP_SIZE;*/
    } >ram
}

