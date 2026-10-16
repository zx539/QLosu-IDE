#include "ELS_lsnput.h"
#include "clsnput.h"

#include <string.h>

int ELSAPI_lsnput_GetMousePosition(els_VmObj* vm)
{
    LosuObj output = obj_newunit(vm);
    MousePosition p;

    //获取鼠标位置
    GetMousePosition(&p);
    obj_setunit(vm, output, obj_newstr(vm, "x"), obj_newnum(vm, p.x));
    obj_setunit(vm, output, obj_newstr(vm, "y"), obj_newnum(vm, p.y));

    arg_return(vm, output);
    return 1;
}

int ELSAPI_lsnput_SetMousePosition(els_VmObj* vm)
{
    MousePosition p;

    //获取要设置的坐标
    p.x = arg_getnum(vm, 2);
    p.y = arg_getnum(vm, 3);

    //设置鼠标位置
    if (SetMousePosition(&p))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);

    return 1;
}

int ELSAPI_lsnput_MouseAct(els_VmObj* vm)
{
    MouseEvent e;

    e.type = (uint32_t)arg_getnum(vm, 2) | (uint32_t)arg_getnum(vm, 3); //获取鼠标事件的类型
    e.slide = arg_getnum(vm, 4);                                        //获取滑轮滚动的距离

    if (MouseAct(&e))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);

    return 1;
}

int ELSAPI_lsnput_click(els_VmObj *vm)
{
    MousePosition p;
    uint8_t type = arg_getnum(vm, 2);

    GetMousePosition(&p);
    if (MouseClick(&p, type))
        arg_returnnum(vm, 1);
    else
        arg_returnnull(vm);
    return 1;
}

int ELSAPI_lsnput_lsnput_init(els_VmObj *vm)
{
    LosuObj lib = obj_newunit(vm);

    vm_setval(vm, "lsnput", lib);

    obj_setunit(vm, lib, obj_newstr(vm, "get_mouse_position"), obj_newfunction(vm, ELSAPI_lsnput_GetMousePosition));
    obj_setunit(vm, lib, obj_newstr(vm, "set_mouse_position"), obj_newfunction(vm, ELSAPI_lsnput_SetMousePosition));
    obj_setunit(vm, lib, obj_newstr(vm, "mouse_act"), obj_newfunction(vm, ELSAPI_lsnput_MouseAct));
    obj_setunit(vm, lib, obj_newstr(vm, "click"), obj_newfunction(vm, ELSAPI_lsnput_click));
    //obj_setunit(vm, lib, obj_newstr(vm, ""), obj_newfunction(vm, ELSAPI_lsnput_));

    return 0;
}
