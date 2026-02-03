#pragma once

#include "hermite_curve.hpp"
#include "arc_length_parameterize.hpp"
#include <glm/glm.hpp>
#include <vector>



namespace modelling
{
    class RollerCoaster;
    /**
     * creates and storest the geometry information for generating the track pieces
     */
    class Track
    {
    public:
        Track();
        /**
         * generate the positions and rotations of the track pieces for a curve
         * @param _s_dist the distance between each piece
         * @param _min_v the minium speed of the roller coaster
         * @param _u_decel the u index after which the cart begins to decelerate
         * @param table the arcLengthTable to use 
         * @param curve the curve to use
         */
        void setupTrack(RollerCoaster* roller_coaster, float _s_dist);

        // get a reference to the track piece transforms
        std::vector<glm::mat4>* pieceTransforms();

    private:
        // the array of transforms for each to the track pieces
        std::vector<glm::mat4> piece_transforms = std::vector<glm::mat4>(0);
        // the distance between each piece
        float s_dist = 1.0;
    };
}