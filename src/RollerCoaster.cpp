#include "RollerCoaster.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace modelling
{

    RollerCoaster::RollerCoaster(float _s_dist, float _min_v, float _decel_frac, float _delta_s, float _delta_h)
    {
        // initialize values
        s_dist = _s_dist;
        min_v = _min_v;
        decel_frac = _decel_frac;
        delta_s = _delta_s;
        delta_h = _delta_h;
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
        // get the positions at this s value
        glm::vec3 p = curve(table(s));
        glm::vec3 p_nh = curve(table(s - delta_h));
        glm::vec3 p_h = curve(table(s + delta_h));

        // the vectors forming the triangle
        glm::vec3 a = p - p_nh;
        glm::vec3 b = p_h - p;
        glm::vec3 c = p_h - p_nh;

        // the tangents
        glm::vec3 T = c / glm::length(c);
        glm::vec3 t0 = a / glm::length(a);
        glm::vec3 t1 = b / glm::length(b);

        // the normal vector
        glm::vec3 n_raw = t0 - t1;
        n_raw = n_raw / glm::length(n_raw);
        n_raw = n_raw - (glm::dot(n_raw, T)*T);
        n_raw = n_raw / glm::length(n_raw);

        // the curvature
        float k = 2.0f * glm::length(glm::cross(t1, t0)) / glm::length(c);

        // the acceleration at this point
        float v = GetSpeedAtPos(s);
        glm::vec3 acc = n_raw * k * v * v;

        // gravity tangent
        glm::vec3 g_tan = gravity - glm::dot(gravity, T)*T;

        // total acceleration vector
        acc = acc - g_tan;

        // true normal
        n_raw = acc / glm::length(acc);

        // bi-normal
        glm::vec3 B = glm::cross(n_raw, T);

        glm::mat4 M = glm::mat4{1.0f};
        
        
        M[0][0] = B.x;
        M[0][1] = B.y;
        M[0][2] = B.z;

        M[1][0] = n_raw.x;
        M[1][1] = n_raw.y;
        M[1][2] = n_raw.z;

        M[2][0] = T.x;
        M[2][1] = T.y;
        M[2][2] = T.z;
        
        M[3][0] = p.x;
        M[3][1] = p.y;
        M[3][2] = p.z;
        

        // create the transform matrix
        M = glm::scale(M, glm::vec3{0.75});
        PrintMat4(M);
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

    void RollerCoaster::PrintMat4(glm::mat4 M) const
    {

        printf("Matrix:\n\n");
        for(int i =0; i < 4; i++)
        {
            printf("| ");
            for(int j = 0; j < 4; j++)
            {
                printf("%6.2f", M[j][i]);
            }

            printf(" |\n");
        }
    }
}