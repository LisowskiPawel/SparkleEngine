#include "PCH.h"
#include "Textures/CookedTextureLoad.h"

#include "Core/Public/Diagnostics/Error.h"
#include "Core/Public/Diagnostics/Verify.h"
#include <TaskScheduler_c.h>
#include <new>
#include <utility>

SPARKLE_DEFINE_LOG_CATEGORY_STATIC(g_cookedTextureLoadLogger, "Renderer.CookedTextureLoad");

CookedTextureLoad::CookedTextureLoad(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::filesystem::path path, std::uint64_t maximumDecodedBytes) :
    m_io(io),
    m_cpu(cpu),
    m_path(std::move(path)),
    m_maximumDecodedBytes(maximumDecodedBytes)
{
}

CookedTextureLoad::~CookedTextureLoad() noexcept
{
	if (m_bSubmitted && !IsSettled())
	{
		Diagnostics::Fatal(g_cookedTextureLoadLogger, __FILE__, __LINE__, "Accepted texture work must be joined before destroying its native argument storage.");
	}
	if (m_readComplete)
	{
		enkiDeleteCompletionAction(m_io, m_readComplete);
	}
	if (m_decode)
	{
		enkiDeletePinnedTask(m_cpu, m_decode);
	}
	if (m_read)
	{
		enkiDeletePinnedTask(m_io, m_read);
	}
}

void CookedTextureLoad::Start(std::uint32_t admissionIndex)
{
	if (m_bSubmitted)
	{
		throw Diagnostics::Error("Cooked texture load was submitted twice.");
	}
	if (!m_io && !m_cpu)
	{
		m_bSubmitted = true;
		Read(this);
		Decode(this);
		return;
	}
	PrepareNativeWork(admissionIndex);
	m_bSubmitted = true;
	enkiAddPinnedTask(m_io, m_read);
}

void CookedTextureLoad::PrepareNativeWork(std::uint32_t admissionIndex)
{
	if (!m_io || !m_cpu)
	{
		throw Diagnostics::Error("Texture loading requires both native resources, or neither.");
	}
	const auto ioConfig = enkiGetTaskSchedulerConfig(m_io);
	const auto cpuConfig = enkiGetTaskSchedulerConfig(m_cpu);
	if (!ioConfig.numTaskThreadsToCreate || !cpuConfig.numTaskThreadsToCreate)
	{
		throw Diagnostics::Error("Texture loading requires physical native I/O and CPU workers.");
	}
	m_read = enkiCreatePinnedTask(m_io, Read, ioConfig.numExternalTaskThreads + 1 + admissionIndex % ioConfig.numTaskThreadsToCreate);
	m_decode = enkiCreatePinnedTask(m_cpu, Decode, cpuConfig.numExternalTaskThreads + 1 + admissionIndex % cpuConfig.numTaskThreadsToCreate);
	m_readComplete = enkiCreateCompletionAction(m_io, AdmitDecode, nullptr);
	if (!m_read || !m_decode || !m_readComplete)
	{
		throw std::bad_alloc();
	}
	enkiSetArgsPinnedTask(m_read, this);
	enkiSetArgsPinnedTask(m_decode, this);
	enkiSetParamsCompletionAction(m_readComplete, {this, nullptr, enkiGetCompletableFromPinnedTask(m_read)});
}

void CookedTextureLoad::Cancel() noexcept
{
	m_stop.request_stop();
}

void CookedTextureLoad::Read(void* argument) noexcept
{
	auto& load = *static_cast<CookedTextureLoad*>(argument);
	if (load.m_stop.stop_requested())
	{
		return;
	}
	try
	{
		load.m_file = ReadCookedTexturePayload(load.m_path, load.m_maximumDecodedBytes);
	}
	catch (...)
	{
		load.m_failure = std::current_exception();
	}
}

void CookedTextureLoad::AdmitDecode(void* argument, std::uint32_t) noexcept
{
	auto& load = *static_cast<CookedTextureLoad*>(argument);
	enkiAddPinnedTask(load.m_cpu, load.m_decode);
}

void CookedTextureLoad::Decode(void* argument) noexcept
{
	auto& load = *static_cast<CookedTextureLoad*>(argument);
	if (!load.m_stop.stop_requested() && !load.m_failure)
	{
		try
		{
			load.m_texture = DecodeCookedTexture(load.m_file, load.m_maximumDecodedBytes);
		}
		catch (...)
		{
			load.m_failure = std::current_exception();
		}
	}
	// Cleanup also runs after a failed read or cancelled/skipped decode.
	load.m_file = {};
}

bool CookedTextureLoad::IsSettled() const noexcept
{
	if (!m_bSubmitted)
	{
		return false;
	}
	if (!m_io)
	{
		return true;
	}
	if (!enkiIsCompletableComplete(m_io, enkiGetCompletableFromCompletionAction(m_readComplete)))
	{
		return false;
	}
	return enkiIsPinnedTaskComplete(m_cpu, m_decode) != 0;
}

void CookedTextureLoad::Join() noexcept
{
	if (!m_bSubmitted || !m_io || IsSettled())
	{
		return;
	}
	// Renderer releases its frame registration before asset-owner destruction.
	// Register one resource at a time: enkiTS uses one thread-local native index.
	const bool bRegisterIo = enkiGetThreadNum(m_io) == ENKI_NO_THREAD_NUM;
	if (bRegisterIo && !enkiRegisterExternalTaskThread(m_io))
	{
		Diagnostics::Fatal(g_cookedTextureLoadLogger, __FILE__, __LINE__, "Texture teardown could not acquire its reserved native I/O registration.");
	}
	if (!bRegisterIo && enkiGetThreadNum(m_io) != 0)
	{
		Diagnostics::Fatal(g_cookedTextureLoadLogger, __FILE__, __LINE__, "Texture I/O teardown requires its creator or the detached render owner.");
	}
	enkiWaitForPinnedTask(m_io, m_read);
	enkiWaitForCompletable(m_io, enkiGetCompletableFromCompletionAction(m_readComplete));
	if (bRegisterIo)
	{
		enkiDeRegisterExternalTaskThread(m_io);
	}

	const bool bRegisterCpu = enkiGetThreadNum(m_cpu) == ENKI_NO_THREAD_NUM;
	if (bRegisterCpu && !enkiRegisterExternalTaskThread(m_cpu))
	{
		Diagnostics::Fatal(g_cookedTextureLoadLogger, __FILE__, __LINE__, "Texture teardown could not acquire its reserved native CPU registration.");
	}
	if (!bRegisterCpu && enkiGetThreadNum(m_cpu) != 0)
	{
		Diagnostics::Fatal(g_cookedTextureLoadLogger, __FILE__, __LINE__, "Texture CPU teardown requires its creator or the detached render owner.");
	}
	enkiWaitForPinnedTask(m_cpu, m_decode);
	if (bRegisterCpu)
	{
		enkiDeRegisterExternalTaskThread(m_cpu);
	}
}

LoadedTextureData CookedTextureLoad::TakeTexture()
{
	if (!IsSettled())
	{
		throw Diagnostics::Error("Texture result consumed before physical native settlement.");
	}
	if (m_failure)
	{
		std::rethrow_exception(m_failure);
	}
	if (m_stop.stop_requested())
	{
		throw Diagnostics::Error("Texture load cancelled.");
	}
	return std::move(m_texture);
}
