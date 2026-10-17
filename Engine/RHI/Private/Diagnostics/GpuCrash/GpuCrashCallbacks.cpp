#include "PCH.h"
#include "Diagnostics/GpuCrash/GpuCrashSession.h"

#include <algorithm>
#include <cstring>
#include <format>

void GFSDK_AFTERMATH_CALL GpuCrashSession::DumpCallback(const void* data, std::uint32_t size, void* user) noexcept
{
	auto& session = *static_cast<GpuCrashSession*>(user);
	try
	{
		if (data == nullptr || size == 0)
		{
			session.Incomplete();
			return;
		}
		session.Enqueue({RhiGpuCrashArtifactKind::Dump}, {static_cast<const std::byte*>(data), size});
	}
	catch (...)
	{
		session.Incomplete();
	}
}

void GFSDK_AFTERMATH_CALL GpuCrashSession::DebugCallback(const void* data, std::uint32_t size, void* user) noexcept
{
	auto& session = *static_cast<GpuCrashSession*>(user);
	try
	{
		GFSDK_Aftermath_ShaderDebugInfoIdentifier id{};
		if (data == nullptr || size == 0 || session.m_debugIdentifier(GFSDK_Aftermath_Version_API, data, size, &id) != GFSDK_Aftermath_Result_Success)
		{
			session.Incomplete();
			return;
		}
		session.Enqueue({RhiGpuCrashArtifactKind::ShaderDebugInfo, id.id[0], id.id[1]}, {static_cast<const std::byte*>(data), size});
	}
	catch (...)
	{
		session.Incomplete();
	}
}

void GFSDK_AFTERMATH_CALL GpuCrashSession::DescriptionCallback(PFN_GFSDK_Aftermath_AddGpuCrashDumpDescription add, void* user) noexcept
{
	try
	{
		add(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_ApplicationName, "Sparkle");
		add(GFSDK_Aftermath_GpuCrashDumpDescriptionKey_UserDefined, static_cast<GpuCrashSession*>(user)->m_incidentId.data());
	}
	catch (...)
	{
		static_cast<GpuCrashSession*>(user)->Incomplete();
	}
}

void GFSDK_AFTERMATH_CALL GpuCrashSession::MarkerCallback(const void* marker, std::uint32_t size, void* user, PFN_GFSDK_Aftermath_ResolveMarker resolve) noexcept
{
	auto& session = *static_cast<GpuCrashSession*>(user);
	try
	{
		// Only Vulkan tokens need resolution. D3D12 supplies copied marker bytes.
		if (size != 0)
		{
			return;
		}
		std::array<char, 128> copy{};
		const auto text = session.MarkerText(marker);
		std::copy(text.begin(), text.end(), copy.begin());
		resolve(copy.data(), static_cast<std::uint32_t>(strnlen(copy.data(), copy.size())));
	}
	catch (...)
	{
		session.Incomplete();
	}
}

const void* GpuCrashSession::Marker(std::string_view label, std::uint32_t parent, char operation, std::uint32_t& id) noexcept
{
	id = 0;
	try
	{
		if (label.size() >= m_unknown.Label.size() || label.empty())
		{
			Incomplete();
			return &m_unknown;
		}
		std::lock_guard lock(m_markerMutex);
		for (std::size_t index = 0; index < m_markerCount; ++index)
		{
			const auto& entry = m_markers[index];
			if (entry.Parent == parent && entry.Operation == operation && label == entry.Label.data())
			{
				id = static_cast<std::uint32_t>(index + 1);
				return &entry;
			}
		}
		if (m_markerCount == MaximumMarkers)
		{
			Incomplete();
			return &m_unknown;
		}
		auto& entry = m_markers[m_markerCount];
		entry = {};
		entry.Parent = parent;
		entry.Operation = operation;
		std::copy(label.begin(), label.end(), entry.Label.begin());
		const auto markerId = static_cast<std::uint32_t>(m_markerCount + 1);
		const auto text = std::format("{} id={} parent={} {}", operation, markerId, parent, label);
		if (text.size() >= entry.Text.size())
		{
			Incomplete();
			id = 0;
			return &m_unknown;
		}
		std::copy(text.begin(), text.end(), entry.Text.begin());
		++m_markerCount;
		id = markerId;
		return &entry;
	}
	catch (...)
	{
		Incomplete();
		return &m_unknown;
	}
}

std::string_view GpuCrashSession::MarkerText(const void* token) const noexcept
{
	std::lock_guard lock(m_markerMutex);
	const auto address = reinterpret_cast<std::uintptr_t>(token);
	const auto first = reinterpret_cast<std::uintptr_t>(m_markers.get());
	if (address >= first && address - first < m_markerCount * sizeof(MarkerEntry) && (address - first) % sizeof(MarkerEntry) == 0)
	{
		return static_cast<const MarkerEntry*>(token)->Text.data();
	}
	return m_unknown.Text.data();
}

const void* GpuCrashScopes::Record(std::string_view label, char operation) noexcept
{
	auto* session = GpuCrashSession::Current();
	if (session == nullptr)
	{
		return nullptr;
	}
	std::uint32_t id;
	const auto* token = session->Marker(label, m_depth == 0 ? 0 : m_parents[m_depth - 1], operation, id);
	if (operation == 'B')
	{
		m_parents[m_depth++] = id;
	}
	return token;
}

const void* GpuCrashScopes::Begin(std::string_view label) noexcept
{
	if (m_overflowDepth != 0 || m_depth == m_parents.size())
	{
		++m_overflowDepth;
		if (auto* session = GpuCrashSession::Current())
		{
			session->Incomplete();
			std::uint32_t id;
			return session->Marker({}, 0, 'B', id);
		}
		return nullptr;
	}
	return Record(label, 'B');
}

const void* GpuCrashScopes::End() noexcept
{
	if (m_overflowDepth != 0)
	{
		--m_overflowDepth;
		return Record("unknown nesting", 'E');
	}
	if (m_depth == 0)
	{
		return Record("unmatched end", 'E');
	}
	// End refers to its Begin entry; never to a later recording or frame.
	const void* token = Record("scope end", 'E');
	--m_depth;
	return token;
}

const void* GpuCrashScopes::Insert(std::string_view label) noexcept
{
	return Record(label, 'M');
}

void GpuCrashScopes::Reset() noexcept
{
	m_depth = 0;
	m_overflowDepth = 0;
}
