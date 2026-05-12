#include "game_Ui.h"
#include "text.h"
#include "enemy.h"
#include "player.h"
#include "score.h"
#include <stdio.h>
#include "sprite.h"
#include "texture.h"
#include "hummer.h"
#include "game.h"
#include "mouse.h"
using namespace DirectX;
int m_plusTexId;

int m_PlayerExp;
int m_PlayerExptoLvl = 0;
int  m_PlayerLvl =0;
int m_PlayerAbillity;
int g_playerHealth = 0;
int g_SpeedAtk;
int g_AttackDamage;
struct ButtonRect {
    int x, y, w, h;
    int statId; 
    XMFLOAT4 color = { 1,1,1,1 };
};
const int plusw = 670;
const float plusSize = 40;
const int startH = 900;

ButtonRect g_StatsButtons[] = {
    { plusw, 900, (int)plusSize, (int)plusSize, 0 }, 
    { plusw, 950, (int)plusSize, (int)plusSize, 1 }, 
    { plusw, 1000, (int)plusSize, (int)plusSize, 2 } 
};
void GameUI_DrawHpBar(float x, float y, float w, float h, int hp, int maxHp);
void GameUI_DrawStatsBar();
void GameUi_Timer();
void GameUI_Initialize()
{
    m_PlayerAbillity = 1;
    m_plusTexId = Texture_Load(L"resource/texture/plus.png");
    g_playerHealth = Player_GetHp();
    g_SpeedAtk = Hummer_GetSpeedAtk();
    g_AttackDamage = Hummer_GetAttackDamage();
}

void GameUI_Finalize()
{
}
void GameUI_Update()
{
}

void GameUI_Draw()
{
    XMFLOAT4 white = { 1,1,1,1 };
    XMFLOAT4 red = { 1,0,0,1 };
    XMFLOAT4 green = { 0,1,0,1 };

    int hp = Player_GetHp();
     int maxHp = Player_GetMaxHp();
    int score = Score_GetScore();

    char buf[64];

    GameUI_DrawHpBar(20, 15, 400, 36, Player_GetHp(), Player_GetMaxHp());
    sprintf_s(buf, "HP %d / %d", hp, maxHp);
    Text_Draw(20, 20, buf, white, 1.0f);

    sprintf_s(buf, "Exp %d / %d", m_PlayerExp, m_PlayerExptoLvl);
    Text_Draw(20, 60, buf, white, 1.0f);
    sprintf_s(buf, "Lvl : %d", m_PlayerLvl);
    Text_Draw(20, 100, buf, white, 1.0f);
    GameUI_DrawStatsBar();
     GameUi_Timer();
}

void GameUI_DrawHpBar(float x, float y, float w, float h, int hp, int maxHp)
{
    if (maxHp <= 0) return;

    float rate = (float)hp / (float)maxHp;
    if (rate < 0) rate = 0;
    if (rate > 1) rate = 1;

    Sprite_Draw(
        0,   
        x, y,
        w, h,
        0, 0, 1, 1,
        0.0f,
        { 0,0,0,1 }
    );

    Sprite_Draw(
        0,
        x, y,
        w * rate, h,
        0, 0, 1, 1,
        0.0f,
        { 1,0,0,1 }
    );
}

void GameUI_DrawStatsBar()
{
    Sprite_Draw(
        0,
        0, 880,
        720, 200,
        0, 0, 1, 1,
        0.0f,
        { 0,0,0,0.3 }
    );
    float plusSize = 40;
    int plusw = 670;
    int h = 900;
    char buf[64];
    sprintf_s(buf, "Skill points: %d", m_PlayerAbillity);
    Text_Draw(10, h, buf, { 1,1,1,1 }, 1.2f);
    h += 50;
    sprintf_s(buf, "Attack dmg: %d", g_AttackDamage);
    Text_Draw(10, h, buf, { 0,1,0,1 }, 1.2f);
    if(m_PlayerAbillity > 0)
    Sprite_Draw(
        m_plusTexId,
        plusw, h,
        plusSize, plusSize,
        0, 0, 1000, 1000,
        0.0f,
        g_StatsButtons[1].color
    );
    h += 50;
    sprintf_s(buf, "Attack speed: %d", g_SpeedAtk);
    Text_Draw(10, h, buf, { 0,0,1,1 }, 1.2f);
   if(g_SpeedAtk<999 && m_PlayerAbillity > 0)
    {
        Sprite_Draw(
            m_plusTexId,
            plusw, h,
            plusSize, plusSize,
            0, 0, 1000, 1000,
            0.0f,
            g_StatsButtons[2].color
        );
    }
}

void GameUi_Timer()
{
    int t = Get_GamePassedTime();

    int minutes = t / 60;
    int seconds = t % 60;

    char buf[64];
    sprintf_s(buf, "%d:%02d", minutes, seconds);
    Text_Draw(880, 10, buf, { 1,1,1,1 }, 1.2f);
}

void GameUi_Button(int mx, int my, bool isClick)
{
        for (int i = 0; i < 3; i++) {
            ButtonRect& btn = g_StatsButtons[i];

            if (mx >= btn.x && mx <= (btn.x + btn.w) &&
                my >= btn.y && my <= (btn.y + btn.h))
            {
                if (isClick && m_PlayerAbillity > 0)
                {
                    if (btn.statId == 0) g_playerHealth += 10;
                    if (btn.statId == 1) {
                        Hummer_SetDamage(20);
                        g_AttackDamage = Hummer_GetAttackDamage();
                        m_PlayerAbillity--;
                    }
                    if (btn.statId == 2)
                    {
                        Hummer_SetSpeed(50);
                        g_SpeedAtk = Hummer_GetSpeedAtk();
                        m_PlayerAbillity--;
                    }
                }
                else
                    btn.color = { 1,1,1,0.5 };
                break;
            }
            else
            {
                btn.color = { 1,1,1,1 };
            }
        }
    
}

void GameUi_OnGetingExp(int exp, int expToLvl, int lvl)
{
     m_PlayerExp = exp;
     m_PlayerExptoLvl = expToLvl;
     int prevLvl = m_PlayerLvl;

     m_PlayerExp = exp;
     m_PlayerExptoLvl = expToLvl;

     if (lvl > prevLvl)
     {
         int gained = lvl - prevLvl;
         m_PlayerAbillity += gained;
         m_PlayerAbillity += (lvl / 10) - (prevLvl / 10);
     }

     m_PlayerLvl = lvl;
}
