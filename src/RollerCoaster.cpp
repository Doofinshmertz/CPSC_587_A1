#include "RollerCoaster.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace modelling
{

    RollerCoaster::RollerCoaster(float _s_dist, float _min_v, float _decel_frac, float _delta_s)
    {
        // initialize values
        s_dist = _s_dist;
        min_v = _min_v;
        decel_frac = _decel_frac;
        delta_s = _delta_s;
    }

    RollerCoaster::~RollerCoaster()
    {

    }

    void RollerCoaster::UpdateCurve(HermiteCurve new_curve)
    {
        curve = new_curve;

        // we want the delta u value to result in smaller distance jumps than delta_s
        // first calculate the maximum change in distance vs change in u max(dS/du)
        // this is calculated as max_seperation * number_points
        // then we require that delta_u * dS/du < delta_s or delta_u < du/dS * delta_s
        delta_u = (0.1 * delta_s) / (float(curve.size()) * curve.maxSeperation());
        table = calculateArcLengthTable(curve, delta_s, delta_u);
        track.setupTrack(this, s_dist);

        // calculate the velocity value at the u value just before the decceleration point
        // first get the hight
        s_start_dec = table.arc_length * decel_frac;
        float z = curve(table(s_start_dec)).y;
        // get the max height
        H = table.max_height;
        s_freefall = table.s_max_height;
        // calculate the velocity at this point using the conservation of energy
        float v_dec = std::sqrt(19.62 * (H - z) + min_v * min_v);
        // bound with min_v
        v_start_dec = std::max(v_dec, min_v);

        printf("curve updated, H: %10.2f, v_start_dec: %10.2f\n", H, v_start_dec);
    }

    void RollerCoaster::UpdateArcLengthTable(float _delta_s)
    {
        delta_s = _delta_s;
        delta_u = (0.1 * delta_s) / (float(curve.size()) * curve.maxSeperation());
        table = calculateArcLengthTable(curve, delta_s, delta_u);
        track.setupTrack(this, s_dist);
    }

    void RollerCoaster::UpdateTrack(float _s_dist, float _min_v, float _decel_frac)
    {
        s_dist = _s_dist;
        min_v = _min_v;
        decel_frac = _decel_frac;

        // calculate the velocity value at the u value just before the decceleration point
        // first get the hight
        s_start_dec = table.arc_length * decel_frac;
        float z = curve(table(s_start_dec)).y;
        // get the max height
        H = table.max_height;
        s_freefall = table.s_max_height;
        // calculate the velocity at this point using the conservation of energy
        float v_dec = std::sqrt(19.62 * (H - z) + min_v * min_v);
        // bound with min_v
        v_start_dec = std::max(v_dec, min_v);

        track.setupTrack(this, s_dist);
    }

    glm::mat4 RollerCoaster::GetTransformAtPosition(float s) const
    {
        // get the position at this s value
        glm::vec3 p = curve(table(s));
        // create the transform matrix
        glm::mat4 M = glm::scale(glm::translate(glm::mat4{1.0f}, p), glm::vec3{0.75});
        // store the transform matrix
        return M;
    }

    std::vector<glm::mat4> *RollerCoaster::pieceTransforms()
    {
        return track.pieceTransforms();
    }

    float RollerCoaster::GetSpeedAtPos(float s) const
    {
        // check to see if this is durring the deceleration region or the lifting region
        s = WrapS(s);
        // deceleration region use decceleration function
        if(s > s_start_dec)
        {
            return v_start_dec + (s - s_start_dec) * (min_v - v_start_dec) / (table.arc_length - s_start_dec);
        }
        // lifting region use min velocity
        else if(s < s_freefall)
        {
            return min_v;
        }

        // use the conservation of energy or min speed everywhere else
        float z = curve(table(s)).y;
        float v = std::sqrt(19.62 * (H - z) + min_v * min_v);
        return std::max(v, min_v);
    }

    float RollerCoaster::ArcLength() const
    {
        return table.arc_length;
    }

    float RollerCoaster::WrapS(float s) const
    {
        // no negative values
        if(s < 0)
        {
            return 0;
        }

        return std::fmod(s, table.arc_length);
    }
}