#pragma once

#include "Level/LevelDesc.h"

class WorldReadView;

LevelDesc CaptureLevelWorldState(LevelDesc description, const WorldReadView& view);
