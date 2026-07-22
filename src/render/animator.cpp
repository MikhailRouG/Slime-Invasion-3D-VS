#include "animator.h"

void Animator::Update(float dt)
{
    timer += dt;

    if (timer > current.Duration)
        timer = 0.0f;

    for (int i = 0; i < current.Bones.size(); i++)
    {
        auto& keys = current.Bones[i].Keys;

        if (keys.empty())
            continue;

        KeyFrame k = keys[0];

        XMMATRIX T = XMMatrixTranslation(
            k.Position.x,
            k.Position.y,
            k.Position.z);

        XMMATRIX R = XMMatrixRotationQuaternion(
            XMLoadFloat4(&k.Rotation));

        XMMATRIX S = XMMatrixScaling(
            k.Scale.x,
            k.Scale.y,
            k.Scale.z);

        XMMATRIX local = S * R * T;

        if (parents[i] >= 0)
            finalBones[i] = local * finalBones[parents[i]];
        else
            finalBones[i] = local;
    }
}