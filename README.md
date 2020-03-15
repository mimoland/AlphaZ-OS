# AlphaZ-OS

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat)](https://github.com/RichardLitt/standard-readme)
![GitHub](https://img.shields.io/github/license/lml256/AlphaZ-OS)
![AlphaZ](https://img.shields.io/badge/Hey!-ALphaZ-brightgreen) ![GitHub last commit](https://img.shields.io/github/last-commit/lml256/AlphaZ-OS)

AlphaZ是一个简易的类Unix操作系统。

## 内容列表

- [背景](#背景)
- [安装](#安装)
- [运行](#运行)
- [维护者](#维护者)
- [如何贡献](#如何贡献)
- [使用许可](#使用许可)

## 背景

该项目起源于我偶然看到的一本有关自制操作系统的书，也为了能够对所学过的一些操作系统、微机原理等知识加以实践，遂想实现一个简单的操作系统。

## 安装

本项目使用了Linux系统进行开发。使用[bochs](http://bochs.sourceforge.net/)（可选）、[qemu](https://www.qemu.org/)、[gcc-4.8](https://gcc.gnu.org/)和[nasm](https://www.nasm.us/)，请确保你本地安装了它们。

首先将项目克隆到本地：

```
git clone git@github.com:lml256/AlphaZ-OS.git
```

下载开发所需的[虚拟硬盘文件](https://github.com/lml256/AlphaZ-OS/releases/download/dev0.01/alphaz.zip), 并将其解压到项目的根目录。

运行下面的命令进行编译：

```
$ make
$ make image
```

接下来可以使用下面命令的任何一个来在虚拟机中运行该系统：

```
$ bochs
或
$ make qemu
```

如果要调试该系统，请通过`make debug`命令来使用gdb进行调试。

## 运行

如果你仅仅是为了运行一下该系统，请直接下载[虚拟硬盘文件](https://github.com/lml256/AlphaZ-OS/releases/download/dev0.01/alphaz.zip)并解压。选择下面两个方式中你最喜欢的一个来运行：

一、 使用qemu运行系统：

```
qemu-system-i386 -hda alphaz.vhd -m 2G,slots=3,maxmem=4G
```

二、使用VirtualBox：

1. 新建一个虚拟机，类型选择其他。
2. 设置你喜欢的内存大小（请不要小于32M）
3. 选择使用下载的虚拟硬盘文件，然后创建即可

## 维护者

[@lml256](https://github.com/lml256)

## 如何贡献

非常欢迎你的加入！你可以提交一个[Issue](https://github.com/lml256/AlphaZ-OS/issues)或者提交一个[Pull request](https://github.com/lml256/AlphaZ-OS/pulls)

提交Commit时请遵循Angular 规范

## 使用许可

[GPL-3.0](LICENSE) © lml
