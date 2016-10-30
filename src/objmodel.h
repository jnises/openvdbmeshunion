#pragma once
#include <iosfwd>
#include <vector>
#include <openvdb/openvdb.h>

namespace deepness
{
    namespace objmodel
    {
        struct facetuple
        {
            int32_t vertexidx;

            friend std::istream &operator>>(std::istream &in, facetuple &ft);
        };

        // can only handle triangulated meshes
        struct obj
        {
            std::vector<openvdb::Vec3s> vertices;
            std::vector<std::vector<uint32_t>> faces;

            friend std::istream &operator>>(std::istream &in, obj &o);
            friend std::ostream &operator<<(std::ostream &out, const obj &o);
        };
    }
}
