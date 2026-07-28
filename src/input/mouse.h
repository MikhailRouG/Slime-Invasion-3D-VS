//--------------------------------------------------------------------------------------
// File: mouse.h
//--------------------------------------------------------------------------------------
// 2020/02/11
// Licensed under the MIT License.
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------
#ifndef HAL_YOUHEI_MOUSE_H
#define HAL_YOUHEI_MOUSE_H
#pragma once


#include <windows.h>
#include <memory>


typedef enum Mouse_PositionMode_tag
{
    MOUSE_POSITION_MODE_ABSOLUTE,
    MOUSE_POSITION_MODE_RELATIVE,
} Mouse_PositionMode;


typedef struct MouseState_tag
{
    bool leftButton;
    bool middleButton;
    bool rightButton;
    bool xButton1;
    bool xButton2;
    int x;
    int y;
    int scrollWheelValue;
    Mouse_PositionMode positionMode;
} Mouse_State;


void Mouse_Initialize(HWND window);

void Mouse_Finalize(void);

void Mouse_GetState(Mouse_State* pState);

void Mouse_ResetScrollWheelValue(void);

void Mouse_SetMode(Mouse_PositionMode mode);

bool Mouse_IsConnected(void);

bool Mouse_IsVisible(void);

void Mouse_SetVisible(bool visible);

void Mouse_ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);


// LResult CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

#endif // MOUSE_H
