#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H


void AudioManager_Initialize();
void AudioManager_Finalize();
void AudioManager_SetVolume(float volume);
float AudioManager_GetVolume();


#endif //AUDIO_MANAGER_H
