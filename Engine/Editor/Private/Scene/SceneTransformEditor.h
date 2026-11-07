#pragma once

class Transform;

namespace SceneTransformEditor
{
	bool Edit(Transform& transform, const float (&defaultPosition)[3]) noexcept;
}
