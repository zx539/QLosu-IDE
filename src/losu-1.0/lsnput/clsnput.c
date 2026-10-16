#include "clsnput.h"

/*
    Windows系统实现
*/
#ifdef LOSU_WINDOWS

#include <windows.h>

inline bool GetMousePosition(MousePosition *position)
{
    /*
        BOOL GetCursorPos(
            [out] LPPOINT lpPoint
        );

        检索鼠标光标的位置（以屏幕坐标为单位）。

        参数
            [out] lpPoint
            类型： LPPOINT
            指向接收光标屏幕坐标的 POINT 结构的指针。

        返回值
            类型： BOOL
            如果成功，则返回非零值，否则返回零。 要获得更多的错误信息，请调用 GetLastError。
        
        注解
            光标位置始终在屏幕坐标中指定，不受包含光标的窗口的映射模式的影响。
            调用进程必须具有对窗口工作站 WINSTA_READATTRIBUTES 访问权限。
            调用 GetCursorPos 时，输入桌面必须是当前桌面。 调用 OpenInputDesktop 以确定当前桌面是否为输入桌面。 
            如果不是，请使用 OpenInputDesktop 返回的 HDESK 调用 SetThreadDesktop 以切换到该桌面。
    */

    bool output;
    POINT pt;

    output = GetCursorPos(&pt);
    position->x = pt.x;
    position->y = pt.y;

    return output;
}

inline bool SetMousePosition(MousePosition *position)
{
    /*
        BOOL SetCursorPos(
            [in] int X,
            [in] int Y
        );

        将光标移动到指定的屏幕坐标。 如果新坐标不在由最近的 ClipCursor 函数调用设置的屏幕矩形内，则系统会自动调整坐标，使光标停留在矩形内。

        参数
            [in] X
            类型： int
            光标的新 x 坐标（以屏幕坐标为单位）。

            [in] Y
            类型： int
            光标的新 y 坐标（以屏幕坐标为单位）。

        返回值
            类型： BOOL
            如果成功，则返回非零值，否则返回零。 要获得更多的错误信息，请调用 GetLastError。
        
        注解
            游标是共享资源。 仅当光标位于窗口工作区时，窗口才应移动光标。
            调用进程必须具有对窗口工作站 WINSTA_WRITEATTRIBUTES 访问权限。
            调用 SetCursorPos 时，输入桌面必须是当前桌面。 调用 OpenInputDesktop 以确定当前桌面是否为输入桌面。 
            如果不是，请使用 OpenInputDesktop 返回的 HDESK 调用 SetThreadDesktop 以切换到该桌面。
    */
    return SetCursorPos(position->x, position->y);
}

bool MouseAct(MouseEvent *event)
{
    /*
        void mouse_event(
            [in] DWORD     dwFlags,
            [in] DWORD     dx,
            [in] DWORD     dy,
            [in] DWORD     dwData,
            [in] ULONG_PTR dwExtraInfo
        );

        mouse_event 函数合成鼠标运动和按钮单击。


    */
    DWORD dwFlags, dwData;

    dwData = 0;

    switch (event->type)
    {
        case LSNPUT_MOUSE_LEFT  |   LSNPUT_MOUSE_DOWN:
            //左键向下
            dwFlags = MOUSEEVENTF_LEFTDOWN;
            break;
        case LSNPUT_MOUSE_LEFT  |   LSNPUT_MOUSE_UP:
            //左键向上
            dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case LSNPUT_MOUSE_RIGHT |   LSNPUT_MOUSE_DOWN:
            //右键向下
            dwFlags = MOUSEEVENTF_RIGHTDOWN;
            break;
        case LSNPUT_MOUSE_RIGHT |   LSNPUT_MOUSE_UP:
            //右键向上
            dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case LSNPUT_MOUSE_MID   |   LSNPUT_MOUSE_DOWN:
            //中键向下
            if (event->slide == 0)
                //纯向下
                dwFlags = MOUSEEVENTF_MIDDLEDOWN;    
            else
            {
                //滚轮滑动
                dwFlags = MOUSEEVENTF_WHEEL;
                dwData = event->slide;
            }
            break;
        case LSNPUT_MOUSE_MID   |   LSNPUT_MOUSE_UP:
            //中键向上
            dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            return false;
    }

    mouse_event(dwFlags, 0, 0, dwData, 0);

    return true;
}

#endif

/*
    Linux系统实现
*/

#ifdef LOSU_LINUX

#endif


/*
    通用操作系统实现
*/

bool MouseClick(MousePosition *p, uint64_t type)
{
    MouseEvent e;

    e.slide = 0;
    SetMousePosition(p);

    if (type & 0b00000111)
    {
        e.type = type | LSNPUT_MOUSE_DOWN;
        MouseAct(&e);
        e.type = type | LSNPUT_MOUSE_UP;
        MouseAct(&e);
        return true;
    }
    return false;
}

