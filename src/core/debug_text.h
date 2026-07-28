#ifndef DEBUG_TEXT_H
#define DEBUG_TEXT_H

#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <tuple>
#include <list>
#include <wrl/client.h>
#include <DirectXMath.h>


namespace hal
{
	class DebugText
	{
	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pContext = nullptr;

		float m_OffsetX{ 0.0f };
		float m_OffsetY{ 0.0f };
		ULONG m_MaxLine{ 0 };
		ULONG m_MaxCharactersPerLine{ 0 };
		float m_LineSpacing{ 0.0f };
		float m_CharacterSpacing{ 0.0f };

		struct Characters { 
			Characters(const DirectX::XMFLOAT4& color) : color(color) {}
			std::string characters; 
			DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct LineStrings {
			std::list<Characters> strings;
			ULONG characterCount{ 0 };
			ULONG spaceCount{ 0 };
		};

		std::list<LineStrings> m_TextLines;
		UINT m_CharacterCount{ 0 };

		std::wstring m_FileName;
		ID3D11Resource* m_pTexture = nullptr;
		ID3D11ShaderResourceView* m_pTextureView = nullptr;
		UINT m_TextureWidth{ 0 };
		UINT m_TextureHeight{ 0 };

		static std::unordered_map<std::wstring, std::tuple<ID3D11Resource*, ID3D11ShaderResourceView*>> m_TextureMap;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
		UINT m_BufferSourceCharacterCount{ 0 };

		static Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
		static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;

		static Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		static Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVSConstantBuffer;
		static Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;

	public:
		DebugText() = delete;
		DebugText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wchar_t* pFontTextureFileName, UINT screenWidth, UINT screenHeight, float offsetX = 0.0f, float offsetY = 0.0f, ULONG maxLine = 0, ULONG maxCharactersPerLine = 0, float lineSpacing = 0.0f, float characterSpacing = 0.0f);
		~DebugText();

		void SetText(const char* pText, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });

		void Draw();

		void Clear();

	private:
		
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texcoord;
		};

		void createBuffer(ULONG characterCount);
	};
}
#endif // DEBUG_TEXT_H