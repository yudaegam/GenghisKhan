#pragma once
#include <DirectXMath.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;


namespace KhanECS::Component
{
	struct EntityName {};
	struct Position : XMFLOAT3
	{
		constexpr Position() noexcept = default;
		constexpr Position(DirectX::XMFLOAT3 pos) noexcept : XMFLOAT3(pos) {}
		constexpr Position(float x, float y, float z) noexcept : XMFLOAT3(x, y, z) {}
	};
	struct Rotation : XMFLOAT3
	{
		constexpr Rotation() noexcept = default;
		constexpr Rotation(DirectX::XMFLOAT3 rot) noexcept : XMFLOAT3(rot) {}
		constexpr Rotation(float x, float y, float z) noexcept : XMFLOAT3(x, y, z) {}
	};
}