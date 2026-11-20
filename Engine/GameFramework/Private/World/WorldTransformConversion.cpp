#include "PCH.h"
#include "World/WorldTransformConversion.h"

#include "GameFramework/Public/Scene/Transform.h"

namespace ECS::WorldTransformConversion
{
	LocalTransform ToLocal(const Transform& transform) noexcept
	{
		const DirectX::XMFLOAT3 rotation = transform.GetRotationEuler();
		LocalTransform local;
		local.Translation = transform.GetTranslation();
		local.Scale = transform.GetScale();
		const DirectX::XMVECTOR orientation = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMStoreFloat4(&local.Rotation, orientation);
		return local;
	}

	Transform ToPublic(const LocalTransform& transform) noexcept
	{
		const DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(transform.Scale.x, transform.Scale.y, transform.Scale.z);
		const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&transform.Rotation));
		const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(transform.Translation.x, transform.Translation.y, transform.Translation.z);
		return Transform(scale * rotation * translation);
	}

	WorldTransform BuildWorld(const LocalTransform& transform) noexcept
	{
		WorldTransform world;
		const DirectX::XMMATRIX matrix = ToPublic(transform).GetWorldMatrix();
		DirectX::XMStoreFloat4x4(&world.Matrix, matrix);
		const DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, matrix);
		const DirectX::XMMATRIX inverseTranspose = DirectX::XMMatrixTranspose(inverse);
		DirectX::XMStoreFloat4x4(&world.InverseTranspose, inverseTranspose);
		return world;
	}
}
