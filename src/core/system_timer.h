
#ifndef SYSTEM_TIMER_H_
#define SYSTEM_TIMER_H_



void SystemTimer_Initialize(void);

void SystemTimer_Reset(void);

void SystemTimer_Start(void);

void SystemTimer_Stop(void);

void SystemTimer_Advance(void);

double SystemTimer_GetTime(void);

double SystemTimer_GetAbsoluteTime(void);

double SystemTimer_GetElapsedTime(void);

bool SystemTimer_IsStoped(void);

void LimitThreadAffinityToCurrentProc(void);

#endif // SYSTEM_TIMER_H_
