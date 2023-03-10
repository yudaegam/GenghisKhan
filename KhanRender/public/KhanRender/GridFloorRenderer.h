#pragma once
#include "KhanRender/NearPlaneRenderer.h"
#include <DirectXMath.h>

namespace KhanRender
{
	class GridFloorRenderer : public NearPlaneRenderer
	{
	public:
		GridFloorRenderer(const NearPlaneRenderer& renderer);
		void Render();
		void Update(DirectX::XMMATRIX const& inverseViewProjMat);

	private:
		ComPtr<ID3D11PixelShader>	m_pPixelShader;
		ComPtr<ID3D11Buffer>		m_pPSDynConstBuf;
	};
}

