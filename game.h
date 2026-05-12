#ifndef GAME_H
#define GAME_H


void Game_Initialize();
void Game_Finalize();

void Game_Update(double elapsed_time);
void Game_Draw();

float Get_GamePassedTime();

#endif //GAME_H