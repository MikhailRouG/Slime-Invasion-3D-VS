#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;

#define MAX_BONES 128

struct KeyFrame
{
    float Time;

    XMFLOAT3 Position;
    XMFLOAT4 Rotation;
    XMFLOAT3 Scale;
};

struct BoneAnimation
{
    std::vector<KeyFrame> Keys;
};

struct AnimationClip
{
    std::string Name;
    float Duration;
    std::vector<BoneAnimation> Bones;
};

class Animator
{
private:

    std::vector<XMMATRIX> finalBones;
    std::vector<int> parents;

    AnimationClip current;

    float timer = 0.0f;
    bool loop = true;

public:

    void Init(int boneCount)
    {
        finalBones.resize(boneCount);
        parents.resize(boneCount, -1);
    }

    void SetParent(int bone, int parent)
    {
        parents[bone] = parent;
    }

    void SetClip(AnimationClip clip)
    {
        current = clip;
    }

    void Play()
    {
        timer = 0.0f;
    }

    void Update(float dt);

    XMMATRIX* GetBones()
    {
        return finalBones.data();
    }
};
