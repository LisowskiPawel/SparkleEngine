#pragma once

#include "ShaderParameterStructBuilder.h"

#include <vector>

template <typename TParameters> class TypedPassParameterInstance final
{
public:
	using Parameters = TParameters;
	using Metadata = ShaderParameterStructMetadata<TParameters>;

	explicit TypedPassParameterInstance(const Metadata& metadata) :
	    m_metadata(metadata),
	    m_parameterSet(metadata.GetLayout(), metadata.GetGraphResourceParameters())
	{
	}

	TypedPassParameterInstance(const Metadata&& metadata) = delete;

	TypedPassParameterInstance(const TypedPassParameterInstance&) = delete;
	TypedPassParameterInstance& operator=(const TypedPassParameterInstance&) = delete;
	TypedPassParameterInstance(TypedPassParameterInstance&&) = delete;
	TypedPassParameterInstance& operator=(TypedPassParameterInstance&&) = delete;

	TParameters& GetFields() noexcept
	{
		m_bIsDirty = true;
		return m_fields;
	}

	const TParameters& GetFields() const noexcept { return m_fields; }

	TParameters* operator->() noexcept
	{
		m_bIsDirty = true;
		return &m_fields;
	}

	const TParameters* operator->() const noexcept { return &m_fields; }

	TParameters& operator*() noexcept
	{
		m_bIsDirty = true;
		return m_fields;
	}

	const TParameters& operator*() const noexcept { return m_fields; }

	bool Sync() const
	{
		if (!m_bIsDirty)
		{
			return m_bBindingsValid;
		}

		m_bBindingsValid = m_metadata.Commit(m_fields, m_parameterSet, m_bindingStorage);
		m_bIsDirty = false;
		return m_bBindingsValid;
	}

	const PassParameterSet& GetPassParameterSet() const
	{
		Sync();
		return m_parameterSet;
	}

private:
	const Metadata& m_metadata;
	TParameters m_fields = {};
	mutable bool m_bIsDirty = true;
	mutable bool m_bBindingsValid = false;
	mutable PassParameterSet m_parameterSet;
	mutable std::vector<std::vector<std::byte>> m_bindingStorage;
};
