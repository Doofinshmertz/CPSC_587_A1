/**
 * CPSC 587 W26 Assignment 1
 * @name Holden Holzer
 * @email holden.holzer@ucalgary.ca
 *
 * Modified from provided Assignment 1 - Boilerplate
 * @authors Copyright 2019 Lakin Wecker, Jeremy Hart, Andrew Owens and Others (see AUTHORS)
 */

#include "track.hpp"
#include "RollerCoaster.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace modelling
{
    Track::Track()
    {

    }

    void Track::setupTrack(RollerCoaster* roller_coaster, float _s_dist, float h)
    {
        // sudo code
        // find the number of pieces needed
        // step through each s position and calculate the position and rotation
        // add the position and rotation to the array

        // set the number of track pieces that will fit on this track
        size_t num_pieces = size_t(roller_coaster->ArcLength() / _s_dist) + 1;
        piece_transforms = std::vector<glm::mat4>(num_pieces);

        // loop through the distances to get the positions
        float s = 0;
        for(size_t i = 0; i < num_pieces; i++)
        {
            // store the transform matrix
            piece_transforms[i] = roller_coaster->GetTransformAtPosition(s);

            // increment the s value
            s = s + _s_dist;
        }
    }

    std::vector<glm::mat4> * Track::pieceTransforms()
    {
        return &piece_transforms;
    }
}