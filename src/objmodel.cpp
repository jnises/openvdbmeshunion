#include "objmodel.h"

namespace deepness
{
    namespace objmodel
    {
        std::istream &operator>>(std::istream &in, facetuple &ft)
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

        std::istream &operator>>(std::istream &in, obj &o)
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

        std::ostream &operator<<(std::ostream &out, const obj &o)
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
    }
}
