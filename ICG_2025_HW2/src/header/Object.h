#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <tiny_obj_loader.h>

using namespace std;

enum class FACETYPE
{
	TRIANGLE,
	QUAD
};

class Object
{
public:
	vector<float> positions;
	vector<float> normals;
	vector<float> texcoords;
	FACETYPE faceType = FACETYPE::TRIANGLE;

	Object(const string& filename)
	{
		loadOBJ(filename);
	}

private:
	unsigned int VAO;
	int vertex_cnt;

	void loadOBJ(const string& filename) {
		vector<tinyobj::shape_t> shapes;
		vector<tinyobj::material_t> materials;
		string err;

		bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str());

		if (!err.empty()) {
			cerr << "Error loading OBJ: " << err << endl;
		}

		if (!ret) {
			cerr << "Failed to load OBJ file: " << filename << endl;
			return;
		}

		// Process all shapes
		for (const auto& shape : shapes) {
			const tinyobj::mesh_t& mesh = shape.mesh;
			
			// Check if we have quads by examining face vertex counts
			for (size_t i = 0; i < mesh.num_vertices.size(); i++) {
				if (mesh.num_vertices[i] == 4) {
					faceType = FACETYPE::QUAD;
					break;
				}
			}

			// Process faces
			size_t index_offset = 0;
			for (size_t f = 0; f < mesh.num_vertices.size(); f++) {
				int fv = mesh.num_vertices[f];
				
				if (fv == 3) {
					// Triangle
					for (size_t v = 0; v < 3; v++) {
						unsigned int idx = mesh.indices[index_offset + v];
						
						// Positions
						if (idx * 3 + 2 < mesh.positions.size()) {
							positions.push_back(mesh.positions[idx * 3 + 0]);
							positions.push_back(mesh.positions[idx * 3 + 1]);
							positions.push_back(mesh.positions[idx * 3 + 2]);
						} else {
							positions.push_back(0.0f);
							positions.push_back(0.0f);
							positions.push_back(0.0f);
						}

						// Texture coordinates
						if (!mesh.texcoords.empty() && idx * 2 + 1 < mesh.texcoords.size()) {
							texcoords.push_back(mesh.texcoords[idx * 2 + 0]);
							texcoords.push_back(mesh.texcoords[idx * 2 + 1]);
						} else {
							texcoords.push_back(0.0f);
							texcoords.push_back(0.0f);
						}

						// Normals
						if (!mesh.normals.empty() && idx * 3 + 2 < mesh.normals.size()) {
							normals.push_back(mesh.normals[idx * 3 + 0]);
							normals.push_back(mesh.normals[idx * 3 + 1]);
							normals.push_back(mesh.normals[idx * 3 + 2]);
						} else {
							normals.push_back(0.0f);
							normals.push_back(1.0f);
							normals.push_back(0.0f);
						}
					}
				} else if (fv == 4) {
					// Quad - convert to two triangles
					vector<unsigned int> quad_indices;
					for (size_t v = 0; v < 4; v++) {
						quad_indices.push_back(mesh.indices[index_offset + v]);
					}

					// First triangle: 0, 1, 2
					for (int i : {0, 1, 2}) {
						unsigned int idx = quad_indices[i];
						
						// Positions
						if (idx * 3 + 2 < mesh.positions.size()) {
							positions.push_back(mesh.positions[idx * 3 + 0]);
							positions.push_back(mesh.positions[idx * 3 + 1]);
							positions.push_back(mesh.positions[idx * 3 + 2]);
						} else {
							positions.push_back(0.0f);
							positions.push_back(0.0f);
							positions.push_back(0.0f);
						}

						// Texture coordinates
						if (!mesh.texcoords.empty() && idx * 2 + 1 < mesh.texcoords.size()) {
							texcoords.push_back(mesh.texcoords[idx * 2 + 0]);
							texcoords.push_back(mesh.texcoords[idx * 2 + 1]);
						} else {
							texcoords.push_back(0.0f);
							texcoords.push_back(0.0f);
						}

						// Normals
						if (!mesh.normals.empty() && idx * 3 + 2 < mesh.normals.size()) {
							normals.push_back(mesh.normals[idx * 3 + 0]);
							normals.push_back(mesh.normals[idx * 3 + 1]);
							normals.push_back(mesh.normals[idx * 3 + 2]);
						} else {
							normals.push_back(0.0f);
							normals.push_back(1.0f);
							normals.push_back(0.0f);
						}
					}

					// Second triangle: 0, 2, 3
					for (int i : {0, 2, 3}) {
						unsigned int idx = quad_indices[i];
						
						// Positions
						if (idx * 3 + 2 < mesh.positions.size()) {
							positions.push_back(mesh.positions[idx * 3 + 0]);
							positions.push_back(mesh.positions[idx * 3 + 1]);
							positions.push_back(mesh.positions[idx * 3 + 2]);
						} else {
							positions.push_back(0.0f);
							positions.push_back(0.0f);
							positions.push_back(0.0f);
						}

						// Texture coordinates
						if (!mesh.texcoords.empty() && idx * 2 + 1 < mesh.texcoords.size()) {
							texcoords.push_back(mesh.texcoords[idx * 2 + 0]);
							texcoords.push_back(mesh.texcoords[idx * 2 + 1]);
						} else {
							texcoords.push_back(0.0f);
							texcoords.push_back(0.0f);
						}

						// Normals
						if (!mesh.normals.empty() && idx * 3 + 2 < mesh.normals.size()) {
							normals.push_back(mesh.normals[idx * 3 + 0]);
							normals.push_back(mesh.normals[idx * 3 + 1]);
							normals.push_back(mesh.normals[idx * 3 + 2]);
						} else {
							normals.push_back(0.0f);
							normals.push_back(1.0f);
							normals.push_back(0.0f);
						}
					}
				}
				
				index_offset += fv;
			}
		}
	}
};
