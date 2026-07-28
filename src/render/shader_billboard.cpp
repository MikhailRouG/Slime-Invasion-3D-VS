#include "shader_billboard.h"
#include "direct3d.h"
#include <fstream>
#include <Windows.h>
#include <vector>
static ID3D11VertexShader* g_pVertexShader = nullptr;
static ID3D11InputLayout* g_pInputLayout = nullptr;
static ID3D11Buffer* g_pVSConstantBuffer0 = nullptr;
static ID3D11Buffer* g_pPSConstantBuffer0 = nullptr;
static ID3D11PixelShader* g_pPixelShader = nullptr;

bool ShaderBillboard_Initialize() {
    HRESULT hr;
    ID3D11Device* dev = Direct3D_GetDevice();
    auto ReadCSO = [](const char* filename) -> std::vector<unsigned char> {
        std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
        if (!ifs.is_open()) return {};
        std::vector<unsigned char> buffer(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        ifs.read((char*)buffer.data(), buffer.size());
        return buffer;
        };
    std::ifstream ifs_vs("resource/shader/shader_vertex_billboard.cso", std::ios::binary);
    if (!ifs_vs.is_open()) {
        MessageBox(nullptr, "�� ������ VS ������!", "Error", MB_OK);
        return false;
    }
    ifs_vs.seekg(0, std::ios::end);
    size_t vs_size = (size_t)ifs_vs.tellg();
    ifs_vs.seekg(0, std::ios::beg);
    std::vector<unsigned char> vs_data(vs_size);
    ifs_vs.read((char*)vs_data.data(), vs_size);
    ifs_vs.close();

    hr = dev->CreateVertexShader(vs_data.data(), vs_size, nullptr, &g_pVertexShader);
    if (FAILED(hr)) return false;

    // 2. Input Layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    hr = dev->CreateInputLayout(layout, 3, vs_data.data(), vs_size, &g_pInputLayout);
    if (FAILED(hr)) return false;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    bd.ByteWidth = sizeof(BillboardVSConstant);
    if (bd.ByteWidth % 16 != 0) bd.ByteWidth += 16 - (bd.ByteWidth % 16);

    hr = dev->CreateBuffer(&bd, nullptr, &g_pVSConstantBuffer0);
    if (FAILED(hr)) {
        OutputDebugStringA("!!! ������ �������� VS Constant Buffer !!!\n");
        return false;
    }

    bd.ByteWidth = sizeof(DirectX::XMFLOAT4);
    hr = dev->CreateBuffer(&bd, nullptr, &g_pPSConstantBuffer0);
    if (FAILED(hr)) return false;

    std::ifstream ifs_ps("resource/shader/shader_pixel_billboard.cso", std::ios::binary);
    if (!ifs_ps) return false;
    ifs_ps.seekg(0, std::ios::end);
    size_t ps_size = (size_t)ifs_ps.tellg();
    ifs_ps.seekg(0, std::ios::beg);
    std::vector<unsigned char> ps_data(ps_size);
    ifs_ps.read((char*)ps_data.data(), ps_size);
    ifs_ps.close();

    hr = dev->CreatePixelShader(ps_data.data(), ps_size, nullptr, &g_pPixelShader);
    return SUCCEEDED(hr);
}

void ShaderBillboard_Begin() {
    auto ctx = Direct3D_GetContext();

    if (g_pVertexShader == nullptr) OutputDebugStringA("VS NULL\n");
    if (g_pPixelShader == nullptr) OutputDebugStringA("PS NULL\n");

    if (!g_pVertexShader || !g_pPixelShader) return;

    ctx->VSSetShader(g_pVertexShader, nullptr, 0);
    ctx->PSSetShader(g_pPixelShader, nullptr, 0);
    ctx->IASetInputLayout(g_pInputLayout);

    ctx->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer0);
    ctx->PSSetConstantBuffers(0, 1, &g_pPSConstantBuffer0);
}

void ShaderBillboard_SetVSConstant(const BillboardVSConstant& cb) {
    if (g_pVSConstantBuffer0 == nullptr) return;
    Direct3D_GetContext()->UpdateSubresource(g_pVSConstantBuffer0, 0, nullptr, &cb, 0, 0);
}

void ShaderBillboard_SetColor(const DirectX::XMFLOAT4& color) {
    if (g_pPSConstantBuffer0 == nullptr) return;
    Direct3D_GetContext()->UpdateSubresource(g_pPSConstantBuffer0, 0, nullptr, &color, 0, 0);
}

void ShaderBillboard_Finalize() {
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pInputLayout) g_pInputLayout->Release();
    if (g_pVSConstantBuffer0) g_pVSConstantBuffer0->Release();
    if (g_pPSConstantBuffer0) g_pPSConstantBuffer0->Release();
}