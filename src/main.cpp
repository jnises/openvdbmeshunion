#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/MeshToVolume.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/optional.hpp>

// boost::optional<float> parseFloat(std::istream &in)
// {
//     char c;
//     while(in.get(c))
//     {
        
//     }
// }

// void parseObj(std::istream &objfile, std::vector<openvdb::Vec3I> indices, std::vector<openvdb::Vec3s> vertices)
// {
//     enum
//     {
//         start_of_line,
//         vertex_x, vertex_y, vertex_z,
//         face,
//         ignored_line,
//     } state = start_of_line;
//     indices.clear();
//     vertices.clear();
//     openvdb::Vec3I vertex;
//     char c;
//     while(objfile.get(c))
//     {
//         switch(state)
//         {
//             case start_of_line:
//             {
//                 switch(c)
//                 {
//                     case 'v':
//                         state = vertex_x;
//                         break;
//                     case 'f':
//                         state = face;
//                         break;
//                     default:
//                         state = ignored_line;
//                 }
//                 break;
//             }
//             case vertex_x:
//             {
//                 switch(c)
//                 {
//                         case ' ':
//                 }
//                 break;
//             }
//         }
//     }
// }

struct obj
{
    std::vector<openvdb::Vec3s> vertices;
    std::vector<std::vector<uint32_t>> faces;
    
    friend std::istream &operator>>(std::istream &in, obj &o)
    {
        o.faces.clear();
        o.vertices.clear();
        std::string line;
        while(std::getline(in, line))
        {
            std::stringstream linestream(line);
            char c;
            if(linestream.get(c))
            {
                switch(c)
                {
                    case 'v':
                    {
                        openvdb::Vec3s vertex;
                        for(int i = 0; i < 3; ++i)
                            linestream >> vertex[i];
                        o.vertices.push_back(vertex);
                        assert(linestream);
                        break;
                    }
                    case 'f':
                    {
                        std::vector<uint32_t> face;
                        for(int i = 0; i < 3; ++i)
                        {
                            uint32_t idx;
                            linestream >> idx;
                            face.push_back(idx);
                        }
                        assert(linestream);
                        o.faces.push_back(face);
                        break;
                    }
                        // ignore other lines
                }
            }
        }
        return in;
    }
    
    friend std::ostream &operator<<(std::ostream &out, const obj &o)
    {
        for(auto v: o.vertices)
        {
            out << "v";
            for(int i = 0; i < 3; ++i)
                out << " " << v[i];
            out << "\n";
        }
        for(auto f: o.faces)
        {
            out << "f";
            for(auto index: f)
                out << " " << index;
            out << "\n";
        }
        return out;
    }
};

int main(int argc, const char *argv[])
{
    openvdb::initialize();
    assert(argc == 3);
    std::ifstream infile(argv[1]);
    obj o;
    infile >> o;
    std::vector<openvdb::Vec3I> triangles;
    for(auto const &face: o.faces)
    {
        assert(face.size() > 3);
        triangles.push_back(openvdb::Vec3I(face[0], face[1], face[2]));
    }
    auto grid = openvdb::tools::meshToLevelSet<openvdb::FloatGrid>(openvdb::math::Transform{}, o.vertices, triangles);
    std::vector<openvdb::Vec3s> points;
    std::vector<openvdb::Vec3I> tris;
    std::vector<openvdb::Vec4I> quads;
    openvdb::tools::volumeToMesh(*grid, points, tris, quads);
    std::ofstream out(argv[2]);
    for(auto point: points)
        out << "v " << point[0] << " " << point[1] << " " << point[2] << "\n";
    for(auto tri: tris)
        out << "f " << tri[0] + 1 << " " << tri[1] + 1 << " " << tri[2] + 1 << "\n";
    for(auto quad: quads)
        out << "f " << quad[0] + 1 << " " << quad[1] + 1 << " " << quad[2] + 1 << "\n";
}
