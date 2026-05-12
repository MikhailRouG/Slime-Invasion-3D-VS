#include "SkinnedModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
SkinnedModel::SkinnedModel()
{
    m_device = nullptr;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_boneBuffer = nullptr;

    m_scene = nullptr;
    m_boneCount = 0;

    m_globalInverseTransform = XMMatrixIdentity();

    m_animationTime = 0.0f;
    m_currentAnimation = 0;
}

SkinnedModel::~SkinnedModel()
{
    Release();
}

bool SkinnedModel::Init(ID3D11Device* device)
{
    m_device = device;

    D3D11_BUFFER_DESC boneDesc = {};
    boneDesc.Usage = D3D11_USAGE_DEFAULT;
    boneDesc.ByteWidth = sizeof(XMMATRIX) * MAX_BONES;
    boneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    HRESULT hr = m_device->CreateBuffer(&boneDesc, nullptr, &m_boneBuffer);

    return SUCCEEDED(hr);
}

bool SkinnedModel::Load(const std::string& path)
{
    ClearModel();

    if (!LoadScene(path))
        return false;

    ProcessNode(m_scene->mRootNode, m_scene);

    CreateBuffers();

    return true;
}

bool SkinnedModel::LoadScene(const std::string& path)
{
    m_scene = m_importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights |
        aiProcess_ConvertToLeftHanded
    );

    if (!m_scene || !m_scene->mRootNode)
        return false;

    m_globalInverseTransform = XMMatrixInverse(
        nullptr,
        AiToXM(m_scene->mRootNode->mTransformation)
    );

    return true;
}

void SkinnedModel::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void SkinnedModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    unsigned int baseVertex = (unsigned int)m_vertices.size();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position = XMFLOAT3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        if (mesh->HasNormals())
        {
            vertex.normal = XMFLOAT3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }

        if (mesh->HasTextureCoords(0))
        {
            vertex.texcoord = XMFLOAT2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }

        m_vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();

        int boneIndex = 0;

        if (m_boneMapping.find(boneName) == m_boneMapping.end())
        {
            boneIndex = m_boneCount;
            m_boneCount++;

            BoneInfo boneInfo;
            boneInfo.offsetMatrix = AiToXM(bone->mOffsetMatrix);

            m_boneInfo.push_back(boneInfo);
            m_boneMapping[boneName] = boneIndex;
        }
        else
        {
            boneIndex = m_boneMapping[boneName];
        }

        for (unsigned int j = 0; j < bone->mNumWeights; j++)
        {
            unsigned int vertexID = baseVertex + bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            AddBoneData(m_vertices[vertexID], boneIndex, weight);
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            m_indices.push_back(face.mIndices[j] + baseVertex);
        }
    }
}

void SkinnedModel::AddBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < 4; i++)
    {
        if (vertex.weights[i] == 0.0f)
        {
            vertex.boneIDs[i] = boneID;
            vertex.weights[i] = weight;
            return;
        }
    }
}

void SkinnedModel::CreateBuffers()
{
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }

    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = UINT(sizeof(Vertex) * m_vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = m_vertices.data();

    m_device->CreateBuffer(&vbDesc, &vbData, &m_vertexBuffer);

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = UINT(sizeof(unsigned int) * m_indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = m_indices.data();

    m_device->CreateBuffer(&ibDesc, &ibData, &m_indexBuffer);
}

void SkinnedModel::Update(float deltaTime)
{
    UpdateAnimation(deltaTime);
}

void SkinnedModel::UpdateAnimation(float deltaTime)
{
    if (!m_scene || !m_scene->HasAnimations())
        return;

    aiAnimation* animation = m_scene->mAnimations[m_currentAnimation];

    float ticksPerSecond = animation->mTicksPerSecond != 0.0
        ? (float)animation->mTicksPerSecond
        : 25.0f;

    m_animationTime += deltaTime * ticksPerSecond;

    float timeInTicks = fmod(m_animationTime, (float)animation->mDuration);

    ReadNodeHierarchy(timeInTicks, m_scene->mRootNode, XMMatrixIdentity());
}

void SkinnedModel::ReadNodeHierarchy(float animationTime, aiNode* node, const XMMATRIX& parentTransform)
{
    std::string nodeName = node->mName.C_Str();

    const aiAnimation* animation = m_scene->mAnimations[m_currentAnimation];

    XMMATRIX nodeTransform = AiToXM(node->mTransformation);

    aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

    if (nodeAnim)
    {
        XMMATRIX scaling = InterpolateScaling(animationTime, nodeAnim);
        XMMATRIX rotation = InterpolateRotation(animationTime, nodeAnim);
        XMMATRIX translation = InterpolatePosition(animationTime, nodeAnim);

        nodeTransform = scaling * rotation * translation;
    }

    XMMATRIX globalTransform = nodeTransform * parentTransform;

    if (m_boneMapping.find(nodeName) != m_boneMapping.end())
    {
        int boneIndex = m_boneMapping[nodeName];

        m_boneInfo[boneIndex].finalMatrix =
            m_boneInfo[boneIndex].offsetMatrix *
            globalTransform *
            m_globalInverseTransform;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
    }
}
aiNodeAnim* SkinnedModel::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
{
    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        aiNodeAnim* nodeAnim = animation->mChannels[i];

        if (nodeName == nodeAnim->mNodeName.C_Str())
            return nodeAnim;
    }

    return nullptr;
}

unsigned int SkinnedModel::FindPosition(float animationTime, aiNodeAnim* nodeAnim)
{
    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
    {
        if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

unsigned int SkinnedModel::FindRotation(float animationTime, aiNodeAnim* nodeAnim)
{
    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
    {
        if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

unsigned int SkinnedModel::FindScaling(float animationTime, aiNodeAnim* nodeAnim)
{
    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
    {
        if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

XMMATRIX SkinnedModel::InterpolatePosition(float animationTime, aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumPositionKeys == 1)
    {
        aiVector3D pos = nodeAnim->mPositionKeys[0].mValue;

        return XMMatrixTranslation(pos.x, pos.y, pos.z);
    }

    unsigned int index = FindPosition(animationTime, nodeAnim);
    unsigned int nextIndex = index + 1;

    float deltaTime =
        (float)(nodeAnim->mPositionKeys[nextIndex].mTime -
            nodeAnim->mPositionKeys[index].mTime);

    float factor =
        (animationTime -
            (float)nodeAnim->mPositionKeys[index].mTime) / deltaTime;

    aiVector3D start = nodeAnim->mPositionKeys[index].mValue;
    aiVector3D end = nodeAnim->mPositionKeys[nextIndex].mValue;

    aiVector3D delta = end - start;
    aiVector3D result = start + factor * delta;

    return XMMatrixTranslation(result.x, result.y, result.z);
}

XMMATRIX SkinnedModel::InterpolateRotation(float animationTime, aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumRotationKeys == 1)
    {
        aiQuaternion q = nodeAnim->mRotationKeys[0].mValue;

        XMVECTOR quat = XMVectorSet(q.x, q.y, q.z, q.w);

        return XMMatrixRotationQuaternion(quat);
    }

    unsigned int index = FindRotation(animationTime, nodeAnim);
    unsigned int nextIndex = index + 1;

    float deltaTime =
        (float)(nodeAnim->mRotationKeys[nextIndex].mTime -
            nodeAnim->mRotationKeys[index].mTime);

    float factor =
        (animationTime -
            (float)nodeAnim->mRotationKeys[index].mTime) / deltaTime;

    aiQuaternion start = nodeAnim->mRotationKeys[index].mValue;
    aiQuaternion end = nodeAnim->mRotationKeys[nextIndex].mValue;

    aiQuaternion result;
    aiQuaternion::Interpolate(result, start, end, factor);
    result.Normalize();

    XMVECTOR quat = XMVectorSet(
        result.x,
        result.y,
        result.z,
        result.w
    );

    return XMMatrixRotationQuaternion(quat);
}

XMMATRIX SkinnedModel::InterpolateScaling(float animationTime, aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumScalingKeys == 1)
    {
        aiVector3D scale = nodeAnim->mScalingKeys[0].mValue;

        return XMMatrixScaling(scale.x, scale.y, scale.z);
    }

    unsigned int index = FindScaling(animationTime, nodeAnim);
    unsigned int nextIndex = index + 1;

    float deltaTime =
        (float)(nodeAnim->mScalingKeys[nextIndex].mTime -
            nodeAnim->mScalingKeys[index].mTime);

    float factor =
        (animationTime -
            (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;

    aiVector3D start = nodeAnim->mScalingKeys[index].mValue;
    aiVector3D end = nodeAnim->mScalingKeys[nextIndex].mValue;

    aiVector3D delta = end - start;
    aiVector3D result = start + factor * delta;

    return XMMatrixScaling(result.x, result.y, result.z);
}

void SkinnedModel::Play(const std::string& animationName)
{
    if (!m_scene || !m_scene->HasAnimations())
        return;

    for (unsigned int i = 0; i < m_scene->mNumAnimations; i++)
    {
        std::string name = m_scene->mAnimations[i]->mName.C_Str();

        if (name == animationName)
        {
            m_currentAnimation = i;
            m_animationTime = 0.0f;
            return;
        }
    }
}

void SkinnedModel::Play(int index)
{
    if (!m_scene) return;

    if (index < 0 || index >= (int)m_scene->mNumAnimations)
        return;

    m_currentAnimation = index;
    m_animationTime = 0.0f;
}

int SkinnedModel::GetAnimationCount() const
{
    if (!m_scene) return 0;

    return (int)m_scene->mNumAnimations;
}

std::string SkinnedModel::GetAnimationName(int index) const
{
    if (!m_scene) return "";

    if (index < 0 || index >= (int)m_scene->mNumAnimations)
        return "";

    return m_scene->mAnimations[index]->mName.C_Str();
}
XMMATRIX SkinnedModel::AiToXM(const aiMatrix4x4& m)
{
    return XMMATRIX(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

void SkinnedModel::Draw(ID3D11DeviceContext* context)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(
        0, 1,
        &m_vertexBuffer,
        &stride,
        &offset
    );

    context->IASetIndexBuffer(
        m_indexBuffer,
        DXGI_FORMAT_R32_UINT,
        0
    );

    XMMATRIX bones[MAX_BONES];

    for (int i = 0; i < MAX_BONES; i++)
        bones[i] = XMMatrixIdentity();

    for (int i = 0; i < m_boneCount; i++)
        bones[i] = XMMatrixTranspose(
            m_boneInfo[i].finalMatrix
        );

    context->UpdateSubresource(
        m_boneBuffer,
        0,
        nullptr,
        bones,
        0,
        0
    );

    context->VSSetConstantBuffers(
        4, 1,
        &m_boneBuffer
    );

    context->DrawIndexed(
        (UINT)m_indices.size(),
        0,
        0
    );
}

void SkinnedModel::ClearModel()
{
    m_vertices.clear();
    m_indices.clear();

    m_boneInfo.clear();
    m_boneMapping.clear();

    m_boneCount = 0;
    m_animationTime = 0.0f;
}

void SkinnedModel::Release()
{
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }

    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    if (m_boneBuffer)
    {
        m_boneBuffer->Release();
        m_boneBuffer = nullptr;
    }

    ClearModel();
}