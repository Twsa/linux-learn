[toc] 

# Buildroot 用户手册

​	Buildroot 2019.11-rc3手册，由git版本584312d16f于2019-11-28 23:17:07 UTC生成

Buildroot手册由Buildroot开发人员编写。 它是根据GNU通用公共许可证版本2许可的。有关此许可证的全文，请参阅Buildroot源中的COPYING文件。

版权所有©2004-2019 Buildroot开发人员

![1](pics/1.png)

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

- 推荐依赖项

  Buildroot中的一些功能和实用的软件，像legal-info(法律信息),或者是图表生成工具需要其他的依赖项，尽管对于简单构建这些不是强制性的，但是它们仍然强烈推荐安装。

  - `python` (version 2.7 or any later) 

- 配置接口用的依赖项

  对于这些库，你需要同时安装运行环境和开发数据包，它们在许多发行版本是分开打包的，开发数据包（development packages）通常以-devel或者-devel为后缀命名。

  -  `ncurses5` 调用 menuconfig 接口
  -  `qt5` 调用 xconfig 的接口
  -  `glib2`, `gtk2` and `glade2`调用 gconfig接口

- 源获取工具

  在官方树中，大多数软件包源都是使用wget从ftp，http或https位置渠道取到的，少数的包只有通过版本控制系统才能得到，此外，Buildroot能够通过其他工具下载资源，像`rsyn`或者`scp`（详情请见第[十九章 下载基础结构]()），如果你想通过其中的任何一种获取软件包，你需要在主机系统中安装对应的工具。

  -  `bazaar` 
  -  `cvs` 
  -  `git` 
  -  `mercurial` 
  -  `rsync` 
  -  `scp` 
  -  `subversion` 

- Java相关的包，如果目标系统需要Ｊava类路径
  -  The `javac` compiler (编译器)
  -  The `jar` tool 

- 文档生成工具：
  -  `asciidoc`, version 8.6.3 or higher 
  -  `w3m` 
  -  `python` with the `argparse` module (automatically present in 2.7+ and 3.2+) 
  -  `dblatex` (required for the pdf manual only) 

- 图表生成工具
  -  `graphviz` to use *graph-depends* and *-graph-depends* 
  -  `python-matplotlib` to use *graph-build* 

## 第三章 获取Buildroot

​	每三个月发布一次Ｂuildroot的版本，分别是2月，5月和11月,发布的版本的序列号的格式是YYY,ＭＭ，所以比如2013.02,2014.08。

​	