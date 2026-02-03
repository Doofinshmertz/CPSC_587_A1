#pragma once

#include <iosfwd>
#include <string>

#include "givio.h"
#include "givr.h"
#include "imgui/imgui.h"

namespace imgui_panel {

extern bool showPanel;
extern ImVec4 clear_color;

// visualization
extern bool resample;
extern int curveSamples;

// loading
extern bool rereadControlPoints;
extern bool clearControlPointsFilePath;
extern std::string controlPointsFilePath;

// animation
extern bool play;
extern bool resetView;

// lambda function
extern std::function<void(void)> draw;

} // namespace panel
