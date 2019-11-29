# Buildroot 用户手册

​	Buildroot 2019.11-rc3手册，由git版本584312d16f于2019-11-28 23:17:07 UTC生成

Buildroot手册由Buildroot开发人员编写。 它是根据GNU通用公共许可证版本2许可的。有关此许可证的全文，请参阅Buildroot源中的COPYING文件。

版权所有©2004-2019 Buildroot开发人员

![1](/home/twsa/Desktop/my_test/pics/1.png)

# 第一卷 入门

## 第一章 Buildroot介绍

​	Buildroot是一个自动化以及简化利用交叉编译器构建嵌入式linux系统的流程的工具。

​	为了实现这个目标,Buildroot能够为你的目标板生成一种交叉编译器，一种根文件系统，一种linux内核镜像和一种bootloader。Buildroot能够使用任意的组合选项来构建系统（比如你能够使用已经存在的交叉编译器，只用Buildroot构建你的根文件系统）。

​	Buildroot对于嵌入式开发人员非常有用。嵌入式系统用到的处理器通常不是人们经常接触到的PC机里面的X86架构的处理器。嵌入式系统用到的处理器可以是PowerPC处理器，MIPS处理器，ARM处理器等等。

​	Buildroot支持很多处理器及变种；它还具有一些现成的可用的板级默认配置，除此之外，许多的第三方项目也是基于此进行开发的，或者是在Buildroot开发他们的BSP<sup>[1]</sup>或SDK<sup>[2]</sup>。

[1] BSP:板级支持包

[2] SDK:软件开发工具包

## 第二章 系统要求

​	Buildroot是为Linux定制的工具

​	尽管Buildroot会自行编译和构建大多数的宿主机软件包，但某些标准的Linux程序是要求被安装在宿主系统中。在下面是强制要求安装的和可选的软件的包（请注意，软件包名称在发行版之间可能有所不同）。

### 2.1. 强制性软件包 

- 构建工具
  -  `which` 
  -  `sed` 
  -  `make` (version 3.81 or any later) 
  -  `binutils` 
  -  `build-essential` (only for Debian based systems) 
  -  `gcc` (version 4.8 or any later) 
  -  `g++` (version 4.8 or any later) 
  -  `bash` 
  -  `patch` 
  -  `gzip` 
  -  `bzip2` 
  -  `perl` (version 5.8.7 or any later) 
  -  `tar` 
  -  `cpio` 
  -  `unzip` 
  -  `rsync` 
  -  `file` (must be in `/usr/bin/file`) 
  -  `bc` 

- 源获取工具
  - `wget`

### 2.2. 可选软件包

- 推荐依赖选项