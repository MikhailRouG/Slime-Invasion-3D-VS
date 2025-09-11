/*==============================================================================

   ÉQÅ[ÉÄñ{ëÃ[game.cpp]
														 Author : Harada Ren
														 Date   : 2025/06/27
--------------------------------------------------------------------------------

==============================================================================*/
#include "game.h"
#include "key_logger.h"
#include "game_window.h"
#include "cube.h"
#include "shader3d.h"


void Game_Initialize(){

}

void Game_Finalize(){
	
}

void Game_Update(double){
	if (KeyLogger_IsTrigger(KK_ESCAPE)) {
		SendMessage(GameWindow_GetHWND(), WM_CLOSE, 0, 0);
	}
}

void Game_Draw(){
	Cube_Draw();
}

