#ifndef LSNPUT_H_
#define LSNPUT_H_

/*
    洛书的监听、操作鼠标和键盘的模块
    作者：Matriller
    立项时间：2023-9-29
    历史维护者：Matriller
    版本：1.7.0.1
*/

#include <stdint.h>
#include <stdbool.h>

#define LSNPUT_MOUSE_LEFT   0b00000001
#define LSNPUT_MOUSE_RIGHT  0b00000010
#define LSNPUT_MOUSE_MID    0b00000100
#define LSNPUT_MOUSE_UP     0b00001000
#define LSNPUT_MOUSE_DOWN   0b00010000

typedef struct {
    uint32_t x; //鼠标的x轴坐标
    uint32_t y; //鼠标的y轴坐标
} MousePosition;

typedef struct {
    uint8_t type; //鼠标事件的类型
    uint8_t slide; //鼠标中键滑动的距离。当type为LSNPUT_MOUSE_MID | LSNPUT_MOUSE_DOWN此值才有效。
} MouseEvent;

bool GetMousePosition(MousePosition *position); //获取鼠标位置
bool SetMousePosition(MousePosition *position); //设置鼠标位置
bool MouseAct(MouseEvent *event); //设置鼠标事件
bool MouseClick(MousePosition *p, uint64_t type); //将鼠标移到指定位置，并点击相应的键

#endif