#ifndef SCENE_H
#define SCENE_H

void Scene_Initialize();
void Scene_Finalize();
void Scene_Update(double elapsed_time);
void Scene_Draw();
void OnClick(int mx, int my,bool isClick);

enum Scene {
	SCENE_TITLE,
	SCENE_VOLUME,
	SCENE_GAME,
	SCENE_PAUSE,
	SCENE_RESULT,
	SCENE_CLEAR,
	SCENE_MAX
};

Scene GetCurrentScene();

void Scene_Change(Scene scene);
void Scene_Push(Scene scene);
void Scene_Pop();

#endif //SCENE_H