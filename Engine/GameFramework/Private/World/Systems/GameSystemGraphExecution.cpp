#include "PCH.h"
#include "World/Systems/GameSystemGraph.h"
#include "World/Systems/CompiledGameSystemGraphData.h"
#include "World/Systems/GameSystemWaveExecution.h"

namespace ECS
{
	bool CompiledGameSystemGraph::Execute(TaskExecutor& executor, GameWorldSystemExecution& systems, GameSystemGraphError& error)
	{
		if (!IsValid())
		{
			error = GetError();
			return false;
		}

		// Each wave resolves counts after prerequisite commits, including newly dirtied transforms.
		for (GameSystemWave& wave : m_data->Waves)
		{
			if (!ExecuteGameSystemWave(wave, m_data->Systems, executor, systems, error))
			{
				return false;
			}
		}
		error = {};
		return true;
	}
}
