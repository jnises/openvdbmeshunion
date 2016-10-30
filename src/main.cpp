#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/Composite.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "objmodel.h"

using namespace deepness::objmodel;

namespace
{
    openvdb::FloatGrid::Ptr loadMesh(std::istream &infile)
    {
        assert(infile);
        obj inobj;
        infile >> inobj;
        std::vector<openvdb::Vec3I> triangles;
        for(auto const &face: inobj.faces)
        {
            assert(face.size() == 3);
            triangles.push_back(openvdb::Vec3I(face[0], face[1], face[2]));
        }
        return openvdb::tools::meshToLevelSet<openvdb::FloatGrid>(openvdb::math::Transform{}, inobj.vertices, triangles);
    }
}

int main(int argc, const char *argv[])
{
    if(argc != 4)
    {
        std::cerr << "usage: " << argv[0] << " inA inB out" << std::endl;
        return 1;
    }
    openvdb::initialize();
    openvdb::FloatGrid::Ptr grid;
    {
        std::ifstream infileA(argv[1]);
        grid = loadMesh(infileA);
        std::ifstream infileB(argv[2]);
        auto inB = loadMesh(infileB);
        openvdb::tools::csgUnion(*grid, *inB);
    }

    obj outobj;
    std::vector<openvdb::Vec3I> tris;
    std::vector<openvdb::Vec4I> quads;
    openvdb::tools::volumeToMesh(*grid, outobj.vertices, tris, quads);
    std::vector<std::vector<uint32_t>> outfaces;
    for(auto tri: tris)
    {
        std::vector<uint32_t> face;
        for(int i = 0; i < 3; ++i)
            face.push_back(tri[i]);
        outobj.faces.push_back(std::move(face));
    }
    for(auto quad: quads)
    {
        std::vector<uint32_t> face;
        for(int i = 0; i < 4; ++i)
            face.push_back(quad[i]);
        outobj.faces.push_back(std::move(face));
    }
    std::ofstream out(argv[3]);
    out << outobj;
    return 0;
}
