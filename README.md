# AlphaZ-OS

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

AlphaZ是一个简易的抢占式多任务操作系统。

## 内容列表

- [背景](#背景)
- [安装](#安装)
- [维护者](#维护者)
- [如何贡献](#如何贡献)
- [使用许可](#使用许可)

## 背景

AlphaZ最开始是因为我偶然接触到的[于渊](https://yuyuan.org/)老师的一本关于自制操作系统的书，外加上自己对平时所学的知识都停留在表面，所以想自己动手制作一个简易的操作系统加以实践。

该项目的目的并不是为了要制作一个可用的操作系统，而是为了能够实践停留在书本上的那些知识，所以为了简单起见，当前系统使用的硬件比较过时。未来会不断地对其进行完善，争取与时俱进。另外，AlphaZ也参考了Minix、Linux等操作系统的诸多优秀设计。

## 安装

本项目使用[bochs](http://bochs.sourceforge.net/)、[gcc-4.8](https://gcc.gnu.org/)和[nasm](https://www.nasm.us/)，请确保你本地安装了它们。

```
$ bximage # 在根目录创建一个虚拟软盘，并命名为a.img
$ make image
$ bochs
```

有关开发环境的详细配置说明请参考[这里](https://mingliangliu.com/2019/12/12/ck5tuae0t0002wktmqym44f9g/#more)。另外，你也可以使用[qemu](https://www.qemu.org/)或[VirtualBox](https://www.virtualbox.org/)来运行该项目。

## 维护者

[@lml256](https://github.com/lml256)

## 如何贡献

非常欢迎你的加入！你可以提交一个[Issue](https://github.com/lml256/AlphaZ-OS/issues)或者提交一个[Pull request](https://github.com/lml256/AlphaZ-OS/pulls)

有关如何提交Pull Request可以参考[这里](https://mingliangliu.com/2020/01/24/ck5tuae100006wktmha916451/#more)

## 使用许可

[GPL-3.0](LICENSE) © lml
