#pragma once

#include "Gui/Models/LauncherContextUiModel.h"
#include <string_view>

namespace SparkleLauncher
{
	QVector<LauncherSelectionOption> BuildExternalCaptureOptions(std::string_view api, std::string_view profile);
}
