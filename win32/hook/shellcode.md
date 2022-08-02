# shellcode

关闭随机基址   4081 改为 0081

搞不懂为什么要在addressOfEntry加 C00 ？？？？？

入口地址
$$
原因：内存对齐 FOV = RVA - 节 virtualAdress + 节 PointToRawData
$$
名称位于 USER32, 条目 1038
 地址=76230BA0
 区段=.text
 类型=输出    (已知)
 名称=MessageBoxA

```asm
最终结果： 注意内存对齐文件对齐公式

//          &MessageBoxA - 新增代码入口地址 - 4个参数地址 - Call硬编码地址
messagebox计算：76230BA0 - 4105D0 - 8 - 5 = 75E2 05C3
//    原始入口地址 - 新增代码入口地址 - E8，E9内存 - 4个参数地址（新增添代码加的内存）
返回计算：401014 - 4105D0 - 5 - 5 - 8 = FFFF 0A32
```

