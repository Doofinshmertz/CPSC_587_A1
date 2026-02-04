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


        if(_s_dist > (0.1 * roller_coaster->ArcLength()))
        {
            printf("WARNING: track piece seperation distance results in small number of track pieces\n");
        }

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