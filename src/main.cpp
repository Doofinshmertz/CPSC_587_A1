#include "givio.h"
#include "givr.h"
#include "picking_controls.h"
#include "turntable_controls.h"

#include "imgui_panel.hpp"
#include "arc_length_parameterize.hpp"
#include "curve_file_io.hpp"
#include "hermite_curve.hpp"
#include "RollerCoaster.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace glm;
using namespace giv;
using namespace giv::io;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;

#define SEP_DIST 0.5f
#define MIN_V 5.0f
#define DEC_FRAC 0.9f
#define DELTA_S 0.34f
#define CART_LENGTH 1.6f
#define SUPPORT_SPACING 20.0f
#define NUM_TREES 30


// This boilerplate sets up work for the Hermite Curve and Arc Length Table. Each will indicate which functions to complete
// You are also expected to implement the Single Particle kinamatics and Framing of the curve (and bonuses) from scratch
// THis main file contains the program loop of the simulation with controls define in the key bindings here and the imgui_panel files
// Once on the single particle kinamatics section, you may hard code values (chain speed and segment starts) and assign the desired
// instead of the default. Lastly, there are 3 coasters in the obj files. We also gave 3 Arc Length parameterized versions for students who
// need to debug or cannot finish the ALP tables (you will not get full marks). Coaster 2 is small compared to the model of the cart so apply 
// scaling to the models when needed. You may also use your own coaster


const std::vector<glm::vec3> default_cps = {
	{0.f, 0.f, 10.f},
	{10.f, 0.f, 0.f},
	{0.f, 0.f, -10.f},
	{-10.f, 0.f, 0.f}
};

// program entry point
int main(void) {
	// initialize OpenGL and window
	GLFWContext glContext;
	glContext.glMajorVesion(3)
		.glMinorVesion(3)
		.glForwardComaptability(true)
		.glCoreProfile()
		.glAntiAliasingSamples(4)
		.matchPrimaryMonitorVideoMode();
	std::cout << glfwVersionString() << '\n';

	// setup window (OpenGL context)
	ImGuiWindow window = glContext.makeImGuiWindow(Properties()
		.size(dimensions{ 800, 600 })
		.title("Assignment 1: Roller Coaster")
		.glslVersionString("#version 330 core"));
	// set our imgui update function
	panel::update_lambda_function = imgui_panel::draw;

	// create viewing camera and controls
	ViewContext view = View(TurnTable(), Perspective());
	TurnTableControls controls(window, view.camera);

	// create key bingdings
	auto reset_camera = [&](auto event) { view.camera.reset(); };
	auto toggle_panel = [&](auto event) {
		if (event.action == GLFW_PRESS)
			imgui_panel::showPanel = !imgui_panel::showPanel;
	};
	window.keyboardCommands() 
		| Key(GLFW_KEY_V, reset_camera) 
		| Key(GLFW_KEY_P, toggle_panel);

	// initial curve --
	modelling::HermiteCurve curve(
		modelling::HermiteCurve::buildControlPoints(default_cps)
	);

	//To load the arc length parameterized curve (only worth part marks so only do this if you need to):
	//curve = modelling::readHermiteCurveFromFile("./models/roller_coaster_1_ALP.txt").value();

	// Control points frame Geometry
	PolyLine cp_geometry = curve.controlPointFrameGeometry();
	GL_Line cp_style = GL_Line(Width(15.), Colour(0.5, 1.0, 0.0));
	RenderContext cp_render = createRenderable(cp_geometry, cp_style);

	// Control points Geometry
	MultiLine cp_t_geometry = curve.controlPointGeometry();
	GL_Line cp_t_style = GL_Line(Width(25.), Colour(1.0, 0.0, 0.3));
	RenderContext cp_t_render = createRenderable(cp_t_geometry, cp_t_style);

	// geometry for curve
	PolyLine track_geometry = curve.sampledGeometry(imgui_panel::curveSamples);
	GL_Line track_style = GL_Line(Width(15.), Colour(0.2, 0.7, 1.0));
	RenderContext track_render = createRenderable(track_geometry, track_style);

	// Cart
	Mesh cart_geometry = Mesh(Filename("./models/cart.obj"));
	PhongStyle cart_style = Phong(Colour(1.f, 1.f, 0.0f), LightPosition(100.f, 100.f, 100.f));
	InstancedRenderContext cart_renders = createInstancedRenderable(cart_geometry, cart_style);

	// Track peice
	Mesh track_piece = Mesh(Filename("./models/track_piece.obj"));
	PhongStyle track_piece_style = Phong(Colour(0.0f, 1.0f, 1.0f), LightPosition(100.0f, 100.0f, 100.0f));
	InstancedRenderContext track_piece_render = createInstancedRenderable(track_piece, track_piece_style);

	Mesh ground_geometry = Mesh(Filename("./models/Ground.obj"));
	PhongStyle ground_style = Phong(Colour(0.4f, 0.40f, 0.20f), LightPosition(100.0f, 100.0f, 100.0f));
	InstancedRenderContext ground_render = createInstancedRenderable(ground_geometry, ground_style);
	glm::mat4 ground_transform = glm::scale(glm::translate(mat4f{1.f}, glm::vec3(0.0f, -10.0f, 0.0f)), glm::vec3(10.0f, 10.0f, 10.0f));

	Mesh tree_geometry = Mesh(Filename("./models/Tree.obj"));
	PhongStyle tree_style = Phong(Colour(0.1f, 0.70f, 0.10f), LightPosition(100.0f, 100.0f, 100.0f));
	InstancedRenderContext tree_render = createInstancedRenderable(tree_geometry, tree_style);

	Mesh sup_geometry = Mesh(Filename("./models/Track_support.obj"));
	PhongStyle sup_style = Phong(Colour(0.0f, 1.0f, 1.0f), LightPosition(100.0f, 100.0f, 100.0f));
	InstancedRenderContext sup_render = createInstancedRenderable(sup_geometry, sup_style);

	size_t cp_index = 0;
	float U = 0.f;

	modelling::RollerCoaster roller_coaster(SEP_DIST, MIN_V, DEC_FRAC, DELTA_S, imgui_panel::look_ahead, SUPPORT_SPACING, NUM_TREES);
	roller_coaster.UpdateCurve(curve);

	// the s position of the roller coaster
	float s = 0;
	
	// main loop
	// To load a model place it in the "models" directory, build, then type "./models/[name].obj" and press load.
	// If the above does not work, manually copy the model into the models folder in the output directory, then try and reload the track
	// Backspace isnt enabled when the panel is over the window, please move the panel off the window to backspace.
	//
	mainloop(std::move(window), [&](float dt /**** Time since last frame ****/) {
		if (imgui_panel::resetView)
			view.camera.reset();

		// load new controll points
		if (imgui_panel::rereadControlPoints) {
			// Try to load new control points as a HermiteCurve if possible
			std::optional<modelling::HermiteCurve> optional_curve 
			= modelling::readHermiteCurveFrom_OBJ_File(
				imgui_panel::controlPointsFilePath
			);
			// if curve is valid then update geometry data
			if (optional_curve) {
				// set the existing curve to the new curve vales
				curve = optional_curve.value();

				// update the redered geometry to represent the new curve
				cp_geometry = curve.controlPointFrameGeometry();
				cp_t_geometry = curve.controlPointGeometry();
				track_geometry = curve.sampledGeometry(imgui_panel::curveSamples);

				updateRenderable(cp_geometry, cp_style, cp_render);
				updateRenderable(cp_t_geometry, cp_t_style, cp_t_render);
				updateRenderable(track_geometry, track_style, track_render);

				// TODO:
				// convert the curve to a smooth curve
				// generate the ArcLengthTable
				
				// we want the delta u value to result in smaller distance jumps than delta_s
				// first calculate the maximum change in distance vs change in u max(dS/du)
				// this is calculated as max_seperation * number_points
				// then we require that delta_u * dS/du < delta_s or delta_u < du/dS * delta_s

				roller_coaster.UpdateCurve(curve);

				cp_index = 0;
				U = 0.f;
			}
		}

		if (imgui_panel::resample) {
			track_geometry = curve.sampledGeometry(imgui_panel::curveSamples);
			updateRenderable(track_geometry, track_style, track_render);
		}

		// Origin Render
		auto p = vec3f{ 0.f, 0.0f, 0.0f};
		auto M = translate(mat4f{ 1.f }, p);
		addInstance(cart_renders, M);

		// Cart animation
		// train of carts
		float D = CART_LENGTH*0.5f*float(imgui_panel::num_carts - 1);
		for(int i = 0; i < imgui_panel::num_carts; i++)
		{
			addInstance(cart_renders, roller_coaster.GetTransformAtPosition((s+CART_LENGTH*float(i) - D)));
		}

		// if true then make the camera follow the cart
		if(imgui_panel::use_moving_camera)
		{
			view.camera.set(givr::camera::Translation(roller_coaster.GetPositionAtS(s)));
		}
		
		// Simulation
		if (imgui_panel::play)
		{
			// increment and wrap
			cp_index = (cp_index + 1) % curve.controlPoints().size();
			U = std::fmod(U + 0.001f, 1.f);
			// update s
			s = s + roller_coaster.GetSpeedAtPos(s) * dt * imgui_panel::playback_speed;
			//printf("speed: %7.3f\n", roller_coaster.GetSpeedAtPos(s));
		}

		if(imgui_panel::update_lookahead)
		{
			roller_coaster.UpdateTrack(SEP_DIST, MIN_V, DEC_FRAC, imgui_panel::look_ahead);
			imgui_panel::update_lookahead = false;
		}

		if(imgui_panel::reset_simulation)
		{
			s = 0.0f;
			imgui_panel::reset_simulation = false;
		}

		
		// TODO: use the trasform array with the positions and rotations to place the track pieces
		std::vector<glm::mat4>* piece_transforms = roller_coaster.pieceTransforms();
		for(size_t i = 0; i < piece_transforms->size(); i++)
		{
			addInstance(track_piece_render, (*piece_transforms)[i]);
		}

		std::vector<glm::mat4> *sup_transforms = roller_coaster.SupportTransforms();
		for (size_t i = 0; i < sup_transforms->size(); i++)
		{
			addInstance(sup_render, (*sup_transforms)[i]);
		}

		std::vector<glm::mat4> *tree_transforms = roller_coaster.TreeTransforms();
		for (size_t i = 0; i < tree_transforms->size(); i++)
		{
			addInstance(tree_render, (*tree_transforms)[i]);
		}

		// environment objects
		
		addInstance(ground_render, ground_transform);

		// render
		auto color = imgui_panel::clear_color;
		glClearColor(color.x, color.y, color.z, color.z);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view.projection.updateAspectRatio(window.width(), window.height());

		draw(cp_render, view);
		draw(cp_t_render, view);
		draw(track_render, view);
		draw(cart_renders, view);
		draw(track_piece_render, view);
		draw(ground_render, view);
		draw(tree_render, view);
		draw(sup_render, view);
	});
	return EXIT_SUCCESS;
}