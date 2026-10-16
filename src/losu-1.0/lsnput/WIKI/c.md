# lsnput C层文档

lsnput的C层被设计为是平台无关的，但是目前只有Windows实现。
欲使用lsnput的C层代码，可包含lsnput.h，之后编译lsnput.c。

## 获取和设置鼠标位置

想要获取鼠标位置，需要先创建一个MousePosition对象，之后将MousePosition*传递给GetMousePosition。
想要设置鼠标位置，需要填写好MousePosition对象，之后将它的指针传递给SetMousePosion。

这两个函数的使用例程如下

```c
#include "lsnput.h"

#include <stdio.h>

int main()
{
    MousePosition p;
    
    GetMousePosition(&p); //获取坐标
    printf("the position of mouse is x:%u, y:%u.\n", p.x, p.y); //打印坐标。中文字面量有乱码等问题，此处采用英文。

    //将坐标都向正方形移动5个像素
    p.x += 5;
    p.y += 5;

    SetMousePosition(&p); //设置新坐标。此时鼠标应向右下角移动。

    return 0;
}

```

## 设置鼠标事件

设置鼠标事件，需要用到MouseAct函数和MouseEvent类型。其原型如下：

```c
typedef struct {
    uint8_t type; //鼠标事件的类型
    uint8_t slide; //鼠标中键滑动的距离。当type为LSNPUT_MOUSE_MID | LSNPUT_MOUSE_DOWN此值才有效。
} MouseEvent;


bool MouseAct(MouseEvent *event); //设置鼠标事件
```

MouseEvent的type成员可以有以下可能值:

+ LSNPUT_MOUSE_LEFT   鼠标的左键
+ LSNPUT_MOUSE_RIGHT  鼠标的右键
+ LSNPUT_MOUSE_MID    鼠标的中键
+ LSNPUT_MOUSE_UP     鼠标向上
+ LSNPUT_MOUSE_DOWN   鼠标向下

这些值直接可以通过|运算符拼接

下面是一个例程

```c
#include "lsnput.h"

int main()
{
    MouseEvent e;

    e.type = LSNPUT_MOUSE_LEFT | LSNPUT_MOUSE_DOWN; //类型为左键向下，即摁下左键
    MouseEvent(&e);

    e.type = LSNPUT_MOUSE_LEFT | LSNPUT_MOUSE_UP; //摁下左键后，要恢复左键
    MouseEvent(&e);

    return 0;
}

```

## 高层API

lsnput封装了MouseClick函数，将鼠标移到指定位置并摁下相应的键。该函数不能处理中键的滚动。
MouseClick函数中会调用SetMousePosition、MouseAct，将鼠标移到对应位置，将对应键摁下并抬起。

原型如下

```c

bool MouseClick(MousePosition *p, uint64_t type); //将鼠标移到指定位置，并点击相应的键

```

下面是一个例程

```c
#include "lsnput.h"

int main()
{
    MousePosition p;

    GetMousePosition(&p); //获取当前位置
    
    //移动鼠标位置
    p.x += 25;
    p.y += 25;

    //按下鼠标左键
    MouseClick(&p, LSNPUT_MOUSE_LEFT);

    return 0;
}

```
