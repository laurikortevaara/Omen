//
// Created by Lauri Kortevaara(Intopalo) on 02/01/16.
//

#ifndef OMEN_MD3LOADER_H
#define OMEN_MD3LOADER_H


#include <vector>
#include <string>
#include "Mesh.h"

namespace Omen {
    class MD3Loader {

        typedef unsigned char U8;
        typedef short S16;
        typedef int S32;
        typedef float F32;
        typedef glm::vec3 VEC3;

        size_t m_bufferSize;
        U8* m_buffer;
        U8* m_currentPos;

        /*
        -	MD3_START	Offset of MD3 object. Usually 0, but not guaranteed.
            S32	IDENT	Magic number. As a string of 4 octets, reads "IDP3"; as unsigned little-endian 860898377 (0x33504449); as unsigned big-endian 1229213747 (0x49445033).
            S32	VERSION	MD3 version number, latest known is 15, but use the constant MD3_VERSION.
            U8 * 64	NAME	MD3 name, usually its pathname in the PK3. ASCII character string, NULL-terminated (C-style).
            S32	FLAGS	 ???
            S32	NUM_FRAMES	Number of Frame objects, with a maximum of MD3_MAX_FRAMES. Current value of MD3_MAX_FRAMES is 1024.
            S32	NUM_TAGS	Number of Tag objects, with a maximum of MD3_MAX_TAGS. Current value of MD3_MAX_TAGS is 16.
                            There is one set of tags per frame so the total number of tags to read is (NUM_TAGS * NUM_FRAMES).
            S32	NUM_SURFACES	Number of Surface objects, with a maximum of MD3_MAX_SURFACES. Current value of MD3_MAX_SURFACES is 32.
            S32	NUM_SKINS	Number of Skin objects. I should note that I have not seen an MD3 using this particular field for anything;
                            this appears to be an artifact from the Quake 2 MD2 format. Surface objects have their own Shader field.
            S32	OFS_FRAMES	Relative offset from start of MD3 object where Frame objects start. The Frame objects are written sequentially,
                            that is, when you read one Frame object, you do not need to seek() for the next object.
            S32	OFS_TAGS	Relative offset from start of MD3 where Tag objects start. Similarly written sequentially.
            S32	OFS_SURFACES	Relative offset from start of MD3 where Surface objects start. Again, written sequentially.
            S32	OFS_EOF	Relative offset from start of MD3 to the end of the MD3 object. Note there is no offset for Skin objects.
            !	(Frame)	The array of Frame objects usually starts immediately afterwards, but OFS_FRAMES should be used.
            !	(Tag)	The array of Tag objects usually starts immediately after FRAMES, but OFS_TAGS should be used.
            !	(Surface)	The array of Surface objects usually start after TAGS, but OFS_SURFACES should be used.
        -	MD3_END	End of MD3 object. Should match OFS_EOF.
         */
        struct s_header {
            S32	indent;
            S32	version;
            U8  name[64];
            S32 flags;

            S32 num_frames;
            S32 num_tags;
            S32 num_surfaces;
            S32 num_skins;

            S32 ofs_frames;
            S32 ofs_tags;
            S32 ofs_surfaces;
            S32 ofs_eof;
        } *m_header;


        /**
         *  VEC3	MIN_BOUNDS	First corner of the bounding box.
            VEC3	MAX_BOUNDS	Second corner of the bounding box.
            VEC3	LOCAL_ORIGIN	Local origin, usually (0, 0, 0).
            F32	RADIUS	Radius of bounding sphere.
            U8 * 16	NAME	Name of Frame. ASCII character string, NUL-terminated (C-style)
         */
        struct s_frame {
            VEC3    min_bounds;
            VEC3    max_bounds;
            VEC3    local_origin;
            F32     radius;
            U8*     name;
        };
        s_frame*    m_frames;

        /*
         *  Data Type	Name	Description
            U8 * 64	NAME	Name of Tag object. ASCII character string, NUL-terminated (C-style).
            VEC3	ORIGIN	Coordinates of Tag object.
            VEC3 * 3	AXIS	3x3 rotation matrix associated with the Tag.
         */
        struct s_tag {
            U8      name[64];
            VEC3    origin;
            VEC3    axis[3];
        };

        /**
         * -	SURFACE_START	Offset relative to start of MD3 object.
                S32	IDENT	Magic number. As a string of 4 octets, reads "IDP3"; as unsigned little-endian 860898377 (0x33504449); as unsigned big-endian 1229213747 (0x49445033).
                U8 * 64	NAME	Name of Surface object. ASCII character string, NUL-terminated (C-style).
                S32	FLAGS	 ???
                S32	NUM_FRAMES	Number of animation frames. This should match NUM_FRAMES in the MD3 header.
                S32	NUM_SHADERS	Number of Shader objects defined in this Surface, with a limit of MD3_MAX_SHADERS. Current value of MD3_MAX_SHADERS is 256.
                S32	NUM_VERTS	Number of Vertex objects defined in this Surface, up to MD3_MAX_VERTS. Current value of MD3_MAX_VERTS is 4096.
                S32	NUM_TRIANGLES	Number of Triangle objects defined in this Surface, maximum of MD3_MAX_TRIANGLES. Current value of MD3_MAX_TRIANGLES is 8192.
                S32	OFS_TRIANGLES	Relative offset from SURFACE_START where the list of Triangle objects starts.
                S32	OFS_SHADERS	Relative offset from SURFACE_START where the list of Shader objects starts.
                S32	OFS_ST	Relative offset from SURFACE_START where the list of ST objects (s-t texture coordinates) starts.
                S32	OFS_XYZNORMAL	Relative offset from SURFACE_START where the list of Vertex objects (X-Y-Z-N vertices) starts.
                S32	OFS_END	Relative offset from SURFACE_START to where the Surface object ends.
                 !	(Shader)	List of Shader objects usually starts immediate after the Surface header, but use OFS_SHADERS (or rather, OFS_SHADERS+SURFACE_START for files).
                 !	(Triangle)	List of Triangle objects usually starts immediately after the list of Shader objects, but use OFS_TRIANGLES (+ SURFACE_START).
                 !	(ST)	List of ST objects usually starts immediately after the list of Triangle objects, but use OFS_ST (+ SURFACE_START).
                 !	(XYZNormal)	List of Vertex objects usually starts immediate after the list of St objects, but use
                                OFS_XYZNORMALS (+ SURFACE_START). The total number of objects is (NUM_FRAMES * NUM_VERTS).
                                One set of NUM_VERTS Vertex objects describes the Surface in one frame of animation;
                                the first NUM_VERTS Vertex objects describes the Surface in the first frame of animation,
                                the second NUM_VERTEX Vertex objects describes the Surface in the second frame of animation, and so forth.
                -	SURFACE_END	End of Surface object. Should match OFS_END.
         */
        struct s_surface {
            S32	ident;
            U8 	name[64];
            S32	flags;
            S32	num_frames;
            S32	num_shaders;
            S32	num_verts;
            S32	num_triangles;
            S32	ofs_triangles;
            S32	ofs_shaders;
            S32	ofs_st;
            S32	ofs_xyznormal;
            S32	ofs_end;
        };
        s_surface* m_surfaces;

        /**
         *  U8 * 64	NAME	Pathname of shader in the PK3. ASCII character string, NUL-terminated (C-style).
            S32	SHADER_INDEX	Shader index number. No idea how this is allocated, but presumably in sequential order of definition.
         */
        struct s_shader {
            U8  name[64];
            S32 shader_index;
        };

        /**
         * S32 * 3	INDEXES	List of offset values into the list of Vertex objects that constitute the corners of the Triangle object.
         * Vertex numbers are used instead of actual coordinates, as the coordinates are implicit in the Vertex object. The triangles have clockwise winding.

         */
        struct s_triangle {
            S32 m_indices[3];
        };


        /**
         * F32 * 2	ST	s and t texture coordinates, normalized to the range [0, 1]. Values outside the range indicate wraparounds/repeats.
         * Unlike UV coordinates, the origin for texture coordinates is located in the upper left corner (similar to the coordinate system used for computer screens) whereas,
         * in UV mapping, it is placed in the lower left corner. As such, the t value must be flipped to correspond with UV coordinates. See also Left-hand coordinates
         */
        typedef glm::vec2 s_texcoord;

        /**
         *  S16 * 3	COORD	x, y, and z coordinates in right-handed 3-space, scaled down by factor 1.0/64. (Multiply by 1.0/64 to obtain original coordinate value.)
            U8*2	NORMAL	Zenith and azimuth angles of normal vector. 255 corresponds to 2 pi. See spherical coordinates.
         */
        struct s_vertex {
            S16 coord[3];
            U8  normal[2];
        };
    public:

        struct _index_elem {
            int vertex_index, texture_index, normal_index;
        };

        class face {
        public:
            std::vector<_index_elem> indices;
        };

        class frame {
        public:
            std::vector<glm::vec3> m_vertices;
            std::vector<glm::vec3> m_normals;
        };

        class mesh {
        public:
            std::string material;
            std::string name;
            std::vector<face> faces;
            std::vector<frame> frames;
        };

    public:

        //std::vector<glm::vec3> m_vertices;
        std::vector<glm::vec3> m_parameter_coords;
        //std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_texcoords;

    public:
        std::vector<mesh *> m_meshes;
        std::string currentMaterialName = "";

        bool loadModel(const std::string &filepath);

    public:


        bool readHeader();
        void readSurfaces();

        void getMesh(std::vector<std::unique_ptr<Omen::Mesh>>& meshes);

        glm::vec3 getRealVertex(S16 vertices[3]);
        glm::vec3 fromSpherical(U8 spherical_coord[2]);

        void readFrames();
    };
} // namespace Omen
#endif //OMEN_MD3LOADER_H
