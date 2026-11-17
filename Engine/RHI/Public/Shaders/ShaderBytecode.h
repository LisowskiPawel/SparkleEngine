#pragma once

#include <cstddef>

// Non-owning view of cooked shader bytes. The owning library must outlive the view.
struct ShaderBytecode
{
	const void* Data = nullptr;
	std::size_t Size = 0;

	bool IsValid() const noexcept { return Data != nullptr && Size > 0; }

	explicit operator bool() const noexcept { return IsValid(); }
};
