#include "hermite_curve.hpp"

namespace modelling {

	HermiteCurve::ControlPoints
	HermiteCurve::buildControlPoints(std::vector<glm::vec3> posistions) {
		HermiteCurve::ControlPoints cps(posistions.size());
		for (int i = 0; i < posistions.size(); i++)
			cps[i].position = posistions[i];
		calculateCatmullRomTangents(cps);
		return cps;
	}
	void HermiteCurve::calculateCatmullRomTangents(HermiteCurve::ControlPoints& cps) {
		calculateCanonicalTangents(cps, 0.0f);
	}
	void HermiteCurve::calculateCanonicalTangents(HermiteCurve::ControlPoints& cps, float c) {
		float constant = (1.f - c) / 2.f;
		for (int index = 0; index < cps.size(); index++) {
			const ControlPoint& cp_previous = cps[index == 0 ? cps.size() - 1 : index - 1];
			const ControlPoint& cp_next     = cps[index == cps.size() - 1 ? 0 : index + 1];
			cps[index].tangent = constant * (cp_next.position - cp_previous.position);
		}
	}

	HermiteCurve::HermiteCurve(HermiteCurve::ControlPoints controlPoints)
		: m_cps(controlPoints) {}

	// evaluate curve at u
	glm::vec3 HermiteCurve::operator()(float U) const {
		assert(m_cps.size() > 0);
		//Ensure U in [0,1) wrapped (1.1 -> 0.1 and -1.1 -> 0.9)
		U = std::fmod(U, 1.f);
		if (U < 0) U = std::fmod(1.f + U, 1.f);
		//Evaluate
		return positionInSegment(localize(U));
	}
	glm::vec3 HermiteCurve::position(float U) const {
		return operator()(U);
	}

	HermiteCurve::ControlPoints const& HermiteCurve::controlPoints() const {
		return m_cps;
	}
	HermiteCurve::ControlPoints& HermiteCurve::controlPoints() { return m_cps; }

	float HermiteCurve::arcLength(float dU) const {
		assert(m_cps.size() > 0);
		assert(dU > 0.f);
		float l = 0.f;
		for (float u = 0.f; u < 1.f; u += dU) {
			l += glm::length(position(u + dU) - position(u));
		}
		return l;
	}

	std::vector<glm::vec3> HermiteCurve::sample(size_t number_of_samples) const {
		assert(m_cps.size() > 0);
		if (number_of_samples == 0) return {};
		if (number_of_samples == 1) return { m_cps[0].position };

		std::vector<glm::vec3> samples(number_of_samples);
		float dU = 1.f / float(number_of_samples - 1);
		for (int i = 0; i < number_of_samples; i++)
			samples[i] = position(i * dU);

		return samples;
	}

	givr::geometry::MultiLine HermiteCurve::controlPointGeometry() const {
		givr::geometry::MultiLine vectors;
		for (auto const& cp : m_cps) {
			vectors.push_back({ 
				givr::geometry::Point1(cp.position),
				givr::geometry::Point2(cp.position + cp.tangent)
			});
		}
		return vectors;
	}

	givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP>
	HermiteCurve::controlPointFrameGeometry() const {
		givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP> geometry;
		for (auto const& cp : m_cps)
			geometry.push_back(givr::geometry::Point(cp.position));
		return geometry;
	}

	givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP>
	HermiteCurve::sampledGeometry(size_t number_of_samples) const {
		givr::geometry::PolyLine<givr::PrimitiveType::LINE_LOOP> geometry;
		for (auto const& p : sample(number_of_samples))
			geometry.push_back(givr::geometry::Point(p));
		return geometry;
	}

	std::pair<float, size_t> HermiteCurve::localize(float U) const {
		// Assuming U in [0,1) since private funtion
		float float_seg = U * m_cps.size();
		size_t seg = size_t(std::floor(float_seg));
		float u = float_seg - seg;
		return { u, seg };
	}

	//***** STUDENTS TO-DO *****//
	// Gets the position at U (global parameter)
	glm::vec3  HermiteCurve::positionInSegment(std::pair<float, size_t> u_seg) const {
		const float& u = u_seg.first;
		const size_t& seg = u_seg.second;
		// Assuming u in [0,1) and seg in [0, num_cp) since private funtion
		size_t cp_A = seg;
		size_t cp_B = (seg + 1) % m_cps.size();
		const glm::vec3& P_A = m_cps[cp_A].position;
		const glm::vec3& T_A = m_cps[cp_A].tangent;
		const glm::vec3& P_B = m_cps[cp_B].position;
		const glm::vec3& T_B = m_cps[cp_B].tangent;


		// cubic hermite curve definition
		float u_2 = u*u;
		float u_3 = u_2*u;
		glm::vec3 p = P_A*(2*u_3 - 3 * u_2 + 1) + T_A * (u_3 - 2*u_2 + u) + P_B * (3*u_2 - 2 * u_3) + T_B * (u_3 - u_2);
		return p;
	}
	//***** ******** ***** *****//

	size_t HermiteCurve::size() const
	{
		return m_cps.size();
	}

	float HermiteCurve::minSeperation()
	{
		glm::vec3 v = (m_cps[0].position - m_cps[m_cps.size() - 1].position); 
		float min_dist = v.x*v.x + v.y*v.y + v.z*v.z;

		for(size_t i = 0; i < m_cps.size()-1; i++)
		{
			v = (m_cps[i].position - m_cps[i+1].position);
			float temp = v.x * v.x + v.y * v.y + v.z * v.z;
			if(temp < min_dist)
			{
				min_dist = temp;
			}
		}

		return glm::sqrt(min_dist);
	}

	float HermiteCurve::maxSeperation()
	{
		glm::vec3 v = (m_cps[0].position - m_cps[m_cps.size() - 1].position);
		float max_dist = v.x * v.x + v.y * v.y + v.z * v.z;

		for (size_t i = 0; i < m_cps.size() - 1; i++)
		{
			v = (m_cps[i].position - m_cps[i + 1].position);
			float temp = v.x * v.x + v.y * v.y + v.z * v.z;
			if (temp > max_dist)
			{
				max_dist = temp;
			}
		}

		return glm::sqrt(max_dist);
	}
} // namespace modelling