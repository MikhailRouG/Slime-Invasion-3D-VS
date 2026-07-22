#ifndef HUMMER_H
#define HUMMER_H

#include <DirectXMath.h>
#include "collision.h"
#include "map.h"
struct Sphere;

void Hummer_Initialize();
void Hummer_Finalize();
void Hummer_Update(double elapsed_time);
void Hummer_Draw();
Sphere Hummer_GetHitSphere();

int Hummer_GetSpeedAtk();
int Hummer_GetAttackDamage();
void Hummer_SetScale();
void Hummer_SetSpeed(int speed);
void Hummer_SetDamage(int dmg);
#endif //HUMMER_H