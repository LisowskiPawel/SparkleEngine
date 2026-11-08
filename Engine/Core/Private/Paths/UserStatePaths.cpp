#include "PCH.h"

#include "Core/Public/Strings/StringUtils.h"
#include "Core/Public/Hash/HashUtils.h"
#include "Core/Public/Paths/ProductUserStatePaths.h"
#include "Core/Public/Paths/WorkspaceUserStatePaths.h"

#include "Core/Public/Paths/FileSystemPaths.h"
#include "Core/Public/Paths/PathUtils.h"
#include "FileSystemDiscovery.h"
#include "UserStatePaths.h"

#include <cctype>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace Filesystem
{
	static std::string MakeWorkspaceStateKey(const std::filesystem::path& workspaceRoot)
	{
		const std::filesystem::path normalizedRoot = Paths::Normalize(workspaceRoot);
		std::string pathKey = Strings::ToNarrow(normalizedRoot.generic_wstring());
#if defined(_WIN32)
		for (char& character : pathKey)
		{
			character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
		}
#endif
		const std::uint64_t hash = Hash::Fnv1a64(pathKey);

		std::string workspaceName = Paths::MakeSafePathComponent(Strings::ToNarrow(normalizedRoot.filename().wstring()));
		if (workspaceName.empty())
		{
			workspaceName = "Workspace";
		}

		std::ostringstream result;
		result << workspaceName << '-' << std::hex << std::setfill('0') << std::setw(16) << hash;
		return result.str();
	}

	static std::string ResolveProductName(const std::filesystem::path& projectRoot)
	{
		if (!projectRoot.empty())
		{
			return projectRoot.filename().string();
		}
		const std::string inferredName = InferProjectNameFromExecutableStem(GetExecutableStem());
		return inferredName.empty() ? "Sparkle" : inferredName;
	}

	static ProductUserStatePaths BuildProductUserStatePaths(const std::filesystem::path& root)
	{
		const std::filesystem::path normalizedRoot = Paths::Normalize(root);

		return ProductUserStatePaths{
		    .Root = normalizedRoot,
		    .SettingsRoot = normalizedRoot / "Settings",
		    .LogsRoot = normalizedRoot / "Logs",
		    .CapturesRoot = normalizedRoot / "Captures",
		    .CrashesRoot = normalizedRoot / "Crashes",
		    .CacheRoot = normalizedRoot / "Cache"};
	}

	WorkspaceUserStatePaths ResolveWorkspaceUserStatePaths(const std::filesystem::path& workspaceRoot)
	{
		const std::filesystem::path root = ResolveSparkleUserStateRoot();
		const std::string workspaceKey = MakeWorkspaceStateKey(workspaceRoot);
		return WorkspaceUserStatePaths{.DevelopmentProductsRoot = root / "Development" / workspaceKey, .LauncherRoot = root / "LauncherState" / workspaceKey};
	}

	ProductUserStatePaths ResolveDevelopmentProductUserStatePaths(const std::filesystem::path& workspaceRoot, std::string_view productName)
	{
		std::string safeProductName = Paths::MakeSafePathComponent(productName);
		if (safeProductName.empty())
		{
			safeProductName = "Sparkle";
		}
		const WorkspaceUserStatePaths workspaceState = ResolveWorkspaceUserStatePaths(workspaceRoot);
		return BuildProductUserStatePaths(workspaceState.DevelopmentProductsRoot / safeProductName);
	}

	ProductUserStatePaths ResolveCurrentProductUserStatePaths()
	{
		const std::filesystem::path workspaceRoot = ResolveWorkspaceRootPath();
		const auto projectRoot = DiscoverProjectRoot();
		return ResolveDevelopmentProductUserStatePaths(workspaceRoot, ResolveProductName(projectRoot.value_or(std::filesystem::path{})));
	}
}
