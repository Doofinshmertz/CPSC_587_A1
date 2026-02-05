/**
 * CPSC 587 W26 Assignment 1
 * @name Holden Holzer
 * @email holden.holzer@ucalgary.ca
 *
 * Modified from provided Assignment 1 - Boilerplate
 * @authors Copyright 2019 Lakin Wecker, Jeremy Hart, Andrew Owens and Others (see AUTHORS)
 */

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "givr.h"

namespace modelling {
	class HermiteCurve {
	public:
		struct ControlPoint {
			glm::vec3 position;
			glm::vec3 tangent;
		};
		using ControlPoints = std::vector<ControlPoint>;
		static HermiteCurve::ControlPoints
			buildControlPoints(std::vector<glm::vec3> posistions);
		static void calculateCatmullRomTangents(HermiteCurve::ControlPoints& cps);
		static void calculateCanonicalTangents(HermiteCurve::ControlPoints& cps, float c);

		HermiteCurve() = default;
		explicit HermiteCurve(ControlPoints controlPoints);

		glm::vec3 operator()(float U) const;
		glm::vec3 position(float U) const;

		ControlPoints const& controlPoints() const;
		ControlPoints& controlPoints();

		float arcLength(float dU) const;
		std::vector<glm::vec3> sample(size_t number_of_samples) const;

		//Render tracks
		givr::geometry::MultiLine controlPointGeometry() const;
		givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP>
			controlPointFrameGeometry() const;
		givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP>
			sampledGeometry(size_t number_of_samples) const;

		size_t size() const;

		// the smallest distance between any two consecutive points in the curve
		float minSeperation();
		// the largest distance between any two consecutive points in the curve
		float maxSeperation();

	private:
		ControlPoints m_cps;

		std::pair<float, size_t> localize(float U) const;

		//***** STUDENTS TO-DO *****//
		// Gets the position at U (global parameter)
		glm::vec3 positionInSegment(std::pair<float, size_t> u_seg) const;
		//***** ******** ***** *****//
	};

} // namespace modelling