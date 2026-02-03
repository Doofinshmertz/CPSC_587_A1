#pragma once

#include "hermite_curve.hpp"
#include "arc_length_parameterize.hpp"
#include "track.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace modelling
{
    /**
     * Manages the curve, arclength table, track pieces, and cart movement
     */
    class RollerCoaster
    {
    public:
        
        /**
         * initialize the roller coaster
         * @param _s_dist the seperation distance for the track pieces
         * @param _min_v the minimum velocity
         * @param _decel_frac the fraction of the curve distance a which point the deceleration starts
         */
        RollerCoaster(float _s_dist, float _min_v, float _decel_frac, float delta_s);

        /**
         * delete the curve, arclength table, and track
         */
        ~RollerCoaster();

        /**
         * updates to use the new given curve, also creates a new arc length table at updates the track
         * @param new_curve the new curve to use
         */
        void UpdateCurve(HermiteCurve new_curve);

        /**
         * updates the arc length table (and also the track because it depends on this)
         * @param _delta_s the new s step to use
         */
        void UpdateArcLengthTable(float _delta_s);

        /**
         * updates the track using new motion parameters
         * @param _s_dist the seperation distance for the track pieces
         * @param _min_v the minium velocity
         * @param _decel_frac the fraction of the curve distance a which point the deceleration starts
         */
        void UpdateTrack(float _s_dist, float _min_v, float _decel_frac);

        /**
         * get the transform matrix at a position s
         * @param s the position allong the track to get the transform at
         */
        glm::mat4 GetTransformAtPosition(float s) const;

        // get a reference to the track piece transforms
        std::vector<glm::mat4> *pieceTransforms();

        /**
         * calculate the speed at an s position allong the track based on the movement parameters and conservation of energy
         */
        float GetSpeedAtPos(float s) const;

        // get the arc length
        float ArcLength() const;

        /**
         */
    private:

        HermiteCurve curve;
        ArcLengthTable table;
        Track track;

        // related to motion
        float s_dist; // the distance between track pieces
        float min_v; // the minium velocity
        float decel_frac; // the fraction of the curve distance at which the deceleration starts
        float s_start_dec; // the s distance at which deceleration starts
        float v_start_dec; // the velocity when the cart starts to decelerate
        float s_freefall; // the s position at which the freefall begins
        float H; // the maximum height

        // related to arc length parameterization
        float delta_u;
        float delta_s;


        /**
         * wrap s around the length of the track
         */
        float WrapS(float s) const;
    };
}