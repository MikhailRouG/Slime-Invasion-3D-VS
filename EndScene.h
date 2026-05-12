#ifndef END_SCENE_H
#define END_SCENE_H

void EndScene_Initialize();
void EndScene_Finalize();
void EndScene_Update(double elapsed_time);
void EndScene_Draw();
void EndUI_HandleClick(int mx, int my, bool isClick);
#endif // !END_SCENE_H