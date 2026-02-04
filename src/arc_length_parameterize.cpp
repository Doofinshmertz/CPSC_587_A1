#include "arc_length_parameterize.hpp"

namespace modelling {

	ArcLengthTable::ArcLengthTable(float deltaS) : m_delta_s(deltaS) { assert(deltaS > 0); }

	void ArcLengthTable::addNext(float u) { m_values.push_back(u); }

	void ArcLengthTable::reserve_memory(size_t n) { m_values.reserve(n); }

	float ArcLengthTable::deltaS() const { return m_delta_s; }

	size_t ArcLengthTable::size() const { return m_values.size(); }

	float ArcLengthTable::length() const { return size() * deltaS(); }

	ArcLengthTable::iterator ArcLengthTable::begin() {
		return std::begin(m_values);
	}

	ArcLengthTable::iterator ArcLengthTable::end() { return std::end(m_values); }

	ArcLengthTable::const_iterator ArcLengthTable::begin() const {
		return std::begin(m_values);
	}

	ArcLengthTable::const_iterator ArcLengthTable::end() const {
		return std::end(m_values);
	}

	//***** STUDENTS TO-DO *****//
	// Gets the nearest sample U at s
	float ArcLengthTable::nearestValueTo(float s) const {
		return 0;
	}
	//***** ******** ***** *****//

	//***** STUDENTS TO-DO *****//
	// Gets the U value (linearly calculated) at s
	float ArcLengthTable::operator()(float s) const {

		s = WrapS(s);
		// ensure no negative values
		if(s < 0.0f) {return 0;} 
		// get the nearest index for this s value
		size_t index_a = size_t(std::floor(s / m_delta_s));
		// get the segment s value
		float s_seg = (s/m_delta_s) - float(index_a);
		// ensure that the indices wrap around
		index_a = index_a % m_values.size();

		// get the u values
		float u_a = m_values[index_a];
		float u_b;
		// special case for last index
		if(index_a == (m_values.size()-1))
		{
			u_b = 1.0f;
		}
		else
		{
			u_b = m_values[index_a+1];
		}
		
		// interpolate the values
		float u = (u_b - u_a) * s_seg + u_a;

		//printf("index_a: %d, index b: %d, s_seg: %10.3f, u_a: %10.7f, u_b: %10.7f, u: %10.7f\n", index_a, index_b, s_seg, u_a, u_b, u);
		return u;
	}
	//***** ******** ***** *****//

	size_t ArcLengthTable::indexAt(float s) const {
		// Assuming s in [0, length) since private function
		return size_t(s / m_delta_s);
	}

	//***** STUDENTS TO-DO *****//
	// Generates the ALP
	// maybe set delta_u based on the number of controll points (we want their to be more u steps than controll points)
	// delta_u should correspond to a smaller jump in position than delta_s
	// 1/num_control_points = 
	ArcLengthTable calculateArcLengthTable(HermiteCurve const& curve, float delta_s, float delta_u) 
	{
		assert(curve.controlPoints().size() > 0);
		assert(delta_s > 0.f);
		assert(delta_u > 0.f);

		ArcLengthTable table(delta_s); // this creates a table and sets the delta_s value
		//Loop here to fill table

		// get the arc-length
		float arc_length = curve.arcLength(delta_u);
		table.arc_length = arc_length;
		
		float u = 0; // the current u position
		float s = 0; // the current s position
		size_t num_points = size_t(std::ceil(arc_length / delta_s)); // the number of points in the table is the arclength divided by the s step
		table.reserve_memory(num_points);

		//printf("num_controll points: %d, arc length: %7.2f, delta s: %8.6f, delta_u: %8.6f, num_s_points: %d\n",curve.size(), arc_length, delta_s, delta_u, num_points);

		size_t num_u = 1.0 / delta_u;
		size_t s_index = 0;

		// the maximum height and position at which it occurs in the curve
		float H = curve(0.0f).y;
		float s_H = 0.0f;

		// create the table
		for(float u = 0; u < 1.0f; u += delta_u)
		{
			// try to find the maximum height 
			float temp = curve(u).y;
			if(temp > H)
			{
				H = temp;
				s_H = s;
			}

			// increment the position of s
			s = s + glm::length(curve(u + delta_u) - curve(u));
			// if this is past the next position then store the u value
			if(s > (float(s_index)*delta_s))
			{
				//printf("Table: %d : %10.7f at s: %10.3f\n", s_index, u, s);
				// add the next u value
				table.addNext(u);
				// increment the s index
				s_index++;
			}
		}

		table.max_height = H;
		table.s_max_height = s_H;

		return table;
	}

	float ArcLengthTable::WrapS(float s) const
	{
		// no negative values
		if (s < 0)
		{
			return std::fmod(s, arc_length) + arc_length;
		}

		return std::fmod(s, arc_length);
	}

	void ArcLengthTable::TestTable(float jump)
	{
		float s = 0;
		float u_last = 0;
		float u = 0;
		while(s < arc_length)
		{
			s = s + jump;
			u = (*this)(s);			
			printf("table(%10.3f) = %10.7f\n", s, u);
			if(u < u_last)
			{
				printf("something is wrong with the table\n");
			}

			u_last = u;
		}
	}

	//***** ******** ***** *****//
} // namespace modelling