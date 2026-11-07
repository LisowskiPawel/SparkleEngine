#pragma once

class QWidget;

namespace SparkleLauncher
{
	struct LauncherOperationRequest;
	bool ConfirmLauncherRunRequest(QWidget& parent, LauncherOperationRequest& request);
}
