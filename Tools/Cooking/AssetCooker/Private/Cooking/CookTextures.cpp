#include "Cooking/AssetCookStages.h"
#include "Core/Public/Strings/StringUtils.h"
#include "Dispatch/AssetCookerToolProcess.h"
#include "Cooking/TextureRequestPlanBuilder.h"

#include <chrono>
#include <system_error>
#include <utility>

class AssetCookerTemporaryFile final
{
public:
	explicit AssetCookerTemporaryFile(std::filesystem::path path);
	~AssetCookerTemporaryFile();

	const std::filesystem::path& GetPath() const noexcept;

	AssetCookerTemporaryFile(const AssetCookerTemporaryFile&) = delete;
	AssetCookerTemporaryFile& operator=(const AssetCookerTemporaryFile&) = delete;

private:
	std::filesystem::path m_path;
};

AssetCookerTemporaryFile::AssetCookerTemporaryFile(std::filesystem::path path) :
    m_path(std::move(path))
{
}

AssetCookerTemporaryFile::~AssetCookerTemporaryFile()
{
	std::error_code errorCode;
	std::filesystem::remove(m_path, errorCode);
}

const std::filesystem::path& AssetCookerTemporaryFile::GetPath() const noexcept
{
	return m_path;
}

static std::filesystem::path MakeTemporaryPath(const AssetCookerProjectCookPlan& plan, std::string_view stem, std::string_view extension)
{
	const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
	return plan.temporaryRoot / (std::string(stem) + "-" + plan.projectName + "-" + std::to_string(timestamp) + std::string(extension));
}

bool ValidateTextureCookStage(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics)
{
	std::error_code error;
	if (!std::filesystem::is_regular_file(plan.textureCookerPath, error) || error)
	{
		diagnostics.AddError(AssetCookerCategory::Textures, "TextureCooker executable was not found.", plan.textureCookerPath);
		return false;
	}
	return true;
}

bool CookTextures(const AssetCookerProjectCookPlan& plan, AssetCookerDiagnostics& diagnostics, std::vector<AssetCookerOutputRecord>& outputs)
{
	const AssetCookerTemporaryFile requestFile(MakeTemporaryPath(plan, "assetcooker-texture-requests", ".txt"));

	std::error_code errorCode;
	std::filesystem::create_directories(requestFile.GetPath().parent_path(), errorCode);
	if (errorCode)
	{
		diagnostics.AddError(AssetCookerCategory::Textures, "Failed to create texture-request temp directory.", requestFile.GetPath().parent_path());
		return false;
	}

	if (!TextureRequestPlanBuilder::Build(plan, diagnostics, requestFile.GetPath()))
	{
		return false;
	}

	const int exitCode = AssetCookerToolProcess::Run(plan.textureCookerPath, {"cook-request-file", Strings::ToNarrow(requestFile.GetPath().wstring())}, plan.projectRoot);
	if (exitCode != 0)
	{
		diagnostics.AddError(AssetCookerCategory::Textures, "Texture asset cooking failed.");
		return false;
	}

	outputs.push_back({AssetCookerCategory::Textures, "textures", Strings::ToNarrow((plan.cookedRoot / "Textures").wstring())});
	return true;
}
