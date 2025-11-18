#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
public:
	void parse_face(std::istringstream& iss);
	void parse_vertice(std::istringstream& iss);
	void parse_file(std::ifstream& in);
	explicit Model(const char *filename);
	~Model();
	[[nodiscard]] size_t nverts() const;
	[[nodiscard]] size_t nfaces() const;
	[[nodiscard]] const Vec3f& vert(int i) const;
	[[nodiscard]] const std::vector<int>& face(int idx) const;
};

#endif //__MODEL_H__
