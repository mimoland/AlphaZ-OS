
; FAT12 磁盘的头
BS_OEMName	db 'AlphaOS '	; OEM String, 必须 8 个字节

BPB_BytsPerSec	dw 512		; 每扇区字节数
BPB_SecPerClus	db 1		; 每簇多少扇区
BPB_RsvdSecCnt	dw 1		; Boot 记录占用多少扇区
BPB_NumFATs	db 2		; 共有多少 FAT 表
BPB_RootEntCnt	dw 224		; 根目录文件数最大值
BPB_TotSec16	dw 2880		; 逻辑扇区总数
BPB_Media	db 0xF0		; 媒体描述符
BPB_FATSz16	dw 9		; 每FAT扇区数
BPB_SecPerTrk	dw 18		; 每磁道扇区数
BPB_NumHeads	dw 2		; 磁头数(面数)
BPB_HiddSec	dd 0		; 隐藏扇区数
BPB_TotSec32	dd 0		; 如果 wTotalSectorCount 是 0 由这个值记录扇区数

BS_DrvNum	db 0		; 中断 13 的驱动器号
BS_Reserved1	db 0		; 未使用
BS_BootSig	db 29h		; 扩展引导标记 (29h)
BS_VolID	dd 0		; 卷序列号
BS_VolLab	db 'AlphaOS    '; 卷标, 必须 11 个字节
BS_FileSysType	db 'FAT12   '	; 文件系统类型, 必须 8个字节


; FAT表的大小
FATSize			equ	9

; 根目录占用的空间
RootDirSectors		equ	14

; 根目录的第一个扇区号
SectorNoOfRootDirectory	equ	19

; FAT1 的第一个扇区号，由于FAT2和FAT1内容一致，此处不过多定义
SectorNoOfFAT1		equ	1

; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
; 文件的开始Sector号 = DirEntry中的开始Sector号 + 根目录占用Sector数目
;                      + DeltaSectorNo
DeltaSectorNo		equ	17
