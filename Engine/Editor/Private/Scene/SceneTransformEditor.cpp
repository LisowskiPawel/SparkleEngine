#include "PCH.h"

#include "Scene/SceneTransformEditor.h"
#include "Core/Public/Math/MathUtils.h"
#include "GameFramework/Public/Scene/Transform.h"
#include "Widgets/UiDetailsWidgets.h"

static bool EditTranslation(Transform& transform, const float (&defaultPosition)[3]) noexcept
{
	const auto position = transform.GetTranslation();
	float values[3] = {position.x, position.y, position.z};
	if (!EditorWidgets::EditDetailsFloat3("Location", values, 0.05f, -500.0f, 500.0f, "%.3f", defaultPosition))
	{
		return false;
	}

	transform.SetTranslation({values[0], values[1], values[2]});
	return true;
}

static bool EditRotation(Transform& transform) noexcept
{
	const auto degrees = MathUtils::RadiansToDegrees(transform.GetRotationEuler());
	float values[3] = {degrees.x, degrees.y, degrees.z};
	const float defaultRotation[3] = {};
	if (!EditorWidgets::EditDetailsFloat3("Rotation", values, 0.1f, -360.0f, 360.0f, "%.2f", defaultRotation))
	{
		return false;
	}

	transform.SetRotationEuler(MathUtils::DegreesToRadians(DirectX::XMFLOAT3{values[0], values[1], values[2]}));
	return true;
}

static bool EditScale(Transform& transform) noexcept
{
	const auto scale = transform.GetScale();
	float values[3] = {scale.x, scale.y, scale.z};
	const float defaultScale[3] = {1.0f, 1.0f, 1.0f};
	if (!EditorWidgets::EditDetailsFloat3("Scale", values, 0.01f, 0.001f, 100.0f, "%.3f", defaultScale))
	{
		return false;
	}

	transform.SetScale({values[0], values[1], values[2]});
	return true;
}

bool EditSceneTransform(Transform& transform, const float (&defaultPosition)[3]) noexcept
{
	bool changed = EditTranslation(transform, defaultPosition);
	changed |= EditRotation(transform);
	changed |= EditScale(transform);
	return changed;
}
