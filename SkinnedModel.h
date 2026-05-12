#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <string>
#include <vector>
#include <map>

using namespace DirectX;

#define MAX_BONES 128

class SkinnedModel
{
public:
    SkinnedModel();
    ~SkinnedModel();

    bool Init(ID3D11Device* device);
    bool Load(const std::string& path);

    void Update(float deltaTime);
    void Draw(ID3D11DeviceContext* context);

    void Play(const std::string& animationName);
    void Play(int index);

    int GetAnimationCount() const;
    std::string GetAnimationName(int index) const;

    void Release();

private:
    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texcoord;

        int boneIDs[4];
        float weights[4];

        Vertex()
        {
            position = XMFLOAT3(0, 0, 0);
            normal = XMFLOAT3(0, 1, 0);
            texcoord = XMFLOAT2(0, 0);

            for (int i = 0; i < 4; i++)
            {
                boneIDs[i] = 0;
                weights[i] = 0.0f;
            }
        }
    };

    struct BoneInfo
    {
        XMMATRIX offsetMatrix;
        XMMATRIX finalMatrix;

        BoneInfo()
        {
            offsetMatrix = XMMatrixIdentity();
            finalMatrix = XMMatrixIdentity();
        }
    };

private:
    bool LoadScene(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);

    void AddBoneData(Vertex& vertex, int boneID, float weight);

    void UpdateAnimation(float deltaTime);
    void ReadNodeHierarchy(float animationTime, aiNode* node, const XMMATRIX& parentTransform);

    aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);

    XMMATRIX InterpolatePosition(float animationTime, aiNodeAnim* nodeAnim);
    XMMATRIX InterpolateRotation(float animationTime, aiNodeAnim* nodeAnim);
    XMMATRIX InterpolateScaling(float animationTime, aiNodeAnim* nodeAnim);

    unsigned int FindPosition(float animationTime, aiNodeAnim* nodeAnim);
    unsigned int FindRotation(float animationTime, aiNodeAnim* nodeAnim);
    unsigned int FindScaling(float animationTime, aiNodeAnim* nodeAnim);

    XMMATRIX AiToXM(const aiMatrix4x4& matrix);

    void CreateBuffers();
    void ClearModel();

private:
    ID3D11Device* m_device;

    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    ID3D11Buffer* m_boneBuffer;

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    std::map<std::string, int> m_boneMapping;
    std::vector<BoneInfo> m_boneInfo;

    int m_boneCount;

    Assimp::Importer m_importer;
    const aiScene* m_scene;

    XMMATRIX m_globalInverseTransform;

    float m_animationTime;
    int m_currentAnimation;
};