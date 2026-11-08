#pragma once

namespace SparkleLauncher
{
	struct MaintenanceOperationPlan;
	void InspectMaintenanceCleanTargets(MaintenanceOperationPlan& plan);
	void DescribeMaintenancePlan(MaintenanceOperationPlan& plan);
}
