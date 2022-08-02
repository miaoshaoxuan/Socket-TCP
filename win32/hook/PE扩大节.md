# PE扩大节

一般只在最后的节进行扩大节，因为在之前的节进行扩大的话，会造成后边的节都要向前移动，稍有不注意就很可能会影响PE文件

步骤：

1. 先分配一块空间大小为S

2. 将最后一个节的 SizeOfRawData 和 VirtualSize 改成（N）最终的大小

   N =（SizeOfRawData或者VirtualSize 内存对齐后的值 ） + S

3. 修改SizeOfImage大小



打开010Editor->edit->insert->Insert Byte，增加0x1000个字节

![1659432212507](.\asset\1659432212507.png)

修改sizeOfRawData+1000H

![1659432693474](C:\Users\Miaoshaoxuan\Desktop\asset\1659432693474.png)

修改为

![1659432742144](.\asset\1659432742144.png)

修改sizeofImage+1000H

![1659432982768](.\asset\1659432982768.png)

![1659433002515](.\asset\1659433002515.png)



注意按照内存方式对齐去修改，都是SectionAlignment的整数倍

