#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include "model.h"

void Model::parse_face(std::istringstream& iss)
{
    char c;
    iss >> c;

    std::vector<int> face;
    std::vector<int> uv_indices;
    face.reserve(3);
    uv_indices.reserve(3);

    std::string vertdef;
    while (iss >> vertdef)
    {
        std::istringstream viss(vertdef);
        int vertex_index = 0;
        int tex_index = -1;

        viss >> vertex_index;
        if (viss.peek() == '/')
        {
            char slash;
            viss >> slash;
            if (viss.peek() != '/' && !viss.eof())
            {
                viss >> tex_index;
            }
            if (viss.peek() == '/')
            {
                int normal_index = -1;
                viss >> slash >> normal_index;
            }
        }

        vertex_index -= 1;
        if (tex_index >= 1)
        {
            tex_index -= 1;
        }
        else
        {
            tex_index = -1;
        }

        face.push_back(vertex_index);
        uv_indices.push_back(tex_index);
    }

    if (!face.empty())
    {
        faces_.push_back(face);
        texcoord_indices_.push_back(uv_indices);
    }
}

void Model::parse_vertice(std::istringstream& iss)
{
    char c;
    Vec3f v;
    iss >> c >> v.x >> v.y >> v.z;
    if (iss.fail())
    {
        return;
    }
    verts_.push_back(v);
}

void Model::parse_texcoord(std::istringstream& iss)
{
    char c1, c2;
    Vec2f uv;
    iss >> c1 >> c2 >> uv.x >> uv.y;
    if (iss.fail())
    {
        return;
    }
    texcoords_.push_back(uv);
}

void Model::parse_file(std::ifstream& in)
{
    std::string line;
    verts_.reserve(1000);
    faces_.reserve(1000);
    texcoords_.reserve(1000);
    texcoord_indices_.reserve(1000);

    while (std::getline(in, line))
    {
        if (line.size() < 2)
        {
            continue;
        }

        std::istringstream iss(line);
        if (line.rfind("v ", 0) == 0)
        {
            parse_vertice(iss);
        }
        else if (line.rfind("vt ", 0) == 0)
        {
            parse_texcoord(iss);
        }
        else if (line.rfind("f ", 0) == 0)
        {
            parse_face(iss);
        }
    }

    std::cerr << "# v: " << verts_.size() << "   f: " << faces_.size() << std::endl;
}

Model::Model(const String& filename)
{
    std::ifstream in(filename);
    if (!in)
    {
        throw std::runtime_error("Failed to open model file: " + filename);
    }

    parse_file(in);
}

Model::~Model() = default;

size_t Model::nverts() const
{
    return verts_.size();
}

size_t Model::nfaces() const
{
    return faces_.size();
}

const std::vector<int>& Model::face(int idx) const
{
    return faces_.at(idx);
}

const Vec3f& Model::vert(int i) const
{
    return verts_.at(i);
}

Vec2f Model::texcoord(int face_index, int vertex_index) const
{
    if (texcoord_indices_.empty())
    {
        return Vec2f(0.0f, 0.0f);
    }
    const auto& indices = texcoord_indices_.at(face_index);
    if (vertex_index >= static_cast<int>(indices.size()))
    {
        return Vec2f(0.0f, 0.0f);
    }
    const int tex_index = indices[vertex_index];
    if (tex_index < 0 || tex_index >= static_cast<int>(texcoords_.size()))
    {
        return Vec2f(0.0f, 0.0f);
    }
    return texcoords_.at(tex_index);
}
