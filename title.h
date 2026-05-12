#ifndef TITLE_H
#define TITLE_H

void Title_Initialize();
void Title_Finalize();
void Title_Update(double elapsed_time);
void Title_Draw();
void TitleUI_HandleClick(int mx, int my, bool  isClick);
#endif // !TITLE_H