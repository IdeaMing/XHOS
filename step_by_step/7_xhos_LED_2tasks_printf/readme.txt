之前的代码比较乱，还是重构一下比较好



.
├── 3rdpart               // 第三方的组件
│   ├── CMSIS            
│   └── makefile.mk
├── app                   // 具体的应用用APP
│   ├── main.c
│   └── makefile.mk
├── Makefile              // 主makefile
├── readme.txt
└── xhos                  // OS 内核，当前目录下是和具体设备无关的代码
    ├── include
    │   ├── list.h
    │   ├── print.h
    │   ├── xhos_conf.h
    │   └── xhos.h
    ├── port              // 和具体设备相关的代码
    │   ├── port.c
    │   ├── port.h
    │   ├── start.s
    │   ├── xhos_mem.c
    │   └── xhos_stm32.ld
    ├── makefile.mk
    ├── printf.c
    └── xhos.c




约定：
1. OS内核，APP，第三方组建分开。其中OS内核中设备相关和设备无关的代码也分开。
2. 关于命名的约定
2.1 全局变量以 g_ + 驼峰命名。
2.2 OS内核中硬件相关代码以 port + 驼峰命名，并在xhos/port中实现。
2.3 OS提供给APP的API以 os + 驼峰命名。
2.4 OS和port层的接口 os_ + 驼峰命名（port层调用该API）。
2.5 配置信息以 conf_ + 驼峰命名, 在 xhos/include/xhos_conf.h



内存管理，可以吧空闲的内存串成链表，链表节点中记录该块内存的大小。申请后格式化，释放时加入链表。
可以根据大小分成多个链表。
