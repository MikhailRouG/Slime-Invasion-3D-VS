#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN //コンパイル時間が早くなる(古いプログラムを使わない)
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
//メイン
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow) {

	//COMライブラリの初期化
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	////GPIスケーリング
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HWND hWnd = GameWindow_Create(hInstance);

	SystemTimer_Initialize();
	KeyLogger_Initialize();
	Mouse_Initialize(hWnd);
	InitAudio();

	Direct3D_Initialize(hWnd); //ダイレクト3Dの初期化
	Shader2d_Initialize(Direct3D_GetDevice(), Direct3D_GetContext()); //↓ダイレクト3Dの初期化の後に初期化
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
//#if defined(DEBUG)||defined(_DEBUG)
//	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
//		L"resource/texture/consolab_ascii_512.png",
//		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
//		0.0f, 0.0f, //調整
//		0, 0,
//		0.0f, 14.0f); //調整
//
//	Collision_DebugInitialize(Direct3D_GetDevice(), Direct3D_GetContext());
//#endif

	ShowWindow(hWnd, nCmdShow); //指定のウィンドウを画面に表示する
	UpdateWindow(hWnd); //ウィンドウの中を更新する

	double exec_last_time = SystemTimer_GetTime();
	double fps_last_time = exec_last_time;
	double current_time = 0.0;
	ULONG frame_count = 0;
	double fps = 0.0;
	double elapsed_time = 0.0f;
	MSG msg;

	//ウィンドウプログラムの中心	
	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { //ウィンドウメッセージが来ていたら
		
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else { //ゲームの処理
			current_time = SystemTimer_GetTime(); //システム時刻を取得
			double fps_elapsed_time = current_time - fps_last_time; //fps計測用の経過時間を計算

			if (fps_elapsed_time >= 1.0) { //1秒ごとに計測
				fps = frame_count / fps_elapsed_time;
				fps_last_time = current_time; //FPSを測定した時刻を保存
				frame_count = 0; //カウントをクリア
			}

			//1/60秒ごとに実行
			elapsed_time += SystemTimer_GetElapsedTime();
			//if(elapsed_time>=(1.0/60.0)){ //60fpsに固定
			//if (true) 
			{ //どんなfps値でもアニメーションの動きは固定
				//exec_last_time = current_time; //処理した時刻を保存

				//ゲームの更新
				KeyLogger_Update();

				Mouse_State ms{};
				Mouse_GetState(&ms); //マウスの状態取得

				
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

//#if defined(DEBUG)||defined(_DEBUG)//デバッグの時だけ有効
//				std::stringstream ss;
//				ss << "fps:" << fps << std::endl;
//
//				dt.SetText(ss.str().c_str(), { 0.0f,1.0f,0.0f,1.0f });
//				//dt.SetText("debug!\nwatashidayo! \n nye!!", { 0.0f,0.0f,1.0f,1.0f });
//
//				dt.Draw();
//				dt.Clear();
//#endif

				//画面のスワップ
				Direct3D_Present();

				frame_count++;
				elapsed_time = 0.0f;
			}
		}
	} while (msg.message != WM_QUIT);

//	
//#if defined(DEBUG)||defined(_DEBUG)
//	Collision_DebugFinalize();
//#endif

	//初期化と逆順に後片付け
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

