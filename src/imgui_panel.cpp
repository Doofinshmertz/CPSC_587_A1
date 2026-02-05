/**
 * CPSC 587 W26 Assignment 1
 * @name Holden Holzer
 * @email holden.holzer@ucalgary.ca
 *
 * Modified from provided Assignment 1 - Boilerplate
 * @authors Copyright 2019 Lakin Wecker, Jeremy Hart, Andrew Owens and Others (see AUTHORS)
 */

#include "imgui_panel.hpp"

#include <array>

namespace imgui_panel {
	// default values
	bool showPanel = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// simulation controlls
	float look_ahead = 5.00f; 
	bool update_lookahead = false;
	float playback_speed = 1.0f;
	bool reset_simulation = false;

	// bonus stuff
	int num_carts = 4;
	bool use_moving_camera = false;

	// visualization
	bool resample = true;
	int curveSamples = 300;
	bool show_curve = false;

	// loading
	bool rereadControlPoints = false;
	bool clearControlPointsFilePath = false;
	std::string controlPointsFilePath = "./roller_coaster.obj";

	// animation
	bool play = false;
	bool resetView = false;

	std::function<void(void)> draw = [](void) {
		if (!showPanel) return;

		if (showPanel && ImGui::Begin("ImGui Panel", &showPanel, ImGuiWindowFlags_MenuBar)) {
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Close", "(P)"))
						showPanel = false;
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Background Color"))
				ImGui::ColorEdit3("Clear color", (float*)&clear_color);

			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Visualization")) {
				ImGui::SliderInt("Curve Samples", &curveSamples, 5, 1000);
				resample = ImGui::Button("Resample Curve");
			}

			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Loading control points")) {
				static std::array<char, 64> buffer;
				ImGui::InputText("(OBJ) file", buffer.data(), buffer.size());

				rereadControlPoints = ImGui::Button("Load");
				ImGui::SameLine();
				clearControlPointsFilePath = ImGui::Button("Clear");

				if (rereadControlPoints) controlPointsFilePath = buffer.data();
				if (clearControlPointsFilePath) {
					buffer = std::array<char, 64>();
					clearControlPointsFilePath = false;
				}
			}

			ImGui::Spacing();
			ImGui::Separator();
			if (ImGui::Button(play ? "Pause" : "Play"))
				play = !play;
			
			// allow hiding the curve/control point/ tangent render
			if(ImGui::Button(show_curve ? "Hide Curve" : "Show Curve"))
			{
				show_curve = !show_curve;
			}

			ImGui::Spacing();
			ImGui::Separator();
			resetView = ImGui::Button("Reset view");

			// allow the camera to follow the carts
			if (ImGui::Button(use_moving_camera ? "Use Stationary Camera" : "Use Moving Camera"))
			{
				use_moving_camera = !use_moving_camera;
				if (!use_moving_camera)
				{
					resetView = true;
				}
			}

			// for reseting the simulation
			if(ImGui::Button("Reset Simulation"))
			{
				reset_simulation = true;
			}

			// set the number of carts
			ImGui::SliderInt("Number of Carts", &num_carts, 1,10);

			// allow user to change the simulation speed
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::SliderFloat("Playback Speed", &playback_speed, 0.1f, 10.0f);
			// allow user to set the look ahead
			if(ImGui::SliderFloat("Look Ahead", &look_ahead, 0.1f, 20.0f))
			{
				update_lookahead = true;
			}

			ImGui::Spacing();
			ImGui::Separator();
			float frame_rate = ImGui::GetIO().Framerate;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / frame_rate, frame_rate);
		}
		ImGui::End();
	};
} // namespace panel