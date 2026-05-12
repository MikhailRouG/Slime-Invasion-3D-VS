#include "game.h"
#include "key_logger.h"
#include "game_window.h"
#include "cube.h"
#include "shader3d.h"
#include "camera.h"
#include "player_camera.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "sampler.h"		
#include "light.h"
#include "model.h"
#include "player.h"
#include "map.h"
#include "texture.h"
#include "bullet.h"
#include "billboard.h"
#include "sprite_anim.h"
#include "bullet_hit_effect.h"
#include "direct3d.h"
#include "trajectory3d.h"
#include "sky.h"
#include "enemy.h"
#include "enemy_spawner.h"
#include "sprite.h"
#include "mouse.h"
#include "mapcamera.h"
#include "light_camera.h"
#include "circle_shadow.h"
#include "hummer.h"
#include "score.h"
#include "scene.h"
#include "game_Ui.h"
#include "particle_test.h"
#include "ModelLoader.h"
#include "animator.h"
static int g_AnimPatternId = -1;
static int g_AnimPlayId = -1;

static bool g_IsDebug = false;
int g_TextTex01 = 0;

float g_PassedTime;
void Map_Rendering();
void Light_Rendering();

void Game_Initialize(){
	PlayerCamera_Initialize();
	Camera_Initialize({10.0f,10.0f,-10.0f},{-0.6f,-0.4f,0.6f},{0.7f,0.0f,0.7f});
	Map_Initialize();
	Player_Initialize({ 0.0f,10.0f,0.0f }, { 0.0f,0.0f,1.0f });
	Bullet_Initialize();
	Hummer_Initialize();
	Enemy_Initialize();
	EnemySpawner_Initialize();
	Sky_Initialize();
	Billboard_Initialize();
	
	BulletHitEffect_Initialize();
	Trajectory3d_Initialize();
	LightCamera_Initialize({ -1.0f,-1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	CircleShadow_Initialize();
	g_IsDebug = false;
	g_TextTex01 = Texture_Load(L"resource/texture/Front.png");
	Score_Initialize(1600,30,1);
	GameUI_Initialize();
	g_PassedTime = 0;
}

void Game_Finalize(){
	Sky_Finalize();
	Billboard_Finalize();
	PlayerCamera_Finalize();
	Camera_Finalize();
	Player_Finalize();
	Enemy_Finalize();
	Bullet_Finalize();
	Map_Finalize();
	Hummer_Finalize();
	Score_Finalize();
	 GameUI_Finalize();
}

void Game_Update(double elapsed_time){
	g_PassedTime += elapsed_time;
	Player_Update(elapsed_time);
	Hummer_Update(elapsed_time);
	if (GetCurrentScene() == SCENE_GAME)
	{
		Enemy_Update(elapsed_time);
		EnemySpawner_Update(elapsed_time);
	}
	Sky_SetPosition(Player_GetPosition());
	if (g_IsDebug) {
		Camera_Update(elapsed_time);
	}
	else {
		PlayerCamera_Update(elapsed_time);
	}

	Bullet_Update(elapsed_time);

	//for (int j = 0;j < Map_GetObjectsCount();j++) {
	//	for (int i = 0;i < Bullet_GetCount();i++) {
	//		AABB bullet = Bullet_GetAABB(i);
	//		AABB object = Map_GetObject(j)->aabb;
	//		if (Collision_IsOverlapAABB(bullet, object)) {
	//			BulletHitEffect_Create(Bullet_GetPosition(i));
	//			Bullet_Destroy(i);
	//		}
	//	}
	//}
	//for (int j = 0;j < Enemy_GetEnemyCount();j++) {
	//	for (int i = 0;i < Bullet_GetCount();i++) {
	//		Sphere bullet = Bullet_GetSphere(i);
	//		Sphere enemy = Enemy_GetEnemy(j)->GetCollision();

	//		if (Collision_IsOverlapSphere(bullet, enemy)) {
	//			BulletHitEffect_Create(Bullet_GetPosition(i));
	//			Bullet_Destroy(i);
	//			Enemy_GetEnemy(j)->Damage(50);
	//		}
	//	}
	//}

	BulletHitEffect_Update();
	Trajectory3d_Update(elapsed_time);
	Sky_Update(elapsed_time);

	GameUI_Update();
}

void Game_Draw(){

	Direct3D_SetBackBuffer();
	Direct3D_ClearBackBuffer();
	Direct3D_SetDepthTexture(2);

	XMFLOAT4X4 mtxView = g_IsDebug ? Camera_GetViewMatrix() : PlayerCamera_GetViewMatrix();
	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = g_IsDebug ? XMLoadFloat4x4(&Camera_GetPerspectiveMatrix()) : XMLoadFloat4x4(&PlayerCamera_GetPerspectiveMatrix());
	XMFLOAT3 camera_position = g_IsDebug ? Camera_GetPosition() : PlayerCamera_GetPosition();

	Direct3D_SetLightViewProjctionMatrix(LightCamera_GetViewMatrix() * LightCamera_GetProjectionMatrix());
	Camera_SetMatrix(view, proj);

	Sampler_SetFilterAnisotropic();

	Direct3D_SetDepthEnable(false);
	Sky_Draw();
	Direct3D_SetDepthEnable(true);

	Light_SetAmbient({ 0.45f, 0.45f, 0.45f });
	XMVECTOR vDir = XMVector3Normalize(XMVectorSet(-1.0f, -1.0f, 1.0f, 0.0f));
	XMFLOAT4 dir;
	XMStoreFloat4(&dir, vDir);
	Light_SetDirectionalWorld(dir, { 0.75f, 0.75f, 0.75f, 1.0f });
	Light_SetSpecularWorld(camera_position, 12.0f, { 1,1,1,1 });
	Map_Draw();
	Player_Draw();
	Hummer_Draw();
	Direct3D_SetDepthEnable(true);
	Enemy_Draw();
	Bullet_Draw();
	Trajectory3d_Draw();
	BulletHitEffect_Draw();

	Direct3D_SetDepthStencilStateDepthWhiteDisable();
	Direct3D_SetAlphaBlendAdd();
	Direct3D_SetAlphaBlendTransparent();
	Direct3D_SetDepthEnable(false);
	Sprite_Begin(); 

	Direct3D_SetOffscreenTexture(0);
	Sprite_Draw(
		Direct3D_GetBackBufferWidth() * 0.8f,
		Direct3D_GetBackBufferHeight() * 0.04f,
		256, 256
	);

	Score_Draw();
	 GameUI_Draw();
	Direct3D_SetDepthEnable(true);
}

float Get_GamePassedTime()
{
	return g_PassedTime;
}

void Map_Rendering()
{
	/*Direct3D_SetOffscreen();
	Direct3D_ClearOffscreen();

	XMFLOAT3 position = Player_GetPosition();
	position.y = 100.0f;
	MapCamera_SetPosition(position);
	MapCamera_SetFront(Player_GetFront());
	XMFLOAT4X4 mtxView = MapCamera_GetViewMatrix();
	XMFLOAT4X4 mtxProj = MapCamera_GetPerspectiveMatrix();
	XMMATRIX view = XMLoadFloat4x4(&mtxView);
	XMMATRIX proj = XMLoadFloat4x4(&mtxProj);

	Camera_SetMatrix(view, proj);
	Sampler_SetFilterAnisotropic();

	Direct3D_SetDepthEnable(true);

	Enemy_Draw();
	Player_Draw();
	Map_Draw();*/
}

void Light_Rendering()
{
	Direct3D_SetDepth();
	Direct3D_ClearDepth();

	XMMATRIX view = LightCamera_GetViewMatrix();
	XMMATRIX proj = LightCamera_GetProjectionMatrix();

	Camera_SetMatrix(view, proj);

	Direct3D_SetDepthEnable(true);

	Enemy_DepthDraw();
	Player_DepthDraw();
	//Map_Draw();
}
