#include "pch.h"
#include "SelectionRectRenderer.h"
#include "KhanDx/KhanDxComponents.h"


KhanRender::SelectionRectRenderer::SelectionRectRenderer(const Renderer& renderer)
	:
	Renderer(renderer)
{
	m_pVertexBuffer = KhanDx::CreateVertexBuffer(m_pDevice.Get(), vertices, sizeof(vertices));
	m_pPixelShader = KhanDx::CreatePixelShader(m_pDevice.Get(), "PixelShader");

	ComPtr<ID3DBlob> pBlob = KhanDx::CreateShaderBlob("VertexShader");
	m_pVertexShader = KhanDx::CreateVertexShader(m_pDevice.Get(), pBlob.Get());
	m_pInputLayout = KhanDx::CreateInputLayout(m_pDevice.Get(), pBlob.Get(), elementDescs, ARRAYSIZE(elementDescs));

	m_pVSDynConstBuf = KhanDx::CreateDynConstBuf(m_pDevice.Get(), sizeof(DirectX::XMFLOAT4X4), 1);

	m_pBlendState = KhanDx::CreateBlendState_Alpha(m_pDevice.Get());
	m_pRasterizerState = KhanDx::CreateRasterizerState_Solid(m_pDevice.Get());
	m_pDepthStencilState = KhanDx::CreateDepthStencilState_Default(m_pDevice.Get());
}

void KhanRender::SelectionRectRenderer::Render()
{
	UINT Stride = sizeof(Vertex);
	UINT offset{};
	m_pDeviceContext->IASetVertexBuffers(0U, 1U, m_pVertexBuffer.GetAddressOf(), &Stride, &offset);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());

	m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);
	m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);

	m_pDeviceContext->VSSetConstantBuffers(0U, 1U, m_pVSDynConstBuf.GetAddressOf());

	m_pDeviceContext->RSSetState(m_pRasterizerState.Get());
	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 1U);
	m_pDeviceContext->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xFFFFFFFF);
	m_pDeviceContext->Draw(ARRAYSIZE(vertices), 0U);
}

void KhanRender::SelectionRectRenderer::Update(DirectX::XMINT4 const& rect, UINT m_screenWidth, UINT m_screenHeight)
{
	using namespace DirectX;

	int x1 = rect.x;
	int y1 = rect.y;
	int x2 = rect.z;
	int y2 = rect.w;
	// normalize the rectangle (x1 < x2, y1 < y2) 
	if (x1 > x2)
	{
		std::swap(x1, x2);
	}

	if (y1 > y2)
	{
		std::swap(y1, y2);
	}

	float screen_w = static_cast<float>(m_screenWidth);
	float screen_h = static_cast<float>(m_screenHeight);

	// max value of (x2 - x1) is (screen width - 1), so adjust to have max value of screen width
	float rect_w = static_cast<float>(x2 - x1 + 1);

	// max value of (y2 - y1) is (screen height - 1), so adjust to have max value of screen height
	float rect_h = static_cast<float>(y2 - y1 + 1);

	rect_w = rect_w * 2 / screen_w;
	rect_h = rect_h * 2 / screen_h;
	float pos_x = static_cast<float>(x1) * 2 / screen_w - 1.0F;
	float pos_y = 1.0f - static_cast<float>(y1) * 2 / screen_h;

	XMFLOAT4X4 rectTransform	// row major matrix transposed, because hlsl use column major matrix.
	{							// data to vertex shader constant buffer.
		rect_w, 0.0f,   0.0f,  pos_x,
		0.0f,   rect_h, 0.0f,  pos_y,
		0.0f,   0.0f,   1.0f,  0.0f,
		0.0f,   0.0f,   0.0f,  1.0f,
	};

	// Update vertex shader dynamic constant buffer.
	D3D11_MAPPED_SUBRESOURCE mappedResource_vs{};
	m_pDeviceContext->Map(m_pVSDynConstBuf.Get(), 0U, D3D11_MAP_WRITE_DISCARD, 0U, &mappedResource_vs);
	::memcpy(mappedResource_vs.pData, &rectTransform, sizeof(rectTransform));
	m_pDeviceContext->Unmap(m_pVSDynConstBuf.Get(), 0U);
}
