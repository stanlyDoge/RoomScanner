/*
    This file is part of RoomScanner.

    RoomScanner is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RoomScanner is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RoomScanner.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>

class parameters
{

private:
    static parameters* instance;
    parameters() {}
public:
    // singleton class
    static parameters *GetInstance() {
        if (instance == NULL) {
            instance = new parameters();
        }
        return instance;
    }

    Eigen::Quaternionf m;

    // Default parameters (config file is not found)

    // Parameters for sift computation
    double SIFTmin_scale = 0.003;
    int SIFTn_octaves = 8;
    int SIFTn_scales_per_octave = 10;
    double SIFTmin_contrast = 0.3;

    // Parameters for MLS
    int MLSpolynomialOrder = 2;
    bool MLSusePolynomialFit = true;
    double MLSsearchRadius = 0.05;
    double MLSsqrGaussParam = 0.0025;
    double MLSupsamplingRadius = 0.025;
    double MLSupsamplingStepSize = 0.015;
    int MLSdilationIterations = 2;
    double MLSdilationVoxelSize = 0.01;
    bool MLScomputeNormals = false;

    // Parameters for Voxel Grid
    double VGFleafSize = 0.02;

    // Parameters for Greedy Projection
    double GPsearchRadius = 0.06;
    double GPmu = 2.5;
    int GPmaximumNearestNeighbors = 100;

    // Parameters for registration module
    //fpfh
    double REGnormalsRadius = 0.05;
    double REGfpfh = 1.0;
    double REGreject = 0.3;
    //icp
    double REGcorrDist = 0.2;

    // Parameters for Fast Bilateral Filter
    double FBFsigmaS = 10;
    double FBFsigmaR = 0.1;

    // Parameter for mesh decimation
    double DECtargetReductionFactor = 0.2; // 20%

    // Parameter for hole filling
    double HOLsize = 0.2;

    // Parameter for Grid projection
    double GRres = 0.01;

    // Parameter for Poisson
    int POSdepth = 9;

};



#endif // PARAMETERS_H
