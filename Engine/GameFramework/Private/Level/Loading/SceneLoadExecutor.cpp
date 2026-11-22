#include "PCH.h"
#include "Level/Loading/SceneLoadExecutor.h"

#include "Assets/SceneAssetCatalog.h"
#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Paths/CookedAssetPaths.h"
#include "Level/Loading/SceneLoadRun.h"
#include "Level/Loading/SceneLoadWorkState.h"
#include <utility>

namespace Assets
{
	struct SceneLoadExecutor::ControlState final
	{
		enkiTaskScheduler* Io;
		enkiTaskScheduler* Cpu;
		std::stop_token ParentStop;
		std::unique_ptr<SceneLoadRun> Current;
		std::shared_ptr<const SceneAssetCatalog> Catalog;
		std::uint64_t NextCatalogGeneration = 1;
		std::uint64_t RequestId = 0;
	};

	SceneLoadExecutor::SceneLoadExecutor(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::stop_token parentStop) :
	    m_control(std::make_unique<ControlState>(io, cpu, parentStop))
	{
	}

	SceneLoadExecutor::~SceneLoadExecutor()
	{
		Cancel();
		if (m_control->Current)
		{
			m_control->Current->Join();
		}
	}

	void SceneLoadExecutor::Start(std::uint64_t requestId, std::uint64_t worldGeneration, std::uint64_t documentGeneration, LevelDesc level)
	{
		if (m_control->Current)
		{
			throw Diagnostics::Error("A scene load was started before the previous request settled and was consumed.");
		}
		auto work = std::make_unique<SceneLoadWorkState>(Filesystem::GetCookedAssetRootPath());
		m_control->Catalog = LoadSceneAssetCatalog(m_control->NextCatalogGeneration++);
		work->PreparePackage(requestId, worldGeneration, documentGeneration, std::move(level), *m_control->Catalog);
		auto run = std::make_unique<SceneLoadRun>(m_control->Io, m_control->Cpu, m_control->ParentStop, std::move(work));
		run->Prepare();
		m_control->RequestId = requestId;
		m_control->Current = std::move(run);
		m_control->Current->Submit();
	}

	void SceneLoadExecutor::Cancel() noexcept
	{
		if (m_control->Current)
		{
			m_control->Current->Cancel();
		}
	}

	std::optional<SceneLoadCompletion> SceneLoadExecutor::ConsumeSettled()
	{
		auto& run = m_control->Current;
		if (!run || !run->IsSettled())
		{
			return std::nullopt;
		}
		auto completion = run->TakeCompletion(m_control->RequestId);
		run.reset();
		return completion;
	}

	LevelLoadOperationProgress SceneLoadExecutor::GetProgress() const noexcept
	{
		if (m_control->Current)
		{
			return m_control->Current->GetProgress(m_control->RequestId);
		}
		return {.RequestId = m_control->RequestId};
	}

	std::uint64_t SceneLoadExecutor::GetCatalogGeneration() const noexcept
	{
		return m_control->Catalog ? m_control->Catalog->GetGeneration() : 0;
	}
}
