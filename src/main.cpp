#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <vector>
#include <iostream>
#include <fstream>



int main()
{
    openvdb::initialize();
    // Create a FloatGrid and populate it with a narrow-band
    // signed distance field of a sphere.
    openvdb::FloatGrid::Ptr grid =
    openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(
                                                             /*radius=*/50.0, /*center=*/openvdb::Vec3f(1.5, 2, 3),
                                                             /*voxel size=*/0.5, /*width=*/4.0);
    std::vector<openvdb::Vec3s> points;
    std::vector<openvdb::Vec3I> tris;
    std::vector<openvdb::Vec4I> quads;
    openvdb::tools::volumeToMesh(*grid, points, tris, quads);
    std::ofstream out("out.obj");
    for(auto point: points)
        out << "v " << point[0] << " " << point[1] << " " << point[2] << "\n";
    for(auto tri: tris)
        out << "f " << tri[0] + 1 << " " << tri[1] + 1 << " " << tri[2] + 1 << "\n";
    for(auto quad: quads)
        out << "f " << quad[0] + 1 << " " << quad[1] + 1 << " " << quad[2] + 1 << "\n";
}
