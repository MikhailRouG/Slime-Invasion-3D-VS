#include "collision.h"
#include "direct3d.h"
#include "texture.h"
#include "shader2d.h"
using namespace DirectX;
#include <algorithm>


static constexpr int NUM_VERTEX = 5000;


static ID3D11Buffer* g_pVertexBuffer = nullptr;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static int g_WhiteTex = -1;


struct Vertex
{
	XMFLOAT3 position;

	XMFLOAT4 color;

	XMFLOAT2 texcoord;
};
bool Collision_IsOverlapSphere(const Sphere& a, const Sphere& b)
{
	XMVECTOR ac = XMLoadFloat3(&a.center);
	XMVECTOR bc = XMLoadFloat3(&b.center);
	XMVECTOR lsq = XMVector3LengthSq(bc - ac);

	return (a.radius + b.radius) * (a.radius + b.radius) > XMVectorGetX(lsq);
}

bool Collision_SphereContact(const Sphere& a, const Sphere& b, DirectX::XMFLOAT3& outPoint, DirectX::XMFLOAT3& outNormal, float& outPenetration)
{
	XMVECTOR ac = XMLoadFloat3(&a.center);
	XMVECTOR bc = XMLoadFloat3(&b.center);

	XMVECTOR d = bc - ac;
	float dist = XMVectorGetX(XMVector3Length(d));
	float r = a.radius + b.radius;

	if (dist >= r) return false;

	XMVECTOR n = XMVector3Normalize(d);

	XMVECTOR p = ac + n * a.radius;

	XMStoreFloat3(&outPoint, p);
	XMStoreFloat3(&outNormal, n);
	outPenetration = r - dist;

	return true;
}

bool Collision_IsOverlapSphere(const Sphere& a, const DirectX::XMFLOAT3& point)
{
	XMVECTOR ac = XMLoadFloat3(&a.center);
	XMVECTOR bc = XMLoadFloat3(&point);
	XMVECTOR lsq = XMVector3LengthSq(bc - ac);

	return a.radius *a.radius > XMVectorGetX(lsq);
}

bool Collision_IsOverlapCircle(const Circle& a, const Circle& b){
	float x1 = b.center.x - a.center.x;
	float y1 = b.center.y - a.center.y;
	
	return (a.radius + b.radius) * (a.radius + b.radius) > (x1 * x1 + y1 * y1);

}

bool Collision_IsOverlapBox(const Box& a, const Box& b){
	float at = a.center.y - a.half_height; //top
	float ab = a.center.y + a.half_height; //bottom
	float al = a.center.x - a.half_width; //left
	float ar = a.center.x + a.half_width; //right

	float bt = b.center.y - b.half_height; //top
	float bb = b.center.y + b.half_height; //bottom
	float bl = b.center.x - b.half_width; //left
	float br = b.center.x + b.half_width; //right

	return al<br && ar>bl && at<bb && ab>bt;
}

bool Collision_IsOverlapCircleVSBox(const Box& box, const Circle& circle){
	float closest_x = std::max(box.center.x - box.half_width, std::min(circle.center.x, box.center.x + box.half_width));
	float closest_y = std::max(box.center.y - box.half_height, std::min(circle.center.y, box.center.y + box.half_height));

	float distance_x = circle.center.x - closest_x;
	float distance_y = circle.center.y - closest_y;
	float distance_sq = (distance_x * distance_x) + (distance_y * distance_y); 
	return distance_sq < (circle.radius * circle.radius);
}

bool Collision_IsOverlapOBBVSCircle(const OBB& obb, const Circle& circle)
{
	XMVECTOR obb_center = XMLoadFloat2(&obb.center);
	XMVECTOR circle_center = XMLoadFloat2(&circle.center);
	XMVECTOR vec_to_circle = circle_center - obb_center;

	XMVECTOR obb_axis0 = XMLoadFloat2(&obb.axis[0]);
	XMVECTOR obb_axis1 = XMLoadFloat2(&obb.axis[1]);

	XMFLOAT2 circle_center_local{};
	circle_center_local.x = XMVectorGetX(XMVector2Dot(vec_to_circle, obb_axis0));
	circle_center_local.y = XMVectorGetY(XMVector2Dot(vec_to_circle, obb_axis1));


	float closest_x = std::max(-obb.half_extent.x, std::min(circle_center_local.x, obb.half_extent.x));
	float closest_y = std::max(-obb.half_extent.y, std::min(circle_center_local.y, obb.half_extent.y));


	float distance_x = circle_center_local.x - closest_x;
	float distance_y = circle_center_local.y - closest_y;
	float distance_sq = (distance_x * distance_x) + (distance_y * distance_y);

	return distance_sq < (circle.radius * circle.radius);
}

bool Collision_IsOverlapOBBVSBox(const OBB& obb, const Box& box){
	OBB box_as_obb{};
	box_as_obb.center = box.center;
	box_as_obb.half_extent = { box.half_width, box.half_height };
	box_as_obb.axis[0] = { 1.0f, 0.0f };
	box_as_obb.axis[1] = { 0.0f, 1.0f };

	return Collision_IsOverlapOBB(obb, box_as_obb);
}

bool Collision_IsOverlapOBB(const OBB& a, const OBB& b){
	DirectX::XMFLOAT2 axes[4] = {
		a.axis[0], a.axis[1],
		b.axis[0], b.axis[1]
	};

	for (int i = 0; i < 4; ++i) {
		float a_min, a_max, b_min, b_max;
		ProjectOBB(&a_min, &a_max, a, axes[i]);
		ProjectOBB(&b_min, &b_max, b, axes[i]);

		if (a_max < b_min || b_max < a_min) {
			return false;
		}
	}

	return true;
}

static void ProjectOBB(float* min, float* max, const OBB& obb, const DirectX::XMFLOAT2& axis){
	float p = obb.center.x * axis.x + obb.center.y * axis.y;
	float r = obb.half_extent.x * (float)fabs(obb.axis[0].x * axis.x + obb.axis[0].y * axis.y) +
		obb.half_extent.y * (float)fabs(obb.axis[1].x * axis.x + obb.axis[1].y * axis.y);
	*min = p - r;
	*max = p + r;
}

bool Collision_IsOverlapAABB(const AABB& a, const AABB& b) {
	return a.min.x < b.max.x
		&& a.max.x > b.min.x
		&& a.min.y < b.max.y
		&& a.max.y > b.min.y
		&& a.min.z < b.max.z
		&& a.max.z > b.min.z;
}

Hit Collision_IsHitAABB(const AABB& a, const AABB& b){
	Hit hit;

	hit.isHit = Collision_IsOverlapAABB(a,b);

	if (!hit.isHit) return hit;

	float xdepth = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
	float ydepth = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
	float zdepth = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

	bool isShallowX = false;
	bool isShallowY = false;
	bool isShallowZ = false;

	if (xdepth > ydepth) {
		if (zdepth > ydepth) {
			isShallowY = true;
		}
		else {
			isShallowZ = true;
		}
	}
	else {
		if (zdepth > xdepth) {
			isShallowX = true;
		}
		else {
			isShallowZ = true;
		}
	}

	XMFLOAT3 a_center = a.GetCenter();
	XMFLOAT3 b_center = b.GetCenter();

	hit.normal = { 0,0,0 };

	if (isShallowX) {
		hit.normal.x = (b_center.x > a_center.x) ? 1.0f : -1.0f;
	}
	else if (isShallowY) {
		hit.normal.y = (b_center.y > a_center.y) ? 1.0f : -1.0f;
	}
	else {
		hit.normal.z = (b_center.z > a_center.z) ? 1.0f : -1.0f;
	}
	return hit;
}

void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext){
	g_pDevice = pDevice;
	g_pContext = pContext;
	
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);


	g_WhiteTex = Texture_Load(L"resource/texture/white.png");
	
}

void Collision_DebugFinalize(){
	SAFE_RELEASE(g_pVertexBuffer);
}

void Collision_DebugDraw(const Circle& circle, const DirectX::XMFLOAT4& color){
	int NumVertex = (int)(circle.radius * 2.0f + XM_PI + 1);

	Shader2d_Begin();

	float screen_x = circle.center.x;
	float screen_y = circle.center.y;

	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	Vertex* v = (Vertex*)msr.pData;

	const float rad = XM_2PI / NumVertex;

	for (int i = 0;i < NumVertex; i++) {
		v[i].position.x = cosf(rad * i) * circle.radius + screen_x;
		v[i].position.y = sinf(rad * i) * circle.radius + screen_y;
		v[i].position.z = 0.0f;
		v[i].color = color;
		v[i].texcoord = { 0.0f,0.0f };
	}

	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader2d_SetWorldMatrix(XMMatrixIdentity()); 

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	Texture_SetTexture(g_WhiteTex);

	g_pContext->Draw(NumVertex, 0);

}

void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color){
	Shader2d_Begin();

	float screen_x = box.center.x;
	float screen_y = box.center.y;

	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	Vertex* v = (Vertex*)msr.pData;

	v[0].position = { screen_x - box.half_width , screen_y - box.half_height, 0.0f };
	v[1].position = { screen_x + box.half_width , screen_y - box.half_height, 0.0f };
	v[2].position = { screen_x + box.half_width , screen_y + box.half_height, 0.0f };
	v[3].position = { screen_x - box.half_width , screen_y + box.half_height, 0.0f };
	v[4].position = { screen_x - box.half_width , screen_y - box.half_height, 0.0f };


	for (int i = 0;i < 5;i++) {
		v[i].color = color;
		v[i].texcoord = { 0.0f,0.0f };
	}

	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader2d_SetWorldMatrix(XMMatrixIdentity());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	Texture_SetTexture(g_WhiteTex);

	g_pContext->Draw(5, 0);
}

void Collision_DebugDraw(const OBB& obb, const DirectX::XMFLOAT4& color) {
	XMVECTOR half_extent_x = XMLoadFloat2(&obb.axis[0]) * obb.half_extent.x;
	XMVECTOR half_extent_y = XMLoadFloat2(&obb.axis[1]) * obb.half_extent.y;
	XMVECTOR center = XMLoadFloat2(&obb.center);

	XMFLOAT2 corners[4]{};
	XMStoreFloat2(&corners[0], center - half_extent_x - half_extent_y);
	XMStoreFloat2(&corners[1], center + half_extent_x - half_extent_y);
	XMStoreFloat2(&corners[2], center + half_extent_x + half_extent_y);
	XMStoreFloat2(&corners[3], center - half_extent_x + half_extent_y);


	static constexpr int NumVertex = 5;
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	Vertex* v = (Vertex*)msr.pData;

	v[0].position = { corners[0].x, corners[0].y, 0.0f };
	v[1].position = { corners[1].x, corners[1].y, 0.0f };
	v[2].position = { corners[2].x, corners[2].y, 0.0f };
	v[3].position = { corners[3].x, corners[3].y, 0.0f };
	v[4].position = { corners[0].x, corners[0].y, 0.0f };

	for (int i = 0; i < NumVertex; ++i) {
		v[i].color = color;
		v[i].texcoord = { 0.0f, 0.0f };
	}
	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader2d_Begin();
	Shader2d_SetWorldMatrix(XMMatrixIdentity());
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	Texture_SetTexture(-1);
	g_pContext->Draw(NumVertex, 0);
}