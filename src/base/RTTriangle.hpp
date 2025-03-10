#pragma once


#include "3d/Mesh.hpp"
#include "base/math.hpp"


namespace FW {

    struct tri_data {
        Vec3i vertex_indices; // indices to the vertex array of the mesh
        Mat3f M; Vec3f N; // for Woop intersection
        tri_data() : M(), N() {}
        tri_data(const tri_data& other) : M(other.M), N(other.N), vertex_indices(other.vertex_indices) {}
        tri_data(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Vec3f normal) {

            M.setCol(0, v1 - v0);
            M.setCol(1, v2 - v0);
            M.setCol(2, normal);

            M.invert();
            N = -M * v0;
        }
    };
    // The user pointer member can be used for identifying the triangle in the "parent" mesh representation.
    struct RTTriangle {

        VertexPNTC			m_vertices[3];			// The vertices of the triangle.

        MeshBase::Material* m_material;				// Material of the triangle
        tri_data			m_data;					// Holds the matrix and vector necessary for Woop intersection and vertex index in the mesh

        // Note: Please do not add new member variables to this class.
        //
        // Doing so will cause tricky problems starting from Assignment 2 which includes
        // a prebuilt reference ray tracer using this class. There will be a version conflict with your
        // std::vector<RTTriangle> and the one found in the automatically linked reference library.
        //
        // Recommendation A:
        //      Another way is to deal with two arrays instead of one. One storing these RTTriangles and
        //      the other storing the extra data. If you keep these in the same order, everything will be fine
        //      if you query them both with the same indices.
        //
        // Recommendation B:
        //      Remove all traces to the reference implementation, including linking to it.
        //      Then you are free to do whatever you want with this class. Please consider this option
        //      only if your ray tracer works perfectly and is at least as fast as the reference implementation.
        //      If your ray tracer is not competitive with the reference implementation, there may be a 
        //      bug somewhere and you may want to either fix it or use the reference implementation in later
        //      assignments, at least to make sure that everything works.
        //
        // In any case, adding member functions is fine.

        RTTriangle(const VertexPNTC v0, const VertexPNTC v1, const VertexPNTC v2) {
            m_vertices[0] = v0;
            m_vertices[1] = v1;
            m_vertices[2] = v2;
            m_data = tri_data(v0.p, v1.p, v2.p, normal());
        }


        inline Vec3f min() const {
            return FW::min(m_vertices[0].p, m_vertices[1].p, m_vertices[2].p);
        }

        inline Vec3f max() const {
            return FW::max(m_vertices[0].p, m_vertices[1].p, m_vertices[2].p);
        }

        inline Vec3f centroid() const {
            return (m_vertices[0].p + m_vertices[1].p + m_vertices[2].p) * (1.0f / 3.0f);
        }

        inline Vec3f bbCentroid() const
        {
            return 0.5f * (max() + min());
        }

        inline float area() const {
            return cross(m_vertices[1].p - m_vertices[0].p, m_vertices[2].p - m_vertices[0].p).length() * .5f;
        }

        Vec3f normal() const {
            return cross(m_vertices[1].p - m_vertices[0].p, m_vertices[2].p - m_vertices[0].p).normalized();
        }

        //Triangle intersection as suggested in [Woop04]

        bool RTTriangle::intersect_woop(const Vec3f& orig, const Vec3f& dir, float& t, float& u, float& v) const {

            Vec3f transformed_orig = m_data.M * orig + m_data.N,
                transformed_dir = m_data.M * dir;

            t = -transformed_orig.z / transformed_dir.z;
            u = transformed_orig.x + transformed_dir.x * t;
            v = transformed_orig.y + transformed_dir.y * t;

            return u > .0f && v > .0f && u + v < 1.0f;
        }

    };


}
