#ifndef GAME_UI_H
#define GAME_UI_H

void GameUI_Initialize();
void GameUI_Finalize();
void GameUI_Update();
void GameUI_Draw();
void GameUi_Button(int mx, int my, bool isClick);
void GameUi_OnGetingExp(int exp, int expToLvl, int lvl);
#endif //GAME_H