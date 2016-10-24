#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/MeshToVolume.h>
#include <vector>
#include <iostream>
#include <fstream>

namespace
{
    struct facetuple
    {
        int32_t vertexidx;
        
        friend std::istream &operator>>(std::istream &in, facetuple &ft)
        {
            ft.vertexidx = 0;
            char c;
            while(in.peek() == ' ')
                in.get(c);
            while(in.get(c))
            {
                assert(c != '-');
                if(c >= '0' && c <= '9')
                {
                    ft.vertexidx = ft.vertexidx * 10 + c - '0';
                }
                else if(c != '/')
                {
                    std::cerr << "bad facetuple" << std::endl;
                    in.setstate(std::ios::failbit);
                    return in;
                }
                else
                {
                    break;
                }
            }
            // objs are 1 indexed
            // also, we don't understand negative indices
            --ft.vertexidx;
            if(!in)
                return in;
            // skip normals and uvs
            while(in.get(c) && c != ' ');
            in.clear();
            return in;
        }
    };
    
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
                            if(linestream.get(c))
                            {
                                if(c == ' ')
                                {
                                    openvdb::Vec3s vertex;
                                    for(int i = 0; i < 3; ++i)
                                    {
                                        linestream >> vertex[i];
                                        // 1 indexed obj
                                        --vertex[i];
                                    }
                                    o.vertices.push_back(vertex);
                                    assert(!linestream.fail());
                                }
                                else
                                {
                                    // ignore normals and uvs
                                    continue;
                                }
                                break;
                            }
                            else
                            {
                                std::cerr << "bad line: " << line;
                                return in;
                            }
                        }
                        case 'f':
                        {
                            std::vector<uint32_t> face;
                            for(int i = 0; i < 3; ++i)
                            {
                                facetuple ft;
                                linestream >> ft;
                                assert(ft.vertexidx >= 0);
                                face.push_back(static_cast<uint32_t>(ft.vertexidx));
                            }
                            if(linestream.fail())
                                std::cerr << "bad line: " << line << std::endl;
                            assert(!linestream.fail());
                            o.faces.push_back(face);
                            break;
                        }
                            // ignore other lines
                    }
                }
                else
                {
                    std::cerr << "bad line" << std::endl;
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
                    // 1 index obj
                    out << " " << (1 + v[i]);
                out << "\n";
            }
            for(auto f: o.faces)
            {
                out << "f";
                for(auto index: f)
                    // 1 index obj
                    out << " " << (1 + index);
                out << "\n";
            }
            return out;
        }
    };
}
    
int main(int argc, const char *argv[])
{
    openvdb::initialize();
    assert(argc == 3);
    std::string inname(argv[1]);
    std::ifstream infile(inname);
    assert(infile);
    obj inobj;
    infile >> inobj;
    std::vector<openvdb::Vec3I> triangles;
    for(auto const &face: inobj.faces)
    {
        assert(face.size() == 3);
        triangles.push_back(openvdb::Vec3I(face[0], face[1], face[2]));
    }
    auto grid = openvdb::tools::meshToLevelSet<openvdb::FloatGrid>(openvdb::math::Transform{}, inobj.vertices, triangles);
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
    std::ofstream out(argv[2]);
    out << outobj;
}
