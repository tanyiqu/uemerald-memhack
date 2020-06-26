#include "visualboyadvance.h"
#include "pokemon.h"
#include <QDebug>
#include <string>
using namespace std;

//TCHAR 转 string
string TCHARtoSTRING(TCHAR *STR)
{
    int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
    char* chRtn = new char[iLen*sizeof(char)];
    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
    string str(chRtn);
    return str;
}

BOOL CALLBACK EnumVBAWindow(HWND hWnd,LPARAM lParam)
{
    static CONST TCHAR Name[] = TEXT("VisualBoyAdvance");			//模式标题
    static CONST DWORD NameLength = sizeof(Name) / sizeof(Name[0]);	//模式标题的长度
    TCHAR windowName[NameLength];									//枚举得到的标题
    BOOL Continue = TRUE;											//是否继续枚举的标志
    GetWindowText(hWnd, windowName, NameLength);
    string str,s1("VisualBoyAdvance");		//VBA的标题
    str = TCHARtoSTRING(windowName);
    if (str == s1)
    {
        Continue = FALSE;
        *reinterpret_cast<HWND *>(lParam) = hWnd;
    }
    return Continue;
}

//构造
VisualBoyAdvance::VisualBoyAdvance() { }

//析构
VisualBoyAdvance::~VisualBoyAdvance() { }

//检测句柄是否有效
BOOL VisualBoyAdvance::IsValid()
{
    if (m_hProcess == NULL)//如果m_hProcess为NULL，返回FALSE
        return FALSE;
    if (IsWindow(m_hWnd) == FALSE)//窗口句柄标识的窗口不存在
    {
        if (m_hProcess != NULL)
            CloseHandle(m_hProcess);//关闭该进程
        m_hProcess = NULL;
        m_hWnd = NULL;
        return FALSE;
    }
    return TRUE;
}

//附加vba进程
BOOL VisualBoyAdvance::Attach()
{
    BOOL result = FALSE;
    HWND hWnd = NULL;
    //找到vba的窗口句柄
    result = EnumWindows(EnumVBAWindow,reinterpret_cast<LPARAM>(&hWnd));
    if (NULL == hWnd)
        result = FALSE;
    else
        result = AttachByWindowHandle(hWnd);//找到并打开hWnd相应的进程
    return result;
}

//找到并打开hWnd对应的进程，成功返回TRUE
BOOL VisualBoyAdvance::AttachByWindowHandle(HWND hWnd)
{
    if (IsWindow(hWnd))
    {
        m_hWnd = hWnd;
        GetWindowThreadProcessId(m_hWnd, &m_pid);						//获取vba进程的pid
        m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);		//打开vba进程
        if (NULL == m_hProcess)
            return FALSE;
    }
    return TRUE;
}

//读取偏移后的内容，返回读取到的值，读取1个字节
u8 VisualBoyAdvance::ReadValue_1(u32 offset,u32 baseAddr)
{
    u32 address;//临时存放地址的变量

    if (FALSE == ReadProcessMemory(m_hProcess, (LPVOID)baseAddr, &address, 4, NULL))//第一次读取
        return FALSE;

    address += offset;//计算真实地址

    u8 buff;

    ReadProcessMemory(m_hProcess, (LPVOID)address, &buff, 1, NULL);

    return buff;
}

//读取偏移后的内容，返回读取到的值，读取2个字节
u16 VisualBoyAdvance::ReadValue_2(u32 offset,u32 baseAddr)
{
    u32 address;//临时存放地址的变量

    if (FALSE == ReadProcessMemory(m_hProcess, (LPVOID)baseAddr, &address, 4, NULL))//第一次读取
        return FALSE;

    address += offset;//计算真实地址

    u16 buff;

    ReadProcessMemory(m_hProcess, (LPVOID)address, &buff, 2, NULL);

    return buff;
}

//读取偏移后的内容，返回读取到的值，读取4个字节
u32 VisualBoyAdvance::ReadValue_4(u32 offset,u32 baseAddr)
{
    u32 address;//临时存放地址的变量

    if (FALSE == ReadProcessMemory(m_hProcess, (LPVOID)baseAddr, &address, 4, NULL))//第一次读取
        return FALSE;

    address += offset;//计算真实地址

    u32 buff;

    ReadProcessMemory(m_hProcess, (LPVOID)address, &buff, 4, NULL);

    return buff;
}

u16 VisualBoyAdvance::PtrRead_2(u32 offset,u32 baseAddr)
{
    u16 buff;
    u32 dwBase;
    u32 dwPointer1,dwPointer2;
    u32 Offset;
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F54, &dwPointer1, 4, NULL);
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &dwPointer2, 4, NULL);
    //一级偏移
    Offset = (baseAddr & 0x00FFFFFF);
    ReadProcessMemory(m_hProcess, (LPVOID)(dwPointer1 + Offset), &dwBase, 4, NULL);
    //二级偏移
    Offset = ((dwBase + offset) & 0x00FFFFFF);
    ReadProcessMemory(m_hProcess, (LPVOID)(dwPointer2 + Offset), &buff, 2, NULL);
    return buff;
}

u16 VisualBoyAdvance::PtrReadXor_2(u32 offset,u32 baseAddr)
{
    u16 buff;
    u32 Xor;
    Xor = PtrRead_4(0x000000AC,baseAddr);
    buff = PtrRead_2(offset,baseAddr);
    buff ^= LOWORD(Xor);
    return buff;
}

u32 VisualBoyAdvance::PtrRead_4(u32 offset,u32 baseAddr)
{
    u32 buff,dwBase;
    u32 dwPointer1,dwPointer2;
    u32 Offset;
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F54, &dwPointer1, 4, NULL);
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &dwPointer2, 4, NULL);
    //一级偏移
    Offset = (baseAddr & 0x00FFFFFF);
    ReadProcessMemory(m_hProcess, (LPVOID)(dwPointer1 + Offset), &dwBase, 4, NULL);
    //二级偏移
    Offset = ((dwBase + offset) & 0x00FFFFFF);
    ReadProcessMemory(m_hProcess, (LPVOID)(dwPointer2 + Offset), &buff, 4, NULL);
    return buff;
}

u32 VisualBoyAdvance::PtrReadXor_4(u32 offset,u32 baseAddr)
{
    u32 Xor,buff;
    Xor = PtrRead_4(0x000000AC,baseAddr);
    buff = PtrRead_4(offset,baseAddr);
    buff ^= Xor;
    return buff;
}

BOOL VisualBoyAdvance::PtrWrite_2(u32 offset,u16 *value)
{
    u32 dwBase;
    u32 dwPointer1,dwPointer2;
    u32 Offset;
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F54, &dwPointer1, 4, NULL);
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &dwPointer2, 4, NULL);
    //一级偏移
    Offset = (0x03005D90 & 0x00FFFFFF);
    ReadProcessMemory(m_hProcess, (LPVOID)(dwPointer1 + Offset), &dwBase, 4, NULL);
    //二级偏移
    Offset = ((dwBase + offset) & 0x00FFFFFF);
    return WriteProcessMemory(m_hProcess, (LPVOID)(dwPointer2 + Offset), value, 2, NULL);
}

BOOL VisualBoyAdvance::PtrWriteXor_2(u32 offset,u16 *value)
{
    u32 Xor = PtrRead_4(0x000000AC);
    *value ^= LOWORD(Xor);
    return PtrWrite_2(offset,value);
}

BOOL VisualBoyAdvance::PtrWrite_4(u32 offset,u32 *value)
{
    u32 dwBase;
    u32 dwPointer1,dwPointer2;
    u32 Offset;
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F54, &dwPointer1, 4, NULL);
    ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &dwPointer2, 4, NULL);
    //一级偏移
    Offset = (0x03005D90 & 0x00FFFFFF);
    ReadProcessMemory(m_hProcess, (LPVOID)(dwPointer1 + Offset), &dwBase, 4, NULL);
    //二级偏移
    Offset = ((dwBase + offset) & 0x00FFFFFF);
//    qDebug() << (LPVOID)(dwPointer2 + Offset);
    return WriteProcessMemory(m_hProcess, (LPVOID)(dwPointer2 + Offset), value, 4, NULL);
}

BOOL VisualBoyAdvance::PtrWriteXor_4(u32 offset,u32 *value)
{
    u32 Xor = PtrRead_4(0x000000AC);
    *value ^= Xor;
    return PtrWrite_4(offset,value);
}

BOOL VisualBoyAdvance::WriteValue_1(u32 offset,u8 *value)
{
    u32 address;//临时存放地址的变量
    if (FALSE == ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL))//第一次读取
        return FALSE;
    address += offset;//计算真实地址
    return WriteProcessMemory(m_hProcess,(LPVOID)address,value,1,NULL);
}

BOOL VisualBoyAdvance::WriteValue_2(u32 offset,u16 *value)
{
    u32 address;//临时存放地址的变量
    if (FALSE == ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL))//第一次读取
        return FALSE;
    address += offset;//计算真实地址
    return WriteProcessMemory(m_hProcess,(LPVOID)address,value,2,NULL);
}

BOOL VisualBoyAdvance::WriteValue_4(u32 offset,u32 *value)
{
    u32 address;//临时存放地址的变量
    if (FALSE == ReadProcessMemory(m_hProcess, (LPVOID)0x005A8F50, &address, 4, NULL))//第一次读取
        return FALSE;
    address += offset;//计算真实地址
    return WriteProcessMemory(m_hProcess,(LPVOID)address,value,4,NULL);
}
