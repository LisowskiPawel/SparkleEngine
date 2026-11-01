#pragma once

#include "Graph/TaskGraphStorage.h"

class TaskExecutionContext;

TaskResult InvokeTaskFunction(const TaskGraphNode& node, TaskExecutionContext& context);
