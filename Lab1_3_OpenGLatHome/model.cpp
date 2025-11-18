#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include "model.h"

void Model::parse_face(std::istringstream &iss)
{
    char c;
    iss >> c;

    std::vector<int> face;
    face.reserve(3);

    std::string vertdef;
    while (iss >> vertdef) {
        std::istringstream viss(vertdef);
        int idx = 0, trash = 0;
        char slash;

        // parse vertex/texture/normal
        // Format: v/t/n or v//n or v
        viss >> idx;
        if (viss.peek() == '/') {
            viss >> slash;
            if (viss.peek() != '/') viss >> trash;  // texture index
            if (viss.peek() == '/') viss >> slash >> trash; // normal index
        }

        idx--;  // OBJ indices are 1-based
        face.push_back(idx);
    }

    if (!face.empty()) {
        faces_.push_back(face);
    }
}

void Model::parse_vertice(std::istringstream &iss)
{
    char c;
    Vec3f v;
    iss >> c >> v.raw[0] >> v.raw[1] >> v.raw[2];
    verts_.push_back(v);
}

void Model::parse_file(std::ifstream &in)
{
    std::string line;
    verts_.reserve(1000);
    faces_.reserve(1000);

    while (std::getline(in, line)) {
        if (line.size() < 2) continue;

        std::istringstream iss(line);
        if (line.rfind("v ", 0) == 0) {
            parse_vertice(iss);
        }
        else if (line.rfind("f ", 0) == 0) {
            parse_face(iss);
        }
    }

    std::cerr << "# v: " << verts_.size()
        << "   f: " << faces_.size() << std::endl;
}

Model::Model(const char* filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Failed to open model file: " + std::string(filename));
    }

    parse_file(in);
}

Model::~Model() = default;

size_t Model::nverts() const {
    return verts_.size();
}

size_t Model::nfaces() const {
    return faces_.size();
}

const std::vector<int>& Model::face(int idx) const {
    return faces_.at(idx);
}

const Vec3f& Model::vert(int i) const {
    return verts_.at(i);
}
