#ifndef VISUALBOYADVANCE_H
#define VISUALBOYADVANCE_H
#include <windows.h>
#include "pokemon.h"

class VisualBoyAdvance
{
public:
    DWORD		m_pid;										//进程pid
    HWND		m_hWnd;										//窗口句柄
    HANDLE		m_hProcess;									//进程句柄
    const u32 baseAddress = 0x005A8F50;                   //究极绿宝石的基址
    const u32 ROMAddress = 0x005A8F58;                    //究极绿宝石POM区的基址

public:
    //构造
    VisualBoyAdvance();

    //析构
    ~VisualBoyAdvance();

    //检测句柄是否有效
    BOOL IsValid();

    //附加vba进程
    BOOL Attach();

    //找到并打开hWnd对应的进程，成功返回TRUE
    BOOL AttachByWindowHandle(HWND hWnd);

    //读取偏移后的内容，返回读取到的值，读取1个字节
    u8 ReadValue_1(u32 offset,u32 baseAddr = 0x005A8F50);

    //读取偏移后的内容，返回读取到的值，读取2个字节
    u16 ReadValue_2(u32 offset,u32 baseAddr = 0x005A8F50);

    //读取偏移后的内容，返回读取到的值，读取4个字节
    u32 ReadValue_4(u32 offset,u32 baseAddr = 0x005A8F50);

    u16 PtrRead_2(u32 offset,u32 baseAddr = 0x03005D90);

    u16 PtrReadXor_2(u32 offset,u32 baseAddr = 0x03005D90);

    u32 PtrRead_4(u32 offset,u32 baseAddr = 0x03005D90);

    u32 PtrReadXor_4(u32 offset,u32 baseAddr = 0x03005D90);

    BOOL PtrWrite_2(u32 offset,u16 *value);

    BOOL PtrWriteXor_2(u32 offset,u16 *value);

    BOOL PtrWrite_4(u32 offset,u32 *value);

    BOOL PtrWriteXor_4(u32 offset,u32 *value);

    BOOL WriteValue_1(u32 offset,u8 *value);

    BOOL WriteValue_2(u32 offset,u16 *value);

    BOOL WriteValue_4(u32 offset,u32 *value);
};

#endif // VISUALBOYADVANCE_H
