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
#include "arc_length_parameterize.hpp"
#include "track.hpp"
#include <glm/glm.hpp>
#include <vector>

#define SAMPLING_FACTOR 0.01f
#define BASE_LEVEL 10.0f
#define SUPPORT_HEIGHT 8.0f
#define MAP_SIZE 100.0f

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
         * @param _delta_s the delta_s value to use for generating the arc length parameterize table
         * @param _delta_h the look ahead distance
         * @param _sup_spacing the spacing between support pieces (the things that hold the track in the air)
         * @param _num_trees the number of trees to put in the scene (trees look nice)
         */
        RollerCoaster(float _s_dist, float _min_v, float _decel_frac, float _delta_s, float _delta_h, float _sup_spacing, int _num_trees);

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
         * @param h the look ahead distance
         */
        void UpdateTrack(float _s_dist, float _min_v, float _decel_frac, float h);

        /**
         * get the transform matrix at a position s
         * @param s the position allong the track to get the transform at
         */
        glm::mat4 GetTransformAtPosition(float s) const;

        /**
         * get the transform oriented with the track but level (no accounting for the acceleration)
         */
        glm::mat4 GetLevelTransformAtPosition(float s) const;

        // get the raw position at this s coordinate
        glm::vec3 GetPositionAtS(float s) const;

        // get a reference to the track piece transforms
        std::vector<glm::mat4> *pieceTransforms();

        // get the support transforms
        std::vector<glm::mat4> *SupportTransforms();

        // get the tree transforms
        std::vector<glm::mat4> *TreeTransforms();

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
        float delta_h; // used for finding the normal to the curve

        // extra stuff
        float support_spacing;
        int num_trees;

        // the array of transforms for each to the supports
        std::vector<glm::mat4> support_transforms = std::vector<glm::mat4>(0);
        // the array of transforms for the trees
        std::vector<glm::mat4> tree_transforms = std::vector<glm::mat4>(0);

        // gravity
        glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

        void PrintMat4(glm::mat4 M) const;

        // creates the array of tree transforms
        void GenerateTrees();
        // creates the array of support transforms
        void GenerateSupports();
    };
}