#include "scene.h"
#include "game.h"
#include "pause.h"
#include <vector>
#include "title.h"
#include "EndScene.h"
#include "game_Ui.h"
static std::vector<Scene> g_SceneStack;

void Scene_Initialize(){
	g_SceneStack.clear();
	Scene_Push(SCENE_TITLE);
}

void Scene_Finalize(){
	switch (GetCurrentScene()) {
	case SCENE_TITLE:
		Title_Finalize();
		break;

	case SCENE_VOLUME:
		break;

	case SCENE_GAME:
		Game_Finalize();
		break;

	case SCENE_PAUSE:
		break;

	case SCENE_RESULT:
		EndScene_Finalize();
		break;

	case SCENE_CLEAR:
		break;

	default:
		break;
	}
}

void Scene_Update(double elapsed_time){
	switch (GetCurrentScene()) {
	case SCENE_TITLE:
		Title_Update(elapsed_time);
		break;

	case SCENE_VOLUME:
		break;

	case SCENE_GAME:
		Game_Update(elapsed_time);
		break;

	case SCENE_PAUSE:
		break;

	case SCENE_RESULT:
		EndScene_Update(elapsed_time);
		break;

	case SCENE_CLEAR:
		break;

	default:
		break;
	}
}

void Scene_Draw(){
	for (Scene scene : g_SceneStack) {
		switch (scene) {
		case SCENE_TITLE:
			Title_Draw();
			break;

		case SCENE_VOLUME:
			break;

		case SCENE_GAME:
			Game_Draw();
			break;

		case SCENE_PAUSE:
			break;

		case SCENE_RESULT:
			EndScene_Draw();
			break;

		case SCENE_CLEAR:
			break;

		default:
			break;
		}
	}
}

void OnClick(int mx, int my,bool isClick)
{
	switch (GetCurrentScene()) {
	case SCENE_TITLE:
		TitleUI_HandleClick(mx,my,isClick);
		break;

	case SCENE_VOLUME:
		break;

	case SCENE_GAME:
		 GameUi_Button(mx, my, isClick);
		break;

	case SCENE_PAUSE:
		break;

	case SCENE_RESULT:
		EndUI_HandleClick(mx, my, isClick);
		break;

	case SCENE_CLEAR:
		break;

	default:
		break;
	}
}

Scene GetCurrentScene() {
	return g_SceneStack.back();
}

void Scene_Change(Scene scene){
	while (!g_SceneStack.empty()) {
		Scene_Pop();
	}
	Scene_Push(scene);
}

void Scene_Push(Scene scene){
	g_SceneStack.push_back(scene);

	switch (GetCurrentScene()){
	case SCENE_TITLE:
		Title_Initialize();
		break;

	case SCENE_VOLUME:
		break;

	case SCENE_GAME:
		Game_Initialize();
		break;

	case SCENE_PAUSE:
		break;

	case SCENE_RESULT:
		EndScene_Initialize();
		break;

	case SCENE_CLEAR:
		break;

	default:
		break;
	}
}

void Scene_Pop(){
	if (g_SceneStack.empty()) return;

	Scene_Finalize();

	g_SceneStack.pop_back();
}
