#include "score.h"
#include "sprite.h"
#include "algorithm"
#include "text.h"
#include "game_Ui.h"
static unsigned int g_Score = 0;
static unsigned int g_ViewScore = 0;
int m_currentExp;
int m_ExpToLvlUp;
int m_CurrentLvl;
static int g_Digit = 1;
static unsigned int g_CounterStop = 1;
static float g_x = 0.0f, g_y = 0.0f;
static int g_ScoreTexId = -1;

static constexpr int Score_Font_Width = 179;
static constexpr int Score_Font_Height = 185;


static void drawNumber(float x, float y, int number);

void Score_Initialize(float x,float y,int digit){
	g_Score = 1;
	g_ViewScore = 0;
	m_currentExp = 0;
	m_ExpToLvlUp = 5;
	m_CurrentLvl = 1;
	g_Digit = digit;
	g_x = x;
	g_y = y;

	for (int i = 0;i < digit;i++) {
		g_CounterStop *= 10;
	}
	g_CounterStop--;
	GameUi_OnGetingExp(m_currentExp, m_ExpToLvlUp, m_CurrentLvl);
}

void Score_Finalize(){
}

void Score_Update(){
	g_ViewScore = std::min(g_ViewScore + 1, g_Score);
}

void Score_Draw(){
	char buf[64];
	sprintf_s(buf, "Score: %d", g_Score);
	Text_Draw(g_x, g_y, buf);
}

unsigned int Score_GetScore(){
	return g_Score;
}

void Score_AddScore(int score){
	g_ViewScore = g_Score;
	g_Score += score;
	m_currentExp += score;
	if (m_ExpToLvlUp < m_currentExp)
	{
		m_currentExp -= m_ExpToLvlUp;
		m_CurrentLvl++;
		m_ExpToLvlUp *= 2;
	}
	GameUi_OnGetingExp(m_currentExp, m_ExpToLvlUp, m_CurrentLvl);
}

void Score_Reset(){
	g_Score = 0;
}

void drawNumber(float x, float y, int number){
	Sprite_Draw(g_ScoreTexId, x, y, 32.0f, 64.0f, Score_Font_Width * number+30, 0, Score_Font_Width-70, Score_Font_Height);
}
