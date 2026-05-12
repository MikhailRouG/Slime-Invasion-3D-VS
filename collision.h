#ifndef COLLISION_H
#define COLLISION_H

#include <d3d11.h>
#include <DirectXMath.h>

struct Sphere {
	DirectX::XMFLOAT3 center;
	float radius;
};


struct Circle {
	DirectX::XMFLOAT2 center; 
	float radius;
};

struct Box {
	DirectX::XMFLOAT2 center;
	float half_width; 
	float half_height;
};

struct AABB {
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;

	DirectX::XMFLOAT3 GetCenter() const {
		return {
			(min.x + max.x) * 0.5f,
			(min.y + max.y) * 0.5f,
			(min.z + max.z) * 0.5f
		};
	}
	DirectX::XMFLOAT3 GetHalf() const {
		DirectX::XMFLOAT3 half;
		if (this == nullptr) {
			return half;
		}

		half.x = (max.x - min.x) *0.5f;
		half.y = (max.y - min.y) * 0.5f;
		half.z = (max.z - min.z) * 0.5f;
		return half;
	}
};

struct Hit {
	bool isHit;
	DirectX::XMFLOAT3 normal;
};

struct OBB {
	DirectX::XMFLOAT2 center;
	DirectX::XMFLOAT2 half_extent; 
	DirectX::XMFLOAT2 axis[2]; 
};
bool Collision_IsOverlapSphere(const Sphere& a, const Sphere& b);
bool Collision_SphereContact(const Sphere& a, const Sphere& b, DirectX::XMFLOAT3& outPoint, DirectX::XMFLOAT3& outNormal, float& outPenetration);
bool Collision_IsOverlapSphere(const Sphere& a, const DirectX::XMFLOAT3& point);
bool Collision_IsOverlapCircle(const Circle& a, const Circle& b);
bool Collision_IsOverlapBox(const Box& a, const Box& b);
bool Collision_IsOverlapCircleVSBox(const Box& box, const Circle& circle);
bool Collision_IsOverlapOBBVSCircle(const OBB& obb, const Circle& circle);
bool Collision_IsOverlapOBBVSBox(const OBB& obb, const Box& box);
bool Collision_IsOverlapOBB(const OBB& a, const OBB& b);

static void ProjectOBB(float* min, float* max, const OBB& obb, const DirectX::XMFLOAT2& axis);

bool Collision_IsOverlapAABB(const AABB& a, const AABB& b);

Hit Collision_IsHitAABB(const AABB& a, const AABB& b);

void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Collision_DebugFinalize();
void Collision_DebugDraw(const Circle& circle, const DirectX::XMFLOAT4& color={1.0f,1.0f,1.0f,1.0f});
void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });
void Collision_DebugDraw(const OBB& obb, const DirectX::XMFLOAT4& color = { 1.0f,1.0f,1.0f,1.0f });



#endif //COLLISION_H