#include "PCH.h"

#include "Cooking/Compilation/ShaderCompileBatch.h"

#include "Cooking/Compilation/ShaderCompileJob.h"
#include "Cooking/Compilation/ShaderMake/ShaderMakeBatch.h"
#include "Cooking/ShaderCookSettings.h"
#include "Verification/ShaderParameterStructVerifier.h"

#include <unordered_map>
#include <utility>

struct ProducerMap final
{
	std::vector<std::size_t> ProducerJobIndices;
	std::vector<std::size_t> ProducerForJob;
};

static ProducerMap SelectProducers(std::span<const ShaderCompileJob> jobs)
{
	ProducerMap result;
	result.ProducerForJob.resize(jobs.size());
	std::unordered_map<ShaderCompileInputHash, std::vector<std::size_t>> producersByHash;

	for (std::size_t jobIndex = 0; jobIndex < jobs.size(); ++jobIndex)
	{
		const ShaderCompileJob& job = jobs[jobIndex];
		std::size_t producerIndex = result.ProducerJobIndices.size();
		if (const auto matchingHash = producersByHash.find(job.InputHash); matchingHash != producersByHash.end())
		{
			for (const std::size_t candidateProducer : matchingHash->second)
			{
				if (jobs[result.ProducerJobIndices[candidateProducer]].HasSameCompilerInput(job))
				{
					producerIndex = candidateProducer;
					break;
				}
			}
		}
		if (producerIndex == result.ProducerJobIndices.size())
		{
			result.ProducerJobIndices.push_back(jobIndex);
			producersByHash[job.InputHash].push_back(producerIndex);
		}
		result.ProducerForJob[jobIndex] = producerIndex;
	}
	return result;
}

static std::vector<ShaderCompileResult> FanOutResults(std::span<const ShaderCompileJob> jobs, std::span<ShaderCompileResult> producerResults, std::span<const std::size_t> producerForJob)
{
	std::vector<ShaderCompileResult> results;
	results.reserve(jobs.size());
	std::vector<std::size_t> remainingConsumers(producerResults.size());
	for (const std::size_t producer : producerForJob)
	{
		++remainingConsumers[producer];
	}

	for (std::size_t jobIndex = 0; jobIndex < jobs.size(); ++jobIndex)
	{
		const std::size_t producer = producerForJob[jobIndex];
		if (--remainingConsumers[producer] == 0)
		{
			results.push_back(std::move(producerResults[producer]));
		}
		else
		{
			results.push_back(producerResults[producer]);
		}
		results.back().ShaderType = jobs[jobIndex].Request.ShaderType;
		results.back().Target = jobs[jobIndex].Request.Target;
	}
	return results;
}

static void FinalizeResults(std::span<const ShaderCompileJob> jobs, std::span<ShaderCompileResult> results, std::size_t totalWork, const ToolWorkProgressCallback& progress)
{
	for (std::size_t jobIndex = 0; jobIndex < jobs.size(); ++jobIndex)
	{
		const ShaderCompileJob& job = jobs[jobIndex];
		ShaderCompileResult& result = results[jobIndex];
		VerifyCookedShaderParameters(job, result.Output);

		if (progress)
		{
			progress({.Action = "Verified shader", .Item = job.Request.ShaderTypeName, .Completed = jobs.size() + jobIndex + 1, .Total = totalWork});
		}
	}
}

std::vector<ShaderCompileResult> CompileShaderBatch(const ShaderCookSettings& settings, std::span<const ShaderCompileJob> jobs, std::size_t totalWork, const ToolWorkProgressCallback& progress)
{
	if (jobs.empty())
	{
		return {};
	}

	const ProducerMap producerMap = SelectProducers(jobs);
	std::vector<ShaderCompileResult> producerResults = CompileShaderMakeProducers(settings, jobs, producerMap.ProducerJobIndices, totalWork, progress);
	std::vector<ShaderCompileResult> results = FanOutResults(jobs, producerResults, producerMap.ProducerForJob);
	FinalizeResults(jobs, results, totalWork, progress);
	return results;
}
