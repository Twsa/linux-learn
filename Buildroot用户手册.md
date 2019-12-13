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

​	发布的压缩包能够在http://buildroot.org/downloads/找到

​	为了你方便地地快速上手，在Buildroot源码的目录树下有一个Vagrantfile文件，它能够快速地配置一个拥有Buildroot的依赖的软件的虚拟机。

​	如果你想要在Linux或者Mac Os X 拥有一个单独的Buildroot的环境，复制下面一行到你的终端中执行：

```html
curl -O https://buildroot.org/downloads/Vagrantfile; vagrant up
```

​	如果你是在Windows上玩，复制下面到你的powershell:

```html
(new-object System.Net.WebClient).DownloadFile(
"https://buildroot.org/downloads/Vagrantfile","Vagrantfile");
vagrant up
```

​	如果要关注开发，可以使用每日快照或克隆Git存储库。 有关更多详细信息，请参考Buildroot网站的“ [Download page](http://buildroot.org/download) ”页面。

## 第四章 Buildroot 快速入门

**注意**：你应该在普通用户下构建所有东西。你没有必要通过获取root权限来配置和使用Buildroot。在普通用户下运行所有的命令可以防止系统阻止编译和安装的过程。

​	使用Buildroot的第一步就是创建一个配置文件。Buildroot有非常好用的配置工具，它同linux内核或者buxybox工具的配置工具非常相似。

在buildroot目录下，运行

```html
 $ make menuconfig
```

这个是用基于原始的光标来配置

```html
 $ make nconfig
```

这个是用新的光标来配置

```html
 $ make xconfig
```

这个是用基于QT来配置

```html
 $ make gconfig
```

这个是基于GTK来配置

​	以上的所有的“make”命令将会构建一个配置buildroot的工具（包括接口），所以你也许需要为你的配置工具所依赖的库安装“devl”包。参考[Chapter 2, *System requirements*](http://nightly.buildroot.org/#requirement)查看详情，特别是关于获取你需要的软件的依赖包这一节[optional requirements](http://nightly.buildroot.org/#requirement-optional) [Section 2.2, “Optional packages”](http://nightly.buildroot.org/#requirement-optional)

​	对于配置工具的每个菜单选项，你都能找到帮助和相关描述，参考[Chapter 6, *Buildroot configuration*](http://nightly.buildroot.org/#configure) 了解一些特殊配置方面的相关细节。

​	一旦所有的配置选项都配置完毕，配置工具就会生成一个.config文件，这个文件包含所有的配置信息。这个配置文件将会被顶层的Makefile文件读取。

​	开始构建只需要运行：

```html
 $ make
```

​	你决不能用make -jN来构建Buildroot：因为顶层目录的make当前不支持。而是使用BR2_JLEVEL选项告诉Buildroot使用make -jN运行每个软件包的编译。

 	make命令会按照下面的步骤执行：

- 下载源文件（如果需要的话）；
- 配置，构建和安装交叉编译工具链，或者只是导入外部工具链；

- 配置，构建和安装目标包；
- 构建一个内核镜像（如果你有选择）
- 构建一个bootloader镜像（如果你有选择）；
- 按你所选择的格式创建跟文件系统。

Buildroot 的make之后的输出是存放在一个单独的目录中，<font color=#0000FF >output/</font>。这个目录包含以下几个目录：

- <font color=#0000FF >images/</font> 目录存放了所有的镜像文件(内核镜像，bootloader和根文件系统镜像)，这些就是你需要放进目标系统的镜像文件。

- <font color=#0000FF >build/</font>  目录下存放着所有已经构建的组件（这些组件包括在主机系统中的Buildroot工具和为目标系统编译生成的包）。每一个组件在该目录下都有一个子目录。
- <font color=#0000FF >host/</font> 目录包含了已经构建的主机需要的工具和目标系统的工具链的sysroot。前者是为了能够正确执行Buildroot而为主机系统安装一些编译好的工具，包括交叉编译工具链。后者是类似于跟文件系统层次结构的层次结构。它包含提供给用户的软件包的头文件和一些库，并且可以通过已经在使用的软件包来安装这些库。不得不说的是，这个目录并不是目标系统的根文件目录：它包含对于嵌入式系统来说非常庞大的的一些开发文件，未压缩的二进制文件和一些库文件。这些开发文件被用来为目标系统编译一些库和应用程序。
- <font color=#0000FF >staging/</font>  是/host目录中的目标系统的sysroot的符号链接，这样做是为了向后兼容。
- <font color=#0000FF >target/</font> 这个目录几乎包含了完整的目标系统的完整的根文件系统：除了/dev目录下一些设备驱动的相关文件（Buildroot 不能创建这些文件是因为Buildroot不能以root权限运行，我们也不希望在root用户下运行）外，其他的文件系统该有的这个目录都有了。也就是说Buildroot没有正确的权限（比如为busybox的二进制文件setuid）。因此，这个目录还不能直接用在你的目标系统中去，相反，正确的操作是将images/目录的镜像烧写到你的板子中去。如果你想要通过NFS挂载根文件系统而需要一个解压过的根文件系统的镜像，你可以通过以root权限解压images/下的压缩的根文件镜像。相比较于staging/`, `target/目录仅仅包含目标程序运行所需要的文件和库：没有开发文件，二进制文件被压缩。make menuconfig|nconfig|gconfig|xconfig和make这些命令可以简单快速的生成满足你的需求的镜像，它包含你所选择的所有u的应用程序和功能。

更多关于"make"命令的详情请参考[Section 8.1, “*make* tips”](http://nightly.buildroot.org/#make-tips).

## 第五章 社区资源

像所有的开源项目一样，Buildroot在社区内或之外有许多方式共享资源和分享最新资讯的方式。

如果你在想搞懂Buildroot或者对Buildroot项目做些贡献的时候需要一些帮助，可能下面的其中的一种途径可以帮助到你。

<font color=#0000FF >Mailing List（邮件列表）</font>

​		Buildroot有一个讨论和开发的邮件列表,这是Buildroot用户和开发者们沟通交互的主要方式。只有Buildroot的邮件列表的订阅者才能在这个列表中发邮件。你能通过这个网址 [mailing list info page](http://lists.buildroot.org/mailman/listinfo/buildroot)来订阅。

​	发送到邮件列表的的邮件也可以在[gmane.comp.lib.uclibc.buildroot](http://dir.gmane.org/gmane.comp.lib.uclibc.buildroot)的 [mailing list archives](http://lists.buildroot.org/pipermail/buildroot)并通过[Gmane](http://gmane.org)查看到。在提问之前请查看邮件列表的归档，因为有可能在之前已经有人问过同样的问题。

<font color=#0000FF >IRC（因特网中继聊天（Internet Relay Chat））</font>

​	Buildroot的IRC频道[#buildroot](irc://freenode.net/#buildroot) 托管在 [Freenode](http://webchat.freenode.net)上，这是一个快速提问或者围绕这特定的主题进行讨论的好地方。

​	当你在IRC上面寻求帮助的时候，请用代码共享网站（比如：http://code.bulix.org.）挂上你的问题的相关的日志或者是代码的片段。

​	请注意，对于特定的问题，也许发邮件到邮件列表会是一个更好的选择，因为在邮件列表中能接触到更多的人，包括用户和开发者。

<font color=#0000FF >Bug tracker (错误追踪器)</font>

​	Buildroot中的bug可以通过邮件列表或者通过[Buildroot bugtracker](https://bugs.buildroot.org/buglist.cgi?product=buildroot)来上报。请在创建bug报告之前参阅

[Section 21.6, “Reporting issues/bugs or getting help”](http://nightly.buildroot.org/#reporting-bugs)

<font color=#0000FF >Wiki</font>

 Buildroot的wiki是托管在[eLinux](http://elinux.org)wiki。它包含一些有用的链接，能够纵观Buildroot的的历史和未来的计划。

<font color=#0000FF >PatchWork</font>

​	patchwork是一个基于Web的补丁跟踪系统，旨在促进对开源项目的贡献和贡献的管理。已发送的补丁将会被系统捕获并出现在web页面上。任何发表的引用该补丁的评论也将被附加到补丁的页面上去。更多关于Pathwork请参阅http://jk.ozlabs.org/projects/patchwork/

​	Buildroot的Patchwork网站主要是Buildroot的维护者确保不会漏掉每一个补丁。补丁的审查者同样也在使用它（参阅：[Section 21.3.1, “Applying Patches from Patchwork”](http://nightly.buildroot.org/#apply-patches-patchwork)）

​	但是，由于该网站在简洁明了的Web接口中公开了补丁及其相应的评论，因此它对所有Buildroot开发人员都是有用的。	

​	你能够找到Buildroot的补丁管理接口在http://patchwork.buildroot.org

# 第二卷 用户指导

## 第六章 Buildroot配置

所有的关于Buildroot的配置选项在make *config中有详细帮助描述。
make *config命令也提供了一个搜索的工具，在不同的前置菜单栏阅读帮助信息就能知道怎么用了。

- 在menuconfig 中的搜索工具被称为/;
- 在xconfig中的搜索工具被称为Ctrl+f。

搜索结果会根据匹配检索内容的帮助信息。在menuconfig中，在最左边的一栏的数字就是快速进入选项的快捷键，只要敲打出对应的数字就能进入配置选项，如果由于缺少依赖项而无法选择条目，则转到包含菜单。尽管菜单结构和条目的帮助文本应该足够自解释，但是许多主题需要额外的解释，而这些解释不容易在帮助文本中涵盖，因此将在以下章节中介绍。 
### 6.1 交叉编译工具链
 交叉工具链就是能为你的系统编译代码的工具集，它包括编译器（比如，gcc）,二进制工具比如汇编器和连接器（比如，binutils）和C语言标准库（比如GNU Libc, uClibc-ng），你的开发平台已经安装了一套编译工具链，这套编译工具链能编译在你的开发平台跑的应用。如果你用的是PC机，你的编译工具链将在X86架构的处理器上运行，并且生成X86处理器上能运行的程序。在绝大多数的Linux操作系统中的编译工具链用GNU libc(glibc)作为C标准库，这种编译工具链被称为宿主机编译工具链。你正在工作的，运行着这种工具链的机子被称为“宿主机”。
  
正如上面所述，你的系统随附的编译工具链在你的宿主机中的处理器上运行并为其生成可运行的程序。如果你的嵌入式系统有着不同于宿主机的处理器，那么就需要一种交叉编译工具链——一种在宿主机上可运行，但可以为你的目标系统或目标处理器生成可运行程序的编译工具链。例如，如果主机系统使用x86，而目标系统使用ARM，则主机上的常规编译工具链在x86上运行并为x86生成代码，而交叉编译工具链在x86上运行并为ARM生成代码。

Buildroot为交叉编译工具链的配置提供两种解决方案：

- **内部工具链后端**，在配置界面中称为Buildroot工具链。
- **外部工具链后端**，在配置界面中称为外部工具链。

可以使用“工具链”菜单中的“工具链类型”选项来对这两种解决方案进行选择。一旦选择一种解决方案后，将出现许多配置选项，以下各节将详细介绍它们。
#### 6.1.1 内部工具链后端
#### 6.1.2 外部工具链后端
#### 6.1.3用Buildroot编译一个外部工具链
### 6.2. /dev 管理
在Linux系统中，/dev路径中包含着一些特殊的文件，这些文件被称为设备文件，通过这些文件允许用户程序访问Linux内核管理的硬件资源。