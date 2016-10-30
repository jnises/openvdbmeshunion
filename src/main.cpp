#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/Composite.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "objmodel.h"
#include <boost/program_options.hpp>

using namespace deepness::objmodel;
namespace po = boost::program_options;
using namespace std;

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
    float resolution;
    po::options_description desc("options");
    desc.add_options()
    ("help", "produce help message")
    ("resolution", po::value<float>(&resolution)->default_value(1), "spatial resolution")
    ("output,O", po::value<string>())
    ("input,I", po::value<vector<string>>())
    ;
    po::positional_options_description p;
    p.add("input", -1);
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);
    
    if (vm.count("help"))
    {
        cerr << desc << "\n";
        return 1;
    }
    
    string output;
    vector<string> input;
    if(vm.count("output"))
    {
        output = vm["output"].as<string>();
    }
    else
    {
        cerr << "output required" << endl;
        return 1;
    }
    
    if(vm.count("input"))
    {
        input = vm["input"].as<vector<string>>();
    }
    if(input.size() != 2)
    {
        cerr << "2 input files required" << endl;
        return 1;
    }
    
    openvdb::initialize();
    openvdb::FloatGrid::Ptr grid;
    {
        std::ifstream infileA(input[0]);
        grid = loadMesh(infileA);
        std::ifstream infileB(input[1]);
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
    std::ofstream out(output);
    out << outobj;
    return 0;
}
