//
// Created by Lauri Kortevaara(Intopalo) on 29/12/15.
//

#ifndef OMEN_WAVEFRONTLOADER_H
#define OMEN_WAVEFRONTLOADER_H


#include <iosfwd>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif __APPLE__
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>
#else
//#include <GL/glew.h>
#include <glfw/glfw3.h>
#endif
#include "Material.h"
#include "Mesh.h"

namespace omen {
	class WavefrontLoader {
	public:
		/**
	 * Wavefront file material definition, based on https://en.wikipedia.org/wiki/Wavefront_.obj_file
	 */
		struct _material {
			std::string name;

			float Ns; // Specular exponent
			glm::vec3 Ka; // Ambient color
			glm::vec3 Kd; // Diffuse color
			glm::vec3 Ks; // Specular color
			float Ni; // Refraction index
			float d;  // dissolved == amount of opacity, 0 == transparent, 1 == opaque
			int illum; // Illumination models, see below

			std::string map_Ka; // Ambient texture map
			std::string map_Kd; // Diffuse texture map
			std::string map_Ks; // Specular texture map
			std::string map_NS; // Specular highlight map
			std::string map_d;  // Alpha/transparency texture map
			std::string map_bump; // Bump texture map
			std::string map_disp; // Displacement map
			std::string map_stencil; // Stencil decal map

			// WIP TODO, implement the below exception
			//map_Ka -o 1 1 1 ambient.tga            # texture origin (1,1,1)
			//refl -type sphere clouds.tga           # spherical reflection map
			//

			bool operator==(const _material &m) { return name == m.name; }
		};

		/** Illumination models for illum parameter in material:
		 * 0. Color on and Ambient off
		1. Color on and Ambient on
		2. Highlight on
		3. Reflection on and Ray trace on
		4. Transparency: Glass on, Reflection: Ray trace on
		5. Reflection: Fresnel on and Ray trace on
		6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
		7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
		8. Reflection on and Ray trace off
		9. Transparency: Glass on, Reflection: Ray trace off
		10. Casts shadows onto invisible surfaces
		 */
		struct _index_elem {
			int vertex_index, texture_index, normal_index;
		};

		class face {
		public:
			std::vector<_index_elem> indices;
		};

		class mesh {
		public:
			std::string material;
			std::string name;
			std::vector<face> faces;

			std::unique_ptr<omen::Mesh> getMesh();
		};

	public:

		static std::vector<glm::vec3> m_vertices;
		static std::vector<glm::vec3> m_parameter_coords;
		static std::vector<glm::vec3> m_normals;
		static std::vector<glm::vec2> m_texcoords;
		static std::vector<_material*> materials;

	public:
		std::vector<mesh*> meshes;
		std::string currentMaterialName = "";


	public:
		bool loadObj(const std::string &filename);

		bool loadMaterialLibrary(const std::string &material);


	};
}

#endif //OMEN_WAVEFRONTLOADER_H
