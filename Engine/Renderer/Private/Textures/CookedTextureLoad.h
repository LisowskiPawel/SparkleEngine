#pragma once

#include "Textures/CookedTextureLoader.h"
#include <cstdint>
#include <exception>
#include <filesystem>
#include <stop_token>

struct enkiTaskScheduler;
struct enkiPinnedTask;
struct enkiCompletionAction;

class CookedTextureLoad final
{
public:
	CookedTextureLoad(enkiTaskScheduler* io, enkiTaskScheduler* cpu, std::filesystem::path path, std::uint64_t maximumDecodedBytes);
	~CookedTextureLoad() noexcept;
	CookedTextureLoad(const CookedTextureLoad&) = delete;
	CookedTextureLoad& operator=(const CookedTextureLoad&) = delete;

	void Start(std::uint32_t admissionIndex = 0);
	void Cancel() noexcept;
	void Join() noexcept;
	bool IsSettled() const noexcept;
	LoadedTextureData TakeTexture();

private:
	static void Read(void* argument) noexcept;
	static void AdmitDecode(void* argument, std::uint32_t) noexcept;
	static void Decode(void* argument) noexcept;

	enkiTaskScheduler* m_io;
	enkiTaskScheduler* m_cpu;
	std::filesystem::path m_path;
	std::uint64_t m_maximumDecodedBytes;
	std::stop_source m_stop;
	CookedTextureFilePayload m_file;
	LoadedTextureData m_texture;
	std::exception_ptr m_failure;
	enkiPinnedTask* m_read = nullptr;
	enkiPinnedTask* m_decode = nullptr;
	enkiCompletionAction* m_readComplete = nullptr;
	bool m_bSubmitted = false;
};
