/**
	Object to Arc-length Parameterize the given curve
		f : s -> U (arc length s to global U)
	This object is designed to be closed (the beginning is the end), though a slight change to the functions can make it open curves
	Given dS value, each U value is saved at each dS step

		Table[0] (0 * dS) = u_0 (Usually 0)
		Table[1] (1 * dS) = u_1 
		Table[2] (2 * dS) = u_2 
				....................	
		Table[i] (i * dS) = u_i
				....................
		Table[N-1] ((N-1) * dS) = u_(N-1)
		//END (next is Table[0], this segment ds_end <= dS
  **/

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

		/**
		 * find the maximum height along the curve and the point at which it occurs
		 */
		

		float getMaxHeight() const;
		float posMaxHeight() const;

		float arc_length = 0.0; // this is the arc length that was used in the calculation of this table
		float max_height = 0;
		float s_max_height = 0;

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