*XHOS是一个专门为了学习OS而生的，通过实际编写代码测试会对OS有更深入的理解。

选择stm32f103c8t6的低成本单片机
linux 环境一步步写出XHOS

更多信息请访问网页：findxiaohei.xyz


1. step_by_step 是一步步编写XHOS的过程，可按顺序参考
2. uart_tool.c 为接收串口输出的助手，可以使用  gcc uart_tool.c -o uart_tool, 编译产生uart_tool
使用方法： sudo ./uart_tool /dev/ttyUSB0 115200
3. xhos是最终版，现在还不完善，欢迎提交PR完善它。
