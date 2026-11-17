#include "PCH.h"

#include "GameFramework/Public/Scene/Animations/SkeletonTransformContract.h"

#include <cmath>

namespace SkeletonTransformContract
{
	bool IsFinite(const DirectX::XMFLOAT4X4& matrix) noexcept
	{
		const float* elements = &matrix._11;
		for (std::uint32_t element = 0; element < 16u; ++element)
		{
			if (!std::isfinite(elements[element]))
			{
				return false;
			}
		}
		return true;
	}

	bool IsInvertible(const DirectX::XMFLOAT4X4& matrix, float epsilon) noexcept
	{
		DirectX::XMVECTOR determinant;
		DirectX::XMMatrixInverse(&determinant, DirectX::XMLoadFloat4x4(&matrix));
		const float value = DirectX::XMVectorGetX(determinant);
		return std::isfinite(value) && std::abs(value) > epsilon;
	}

	bool IsTrsDecomposable(const DirectX::XMFLOAT4X4& matrix) noexcept
	{
		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR translation;
		return DirectX::XMMatrixDecompose(&scale, &rotation, &translation, DirectX::XMLoadFloat4x4(&matrix)) && DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation)) > 1.0e-8f;
	}

	bool MatricesNear(DirectX::FXMMATRIX lhs, DirectX::CXMMATRIX rhs, float epsilon) noexcept
	{
		DirectX::XMFLOAT4X4 lhsValues;
		DirectX::XMFLOAT4X4 rhsValues;
		DirectX::XMStoreFloat4x4(&lhsValues, lhs);
		DirectX::XMStoreFloat4x4(&rhsValues, rhs);
		const float* lhsElements = &lhsValues._11;
		const float* rhsElements = &rhsValues._11;
		for (std::uint32_t element = 0; element < 16u; ++element)
		{
			if (std::abs(lhsElements[element] - rhsElements[element]) > epsilon)
			{
				return false;
			}
		}
		return true;
	}

	bool SatisfiesBindInvariant(const DirectX::XMFLOAT4X4& bindLocal, const DirectX::XMFLOAT4X4& parentSpace, const DirectX::XMFLOAT4X4* parentBindModel, const DirectX::XMFLOAT4X4& bindModel) noexcept
	{
		DirectX::XMMATRIX reconstructed = DirectX::XMLoadFloat4x4(&bindLocal) * DirectX::XMLoadFloat4x4(&parentSpace);
		if (parentBindModel != nullptr)
		{
			reconstructed *= DirectX::XMLoadFloat4x4(parentBindModel);
		}
		return MatricesNear(reconstructed, DirectX::XMLoadFloat4x4(&bindModel));
	}
}
