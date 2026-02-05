/**
 * CPSC 587 W26 Assignment 1
 * @name Holden Holzer
 * @email holden.holzer@ucalgary.ca
 *
 * Modified from provided Assignment 1 - Boilerplate
 * @authors Copyright 2019 Lakin Wecker, Jeremy Hart, Andrew Owens and Others (see AUTHORS)
 */

#pragma once

#include "hermite_curve.hpp"
#include <glm/glm.hpp>
#include <vector>
namespace modelling {
	class ArcLengthTable {
	public:
		using table_t = std::vector<float>;
		using iterator = table_t::iterator;
		using const_iterator = table_t::const_iterator;

		ArcLengthTable() = default;
		explicit ArcLengthTable(float deltaS);

		size_t size() const;
		float deltaS() const;
		float length() const;

		void addNext(float t);
		void reserve_memory(size_t n);

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		//***** STUDENTS TO-DO *****//
		// Gets the nearest sample U at s
		float nearestValueTo(float s) const;
		//***** ******** ***** *****//

		//***** STUDENTS TO-DO *****//
		// Gets the U value (linearly calculated) at s
		float operator()(float s) const;
		//***** ******** ***** *****//

		float arc_length = 0.0; // this is the arc length that was used in the calculation of this table
		float max_height = 0; // the maximum height encountered along the curve
		float s_max_height = 0; // the s coordinate of the maximum height

		// prints the values in the table
		void TestTable(float jump);

	private:
		table_t m_values;
		float m_delta_s = 1.f;

		size_t indexAt(float s) const;

		/**
		 * wrap s around the length of the track
		 */
		float WrapS(float s) const;
	};

	//***** STUDENTS TO-DO *****//
	// Generates the ALP
	ArcLengthTable calculateArcLengthTable(
		HermiteCurve const& curve, float delta_s, float delta_u
	);
	//***** ******** ***** *****//

} // namespace modelling