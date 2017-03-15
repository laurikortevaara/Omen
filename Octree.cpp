#define NOMINMAX
#include "Octree.h"
#include <algorithm>
#include "Mesh.h"
#include <vector>

using namespace omen;


/* Thomas Akenine-Möller TRI-AABB Intersection*/
#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
dest[0] = v1[1] * v2[2] - v1[2] * v2[1]; \
dest[1] = v1[2] * v2[0] - v1[0] * v2[2]; \
dest[2] = v1[0] * v2[1] - v1[1] * v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
dest[0] = v1[0] - v2[0]; \
dest[1] = v1[1] - v2[1]; \
dest[2] = v1[2] - v2[2];

#define FINDMINMAX(x0,x1,x2,min,max) \
min = max = x0;   \
if (x1<min) min = x1; \
	if (x1>max) max = x1; \
		if (x2<min) min = x2; \
			if (x2>max) max = x2;

int planeBoxOverlap(float normal[3], float vert[3], std::vector<float> maxbox)	// -NJMP-
{
	int q;
	float vmin[3], vmax[3], v;
	for (q = X; q <= Z; q++)
	{
		v = vert[q];					// -NJMP-
		if (normal[q]>0.0f)
		{
			vmin[q] = -maxbox[q] - v;	// -NJMP-
			vmax[q] = maxbox[q] - v;	// -NJMP-
		}
		else
		{
			vmin[q] = maxbox[q] - v;	// -NJMP-
			vmax[q] = -maxbox[q] - v;	// -NJMP-
		}
	}
	if (DOT(normal, vmin)>0.0f) return 0;	// -NJMP-
	if (DOT(normal, vmax) >= 0.0f) return 1;	// -NJMP-

	return 0;
}


/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
p0 = a*v0[Y] - b*v0[Z];			       	   \
p2 = a*v2[Y] - b*v2[Z];			       	   \
if (p0<p2) { min = p0; max = p2; } \
else { min = p2; max = p0; } \
rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
if (min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
p0 = a*v0[Y] - b*v0[Z];			           \
p1 = a*v1[Y] - b*v1[Z];			       	   \
if (p0<p1) { min = p0; max = p1; } \
else { min = p1; max = p0; } \
rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
if (min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
p0 = -a*v0[X] + b*v0[Z];		      	   \
p2 = -a*v2[X] + b*v2[Z];	       	       	   \
if (p0<p2) { min = p0; max = p2; } \
else { min = p2; max = p0; } \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
if (min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
p0 = -a*v0[X] + b*v0[Z];		      	   \
p1 = -a*v1[X] + b*v1[Z];	     	       	   \
if (p0<p1) { min = p0; max = p1; } \
else { min = p1; max = p0; } \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
if (min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
p1 = a*v1[X] - b*v1[Y];			           \
p2 = a*v2[X] - b*v2[Y];			       	   \
if (p2<p1) { min = p2; max = p1; } \
else { min = p1; max = p2; } \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
if (min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
p0 = a*v0[X] - b*v0[Y];				   \
p1 = a*v1[X] - b*v1[Y];			           \
if (p0<p1) { min = p0; max = p1; } \
else { min = p1; max = p0; } \
rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
if (min>rad || max<-rad) return 0;

int triBoxOverlap(std::vector<float> boxcenter, std::vector<float> boxhalfsize, std::vector<omen::Mesh::Vertex> vertices)
{

	/*    use separating axis theorem to test overlap between triangle and box */
	/*    need to test for overlap in these directions: */
	/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
	/*       we do not even need to test these) */
	/*    2) normal of the triangle */
	/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
	/*       this gives 3x3=9 more tests */
	float v0[3], v1[3], v2[3];
	//   float axis[3];
	float min, max, p0, p1, p2, rad, fex, fey, fez;		// -NJMP- "d" local variable removed
	float normal[3], e0[3], e1[3], e2[3];

	float triverts[3][3] = { { vertices[0].pos.x, vertices[0].pos.y, vertices[0].pos.z },{ vertices[1].pos.x, vertices[1].pos.y, vertices[1].pos.z },{ vertices[2].pos.x, vertices[2].pos.y, vertices[2].pos.z } };

	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */
	SUB(v0, triverts[0], boxcenter);
	SUB(v1, triverts[1], boxcenter);
	SUB(v2, triverts[2], boxcenter);

	/* compute triangle edges */
	SUB(e0, v1, v0);      /* tri edge 0 */
	SUB(e1, v2, v1);      /* tri edge 1 */
	SUB(e2, v0, v2);      /* tri edge 2 */

						  /* Bullet 3:  */
						  /*  test the 9 tests first (this was faster) */
	fex = fabsf(e0[X]);
	fey = fabsf(e0[Y]);
	fez = fabsf(e0[Z]);
	AXISTEST_X01(e0[Z], e0[Y], fez, fey);
	AXISTEST_Y02(e0[Z], e0[X], fez, fex);
	AXISTEST_Z12(e0[Y], e0[X], fey, fex);

	fex = fabsf(e1[X]);
	fey = fabsf(e1[Y]);
	fez = fabsf(e1[Z]);
	AXISTEST_X01(e1[Z], e1[Y], fez, fey);
	AXISTEST_Y02(e1[Z], e1[X], fez, fex);
	AXISTEST_Z0(e1[Y], e1[X], fey, fex);

	fex = fabsf(e2[X]);
	fey = fabsf(e2[Y]);
	fez = fabsf(e2[Z]);
	AXISTEST_X2(e2[Z], e2[Y], fez, fey);
	AXISTEST_Y1(e2[Z], e2[X], fez, fex);
	AXISTEST_Z12(e2[Y], e2[X], fey, fex);

	/* Bullet 1: */
	/*  first test overlap in the {x,y,z}-directions */
	/*  find min, max of the triangle each direction, and test for overlap in */
	/*  that direction -- this is equivalent to testing a minimal AABB around */
	/*  the triangle against the AABB */

	/* test in X-direction */
	FINDMINMAX(v0[X], v1[X], v2[X], min, max);
	if (min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

	/* test in Y-direction */
	FINDMINMAX(v0[Y], v1[Y], v2[Y], min, max);
	if (min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

	/* test in Z-direction */
	FINDMINMAX(v0[Z], v1[Z], v2[Z], min, max);
	if (min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

	/* Bullet 2: */
	/*  test if the box intersects the plane of the triangle */
	/*  compute plane equation of triangle: normal*x+d=0 */
	CROSS(normal, e0, e1);
	// -NJMP- (line removed here)
	if (!planeBoxOverlap(normal, v0, boxhalfsize)) return 0;	// -NJMP-

	return 1;   /* box and triangle overlaps */
}
/*----*/
Octree::Octree(Mesh* mesh) : rootNode(nullptr) {
	rootNode = nullptr;
	buildTree(mesh);
}

void Octree::buildTree(Mesh* mesh) {
	
	/*for (size_t i = 0; i < mesh->vertexIndices().size()-3; i += 3) {
		omen::Mesh::Vertex v1 = mesh->vertices()[i];
		omen::Mesh::Vertex v2 = mesh->vertices()[i+1];
		omen::Mesh::Vertex v3 = mesh->vertices()[i+2];

		/*for (float x = -10.0f; x < 10.f; x += 0.2)
			for (float y = -10.0f; y < 10.f; y += 0.2)
				for (float z = -10.0f; z < 10.f; z += 0.2) {
					if(triBoxOverlap({ x,y,z }, { 0.2f,0.2f,0.2f }, { v1,v2,v3 }))
						addNodeTo({ x,y,z });
				}*
		addNodeTo({ 0,0,0 });
		addNodeTo({ 1,1,1 });
	}*/

	addNodeTo({ 0,0,0 });
}

Octree::Node::Node() : 
	parent(nullptr), 
	children{ nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr }, 
	size(0.0f), 
	pos(0) 
{
}
			
/* Is this node inside of given node*/
bool Octree::Node::isInside(Node* n) {
	return
		(n->pos.x + n->size >= pos.x + size && n->pos.x - n->size >= pos.x - size) &&
		(n->pos.y + n->size >= pos.y + size && n->pos.y - n->size >= pos.y - size) &&
		(n->pos.z + n->size >= pos.z + size && n->pos.z - n->size >= pos.z - size);
}

bool Octree::Node::addChild(Node* node) {
	float sx = node->pos.x <= pos.x ? 0.0f : 1.0f;// sign_x
	float sy = node->pos.y <= pos.y ? 0.0f : 1.0f;// sign_y
	float sz = node->pos.z <= pos.z ? 0.0f : 1.0f;// sign_z
	int nodeIndex = static_cast<int>(sx * 4 + sy * 2 + sz);
	if (node->size < size / 2) {
		Node* newParent = children[nodeIndex];
		if (children[nodeIndex] == nullptr) {
			newParent = new Node;
			newParent->size = size / 2;
			children[nodeIndex] = newParent;
		}
		newParent->addChild(node);
	}
	else {
		if (children[nodeIndex] == nullptr) {
			children[nodeIndex] = node;
			return true;
		}
		else
			return false;
	}
	return false;
}

	

Octree::Node* Octree::findNode(const float& x, const float& y, const float& z, Node* node) {
	Node* n = node ? node : rootNode;
	if (n) {
		if ((n->pos.x - n->size <= x&&n->pos.x + n->size >= x) &&
			(n->pos.y - n->size <= y&&n->pos.y + n->size >= y) &&
			(n->pos.z - n->size <= z&&n->pos.z + n->size >= z))
		{
			for (auto& child : n->children) {
				Node* matching_child = findNode(x, y, z, child);
				if (matching_child)
					return matching_child;
			}

		}

	}
	return n;
}

Octree::Node* Octree::getParentNode(Octree::Node* n, Node* parentNodeCandidate) {
	Node* parentNode = parentNodeCandidate ? parentNodeCandidate : rootNode;
	if (parentNode == nullptr) {
		return n;
	}

	if (n->isInside(parentNode))
		return parentNode;

	float minx = std::min(parentNode->pos.x - parentNode->size / 2, n->pos.x - n->size / 2);
	float miny = std::min(parentNode->pos.y - parentNode->size / 2, n->pos.y - n->size / 2);
	float minz = std::min(parentNode->pos.z - parentNode->size / 2, n->pos.z - n->size / 2);

	float maxx = std::min(parentNode->pos.x + parentNode->size / 2, n->pos.x + n->size / 2);
	float maxy = std::min(parentNode->pos.y + parentNode->size / 2, n->pos.y + n->size / 2);
	float maxz = std::min(parentNode->pos.z + parentNode->size / 2, n->pos.z + n->size / 2);

	glm::vec3 d = glm::vec3(maxx, maxy, maxz) - glm::vec3(minx, miny, minz);
	float s = glm::length(d);

	parentNodeCandidate = new Node;
	parentNodeCandidate->pos = glm::vec3(minx, miny, minz) + d*s;
	parentNodeCandidate->size = s;

	parentNodeCandidate->addChild(parentNode);
	parentNodeCandidate->addChild(n);

	rootNode = parentNodeCandidate;
	return parentNodeCandidate;
}

Octree::Node* Octree::addNodeTo(const float& x, const float& y, const float& z) {
	Node* node = nullptr; // findNode(x, y, z);
	if (node == nullptr) {
		node = new Node;
		node->size = minSize;
		float half_size = node->size / 2.0f;
		node->pos.x = fmod(x, minSize) + half_size;
		node->pos.y = fmod(y, minSize) + half_size;
		node->pos.z = fmod(z, minSize) + half_size;
	}
	Node* parentNode = getParentNode(node);
	if (rootNode == nullptr)
		rootNode = parentNode;
	else {
		rootNode->addChild(node);
	}
	return node;
}

Octree::Node* Octree::addNodeTo(const glm::vec3& pos) {
	return addNodeTo(pos.x, pos.y, pos.z);
}


