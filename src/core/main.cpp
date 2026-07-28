#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "game_window.h"
#include "direct3d.h"
#include "shader2d.h"
#include "shader3d.h"
#include "shader3d_unlit.h"
#include "sampler.h"
#include "sprite.h"
#include "Texture.h"
#include "debug_ostream.h"
#include "sprite_anim.h"
#include "fade.h"
#include "collision.h"
#include "debug_text.h"
#include <sstream>
#include "system_timer.h"
#include "Key_logger.h"
#include "mouse.h"
#include <Xinput.h>
#pragma comment(lib,"xinput.lib")
#include "scene.h"
#include "Audio.h"
#include "game.h"
#include "cube.h"
#include "grid.h"
#include "meshfield.h"
#include "light.h"
#include "shader_depth.h"
#include "text.h"
#include "player.h"
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow) {

	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = GameWindow_Create(hInstance);

	SystemTimer_Initialize();
	KeyLogger_Initialize();
	Mouse_Initialize(hWnd);
	InitAudio();

	Direct3D_Initialize(hWnd);
	Shader2d_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Shader3d_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Shader3DUnlit_Initialize();
	Sampler_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Fade_Initialize();
	Scene_Initialize();
	Grid_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Cube_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Light_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
	Meshfield_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

	Player_SetDevice(Direct3D_GetDevice(), Direct3D_GetContext());
	ShaderDepth_Initialize();
	Text_Initialize();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG frame_count = 0;
	double fps = 0.0;
	double elapsed_time = 0.0f;
	MSG msg;

	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			current_time = SystemTimer_GetTime();
			double fps_elapsed_time = current_time - fps_last_time;

			if (fps_elapsed_time >= 1.0) {
				fps = frame_count / fps_elapsed_time;
				fps_last_time = current_time;
				frame_count = 0;
			}

			elapsed_time += SystemTimer_GetElapsedTime();
			{

				KeyLogger_Update();

				Mouse_State ms{};
				Mouse_GetState(&ms);

				
				Scene_Update(elapsed_time);
				SpriteAnim_Update(elapsed_time);
				Fade_Update(elapsed_time);

				Direct3D_SetBackBuffer();
				Direct3D_ClearBackBuffer();
				if (KeyLogger_IsTrigger(KK_ESCAPE)) {
					SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
				}
				Scene_Draw();
				Fade_Draw();


				Direct3D_Present();

				frame_count++;
				elapsed_time = 0.0f;
			}
		}
	} while (msg.message != WM_QUIT);


	Meshfield_Finalize();
	Light_Finalize();
	Cube_Finalize();
	Grid_Finalize();
	Scene_Finalize();
	Fade_Finalize();
	Sampler_Finalize();
	Shader3d_Finalize();
	Shader3DUnlit_Finalize();
	Shader2d_Finalize();
	Direct3D_Finalize();
	Sprite_Finalize();
	Texture_Finalize();
	UninitAudio();
	Mouse_Finalize();
	ShaderDepth_Finalize();
	return (int)msg.wParam;
}

