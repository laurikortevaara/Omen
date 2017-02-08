#include "MeshProvider.h"
#include "MathUtils.h"
#include "Entity.h"
#include "GameObject.h"
#include "component/MeshController.h"
#include "component/MeshRenderer.h"

#include <fbxsdk.h>
#include <fbxsdk/core/fbxpropertytypes.h>
#include "Common/Common.h"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <list>

#define gVerbose true

using namespace omen;

MeshProvider::MeshProvider()
{
}


MeshProvider::~MeshProvider()
{
}

template<typename T, int sz>
int size(T(&)[sz])
{
	return sz;
}

//get mesh smoothing info
//set pCompute true to compute smoothing from normals by default 
//set pConvertToSmoothingGroup true to convert hard/soft edge info to smoothing group info by default
void GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode, bool pCompute, bool pConvertToSmoothingGroup)
{
	if (!pNode || !pSdkManager)
		return;

	//get mesh
	FbxMesh* lMesh = pNode->GetMesh();

	if (lMesh)
	{
		//print mesh node name
		FBXSDK_printf("current mesh node: %s\n", pNode->GetName());

		//if there's no smoothing info in fbx file, but you still want to get smoothing info.
		//please compute smoothing info from normals.
		//Another case to recompute smoothing info from normals is:
		//If users edit normals manually in 3ds Max or Maya and export the scene to FBX with smoothing info,
		//The smoothing info may NOT match with normals.
		//the mesh called "fbx_customNormals" in Normals.fbx is the case. All edges are hard, but normals actually represent the "soft" looking.
		//Generally, the normals in fbx file holds the smoothing result you'd like to get.
		//If you want to get correct smoothing info(smoothing group or hard/soft edges) which match with normals,
		//please drop the original smoothing info of fbx file, and recompute smoothing info from normals.
		//if you want to get soft/hard edge info, please call FbxGeometryConverter::ComputeEdgeSmoothingFromNormals().
		//if you want to get smoothing group info, please get soft/hard edge info first by ComputeEdgeSmoothingFromNormals() 
		//And then call FbxGeometryConverter::ComputePolygonSmoothingFromEdgeSmoothing().
		if (pCompute)
		{
			FbxGeometryConverter lGeometryConverter(pSdkManager);
			lGeometryConverter.ComputeEdgeSmoothingFromNormals(lMesh);
			//convert soft/hard edge info to smoothing group info
			if (pConvertToSmoothingGroup)
				lGeometryConverter.ComputePolygonSmoothingFromEdgeSmoothing(lMesh);
		}

		//if there is smoothing groups info in your fbx file, but you want to get hard/soft edges info
		//please use following code:
		//FbxGeometryConverter lGeometryConverter(lSdkManager);
		//lGeometryConverter.ComputeEdgeSmoothingFromPolygonSmoothing(lMesh);

		//get smoothing info
		FbxGeometryElementSmoothing* lSmoothingElement = lMesh->GetElementSmoothing();
		if (lSmoothingElement)
		{
			//mapping mode is by edge. The mesh usually come from Maya, because Maya can set hard/soft edges.
			//we can get smoothing info(which edges are soft, which edges are hard) by retrieving each edge. 
			if (lSmoothingElement->GetMappingMode() == FbxGeometryElement::eByEdge)
			{
				//Let's get smoothing of each edge, since the mapping mode of smoothing element is by edge
				for (int lEdgeIndex = 0; lEdgeIndex < lMesh->GetMeshEdgeCount(); lEdgeIndex++)
				{
					int lSmoothingIndex = 0;
					//reference mode is direct, the smoothing index is same as edge index.
					//get smoothing by the index of edge
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lSmoothingIndex = lEdgeIndex;

					//reference mode is index-to-direct, get smoothing by the index-to-direct
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lSmoothingIndex = lSmoothingElement->GetIndexArray().GetAt(lEdgeIndex);

					//Got smoothing of each vertex.
					int lSmoothingFlag = lSmoothingElement->GetDirectArray().GetAt(lSmoothingIndex);
					if (gVerbose) FBXSDK_printf("hard/soft value for edge[%d]: %d \n", lEdgeIndex, lSmoothingFlag);
					//add your custom code here, to output smoothing or get them into a list, such as KArrayTemplate<int>
					//. . .
				}//end for lEdgeIndex
			}//end eByEdge
			 //mapping mode is by polygon. The mesh usually come from 3ds Max, because 3ds Max can set smoothing groups for polygon.
			 //we can get smoothing info(smoothing group ID for each polygon) by retrieving each polygon. 
			else if (lSmoothingElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
			{
				//Let's get smoothing of each polygon, since the mapping mode of smoothing element is by polygon.
				for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
				{
					int lSmoothingIndex = 0;
					//reference mode is direct, the smoothing index is same as polygon index.
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lSmoothingIndex = lPolygonIndex;

					//reference mode is index-to-direct, get smoothing by the index-to-direct
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lSmoothingIndex = lSmoothingElement->GetIndexArray().GetAt(lPolygonIndex);

					//Got smoothing of each polygon.
					int lSmoothingFlag = lSmoothingElement->GetDirectArray().GetAt(lSmoothingIndex);
					if (gVerbose) FBXSDK_printf("smoothing group ID for polygon[%d]: %d \n", lPolygonIndex, lSmoothingFlag);
					//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<int>
					//. . .

				}//end for lPolygonIndex //PolygonCount

			}//end eByPolygonVertex
		}//end if lSmoothingElement
	}//end if lMesh

	 //recursively traverse each node in the scene
	/*int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		GetSmoothing(pSdkManager, pNode->GetChild(i), pCompute, pConvertToSmoothingGroup);
	}*/
}

void LoadUVInformation(FbxMesh* pMesh)
{
    //get all UV set names
    FbxStringList lUVSetNameList;
    pMesh->GetUVSetNames(lUVSetNameList);

    //iterating over all uv sets
    for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
    {
        //get lUVSetIndex-th uv set
        const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
        const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

        if(!lUVElement)
            continue;

        // only support mapping mode eByPolygonVertex and eByControlPoint
        if( lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
            lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint )
            return;

        //index array, where holds the index referenced to the uv data
        const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
        const int lIndexCount= (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

        //iterating through the data by polygon
        const int lPolyCount = pMesh->GetPolygonCount();

        if( lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint )
        {
            for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
            {
                // build the max index array that we need to pass into MakePoly
                const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
                for( int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex )
                {
                    FbxVector2 lUVValue;

                    //get the index of the current vertex in control points array
                    int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex,lVertIndex);

                    //the UV index depends on the reference mode
                    int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

                    lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

                    //User TODO:
                    //Print out the value of UV(lUVValue) or log it to a file
                }
            }
        }
        else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        {
            int lPolyIndexCounter = 0;
            for( int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex )
            {
                // build the max index array that we need to pass into MakePoly
                const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
                for( int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex )
                {
                    if (lPolyIndexCounter < lIndexCount)
                    {
                        FbxVector2 lUVValue;

                        //the UV index depends on the reference mode
                        int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

                        lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

                        //User TODO:
                        //Print out the value of UV(lUVValue) or log it to a file

                        lPolyIndexCounter++;
                    }
                }
            }
        }
    }
}

std::unique_ptr<Mesh> MeshProvider::createSphere()
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	// Load the scene.
	// The example can take a FBX file as an argument.
	FbxString lFilePath("models/banana.fbx");
	lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	
	///
	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(lSdkManager);



	lGeomConverter.Triangulate(lScene, true, true);//node //<<------------------------------------------timing more..

	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	lGeomConverter.SplitMeshesPerMaterial(lScene, /*replace*/true);
	///

	//get root node of the fbx scene
	FbxNode* lRootNode = lScene->GetRootNode();

	std::vector<Mesh::Vertex> vertices = {};
	std::vector<glm::vec3> normals = {};
	std::vector<glm::vec2> uv = {};
	std::vector<int> indices = {};
	std::vector<glm::vec3> tangents = {};
	std::vector<glm::vec3> bitangents = {};

	std::cout << "RootNode: " << lRootNode->GetName() << std::endl;
	for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
		FbxNode* pNode = lRootNode->GetChild(i);
		std::cout << "Child: " << lRootNode->GetChild(i)->GetName() << std::endl;
		FbxMesh* lMesh = pNode->GetMesh();
				
		if (lMesh) {
			/// UV
			//get all UV set names
			FbxStringList lUVSetNameList;
			lMesh->GetUVSetNames(lUVSetNameList);
			const FbxGeometryElementUV* lUVElement = nullptr;
			//iterating over all uv sets
			for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
			{
				//get lUVSetIndex-th uv set
				const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
				lUVElement = lMesh->GetElementUV(lUVSetName);
			}
			/*UVs*/
			//iterating through the data by polygon
			//index array, where holds the index referenced to the uv data
			const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
			const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
			const int lPolyCount = lMesh->GetPolygonCount();
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = lMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
						uv.push_back({ lUVValue[0], lUVValue[1] });
						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file

						lPolyIndexCounter++;
					}
				}
			}
			/*UVs*/

			for (int ci = 0; ci < lMesh->GetControlPointsCount(); ci++) {
				fbxsdk::FbxVector4* vp = lMesh->GetControlPoints();
				fbxsdk::FbxVector4 v = vp[ci];
				vertices.push_back(glm::vec3(v[0], v[1], v[2]));
				Mesh::Vertex& mv = vertices.back();
			}
			if (vertices.empty())
				continue;

			//GetNormals(pNode, normals);
			int polygons = lMesh->GetPolygonCount();
			int vertices = lMesh->GetPolygonVertexCount();
			for (int pi = 0; pi < polygons; pi++) {
				int polySize = lMesh->GetPolygonSize(pi);
				for (int pii = 0; pii < polySize; ++pii){
					int v = lMesh->GetPolygonVertex(pi, pii);
					indices.push_back(v);
				}
			}
		}
	}
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

	/*normals.clear();
	for (int i = 0; i < vertices.size(); i += 3) {
		glm::vec3& v1 = vertices[i];
		glm::vec3& v2 = vertices[(i + 1) % (vertices.size())];
		glm::vec3& v3 = vertices[(i + 2) % (vertices.size())];
		glm::vec3 n = glm::normalize(glm::cross((v2 - v1), (v3 - v1)));
		normals.push_back(n);
		normals.push_back(n);
		normals.push_back(n);
	}*/

	std::vector<Mesh::Vertex> actualVertices;
	std::vector<glm::vec3> actualNormals;
	actualNormals.reserve(vertices.size());
	for (int i = 0; i < indices.size(); i+=3 ) {
		int vi1 = indices[i];
		int vi2 = indices[i + 1];
		int vi3 = indices[i + 2];
		Mesh::Vertex& v1 = vertices[vi1];
		Mesh::Vertex& v2 = vertices[vi2];
		Mesh::Vertex& v3 = vertices[vi3];
		glm::vec3 n = glm::normalize(glm::cross((v2.pos - v1.pos), (v3.pos - v1.pos)));
		actualVertices.push_back(v1);
		actualVertices.push_back(v2);
		actualVertices.push_back(v3);
		v1.normal = (v1.normal+n)*0.5f;
		v2.normal = (v2.normal+n)*0.5f;
		v3.normal = (v3.normal+n)*0.5f;
		/*v1.normal = (n);
		v2.normal = (n);
		v3.normal = (n);*/
	}

	for (int i = 0; i < indices.size(); i += 3) {
		int vi1 = indices[i];
		int vi2 = indices[i + 1];
		int vi3 = indices[i + 2];
		Mesh::Vertex& v1 = vertices[vi1];
		Mesh::Vertex& v2 = vertices[vi2];
		Mesh::Vertex& v3 = vertices[vi3];
		normals.push_back(v1.normal);
		normals.push_back(v2.normal);
		normals.push_back(v3.normal);
		//uv.push_back(v1.uv);
		//uv.push_back(v2.uv);
		//uv.push_back(v3.uv);
	}

	// Calc tangents
	for (int i = 0; i < vertices.size()-3; i += 3) {

		// Shortcuts for vertices
		glm::vec3 & v0 = vertices.empty() ? glm::vec3(0.0f) : vertices[i + 0];
		glm::vec3 & v1 = vertices.empty() ? glm::vec3(0.0f) : vertices[i + 1];
		glm::vec3 & v2 = vertices.empty() ? glm::vec3(0.0f) : vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2 & uv0 = uv.empty() ? glm::vec2(0.0f) : uv[i + 0];
		glm::vec2 & uv1 = uv.empty() ? glm::vec2(0.0f) : uv[i + 1];
		glm::vec2 & uv2 = uv.empty() ? glm::vec2(0.0f) : uv[i + 2];

		// Edges of the triangle : postion delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		// Set the same tangent for all three vertices of the triangle.
		// They will be merged later, in vboindexer.cpp
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		// Same thing for binormals
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);

	}
	// Smoothen normals;
	//normals.clear();
	//for (auto v : vertices)
//		normals.push_back(v.normal);
	mesh->setVertices(actualVertices);
	mesh->setNormals(normals);
	mesh->setUVs(uv);
	//mesh->setVertexIndices(indices);
	return std::move(mesh);





	//get normals info, if there're mesh in the scene
	//GetNormals(lRootNode);

	//Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

	return nullptr;
	////////
	float radius = 10.0f;
	unsigned int rings = 10;
	unsigned int sectors = 10;
	{
		std::vector<GLfloat> vertices;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> texcoords;
		std::vector<GLushort> indices;

		float const R = 1. / (float)(rings - 1);
		float const S = 1. / (float)(sectors - 1);
		int r, s;
		
		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);
		std::vector<GLfloat>::iterator v = vertices.begin();
		std::vector<GLfloat>::iterator n = normals.begin();
		std::vector<GLfloat>::iterator t = texcoords.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*t++ = s*S;
			*t++ = r*R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
		}

		indices.resize(rings * sectors * 4);
		std::vector<GLushort>::iterator i = indices.begin();
		for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}

		
	}
	return nullptr;
}

//get mesh smoothing info
//set pCompute true to compute smoothing from normals by default 
//set pConvertToSmoothingGroup true to convert hard/soft edge info to smoothing group info by default
void GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode, bool pCompute, bool pConvertToSmoothingGroup, std::vector<glm::vec3>& normals)
{
	if (!pNode || !pSdkManager)
		return;

	//get mesh
	FbxMesh* lMesh = pNode->GetMesh();
	if (lMesh)
	{
		//print mesh node name
		FBXSDK_printf("current mesh node: %s\n", pNode->GetName());

		//if there's no smoothing info in fbx file, but you still want to get smoothing info.
		//please compute smoothing info from normals.
		//Another case to recompute smoothing info from normals is:
		//If users edit normals manually in 3ds Max or Maya and export the scene to FBX with smoothing info,
		//The smoothing info may NOT match with normals.
		//the mesh called "fbx_customNormals" in Normals.fbx is the case. All edges are hard, but normals actually represent the "soft" looking.
		//Generally, the normals in fbx file holds the smoothing result you'd like to get.
		//If you want to get correct smoothing info(smoothing group or hard/soft edges) which match with normals,
		//please drop the original smoothing info of fbx file, and recompute smoothing info from normals.
		//if you want to get soft/hard edge info, please call FbxGeometryConverter::ComputeEdgeSmoothingFromNormals().
		//if you want to get smoothing group info, please get soft/hard edge info first by ComputeEdgeSmoothingFromNormals() 
		//And then call FbxGeometryConverter::ComputePolygonSmoothingFromEdgeSmoothing().
		if (pCompute)
		{
			FbxGeometryConverter lGeometryConverter(pSdkManager);
			lGeometryConverter.ComputeEdgeSmoothingFromNormals(lMesh);
			//convert soft/hard edge info to smoothing group info
			if (pConvertToSmoothingGroup)
				lGeometryConverter.ComputePolygonSmoothingFromEdgeSmoothing(lMesh);
		}

		//if there is smoothing groups info in your fbx file, but you want to get hard/soft edges info
		//please use following code:
		//FbxGeometryConverter lGeometryConverter(lSdkManager);
		//lGeometryConverter.ComputeEdgeSmoothingFromPolygonSmoothing(lMesh);

		//get smoothing info
		FbxGeometryElementSmoothing* lSmoothingElement = lMesh->GetElementSmoothing();
		if (lSmoothingElement)
		{
			//mapping mode is by edge. The mesh usually come from Maya, because Maya can set hard/soft edges.
			//we can get smoothing info(which edges are soft, which edges are hard) by retrieving each edge. 
			if (lSmoothingElement->GetMappingMode() == FbxGeometryElement::eByEdge)
			{
				//Let's get smoothing of each edge, since the mapping mode of smoothing element is by edge
				for (int lEdgeIndex = 0; lEdgeIndex < lMesh->GetMeshEdgeCount(); lEdgeIndex++)
				{
					int lSmoothingIndex = 0;
					//reference mode is direct, the smoothing index is same as edge index.
					//get smoothing by the index of edge
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lSmoothingIndex = lEdgeIndex;

					//reference mode is index-to-direct, get smoothing by the index-to-direct
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lSmoothingIndex = lSmoothingElement->GetIndexArray().GetAt(lEdgeIndex);

					//Got smoothing of each vertex.
					int lSmoothingFlag = lSmoothingElement->GetDirectArray().GetAt(lSmoothingIndex);
					if (gVerbose) FBXSDK_printf("hard/soft value for edge[%d]: %d \n", lEdgeIndex, lSmoothingFlag);
					//add your custom code here, to output smoothing or get them into a list, such as KArrayTemplate<int>
					//. . .
				}//end for lEdgeIndex
			}//end eByEdge
			 //mapping mode is by polygon. The mesh usually come from 3ds Max, because 3ds Max can set smoothing groups for polygon.
			 //we can get smoothing info(smoothing group ID for each polygon) by retrieving each polygon. 
			else if (lSmoothingElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
			{
				//Let's get smoothing of each polygon, since the mapping mode of smoothing element is by polygon.
				for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
				{
					int lSmoothingIndex = 0;
					//reference mode is direct, the smoothing index is same as polygon index.
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lSmoothingIndex = lPolygonIndex;

					//reference mode is index-to-direct, get smoothing by the index-to-direct
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lSmoothingIndex = lSmoothingElement->GetIndexArray().GetAt(lPolygonIndex);

					//Got smoothing of each polygon.
					int lSmoothingFlag = lSmoothingElement->GetDirectArray().GetAt(lSmoothingIndex);
					if (gVerbose) FBXSDK_printf("smoothing group ID for polygon[%d]: %d \n", lPolygonIndex, lSmoothingFlag);
					//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<int>
					//. . .
					normals.push_back({});
				}//end for lPolygonIndex //PolygonCount

			}//end eByPolygonVertex
		}//end if lSmoothingElement
	}//end if lMesh

	 //recursively traverse each node in the scene
	int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		GetSmoothing(pSdkManager, pNode->GetChild(i), pCompute, pConvertToSmoothingGroup);
	}
}

void GetVertexIndices(FbxNode* pNode, std::vector<GLsizei>& indices, std::vector<Mesh::Vertex>& vertices, std::vector<glm::vec3>& normals)
{
	FbxMesh* pMesh = pNode->GetMesh();
	if (!pMesh)
		return;
	const char * name = pMesh->GetName();
	const char * nodename = pNode->GetName();
	FbxGeometryElementNormal* lNormalElement = pMesh->GetElementNormal();
	fbxsdk::FbxVector4* vp = pMesh->GetControlPoints();
	for (int i = 0; i < pMesh->GetPolygonVertexCount(); ++i) {
		vertices.push_back(glm::vec3({ 0 }));
		normals.push_back(glm::vec3({ 0 }));
	}

	for (GLsizei pi = 0; pi < pMesh->GetPolygonCount(); ++pi)
	{
		GLsizei ps = pMesh->GetPolygonSize(pi);
		for (GLsizei pii = 0; pii < ps; ++pii)
		{
			indices.push_back(pMesh->GetPolygonVertex(pi, pii));
			FbxVector4 v = vp[indices.back()];
			vertices[indices.back()] = glm::vec3({ v[0],v[2],v[1] });
			FbxVector4 n;
			pMesh->GetPolygonVertexNormal(pi, pii, n);
			//Got normals of each polygon-vertex.
			normals[indices.back()] = glm::vec3({ n[0], n[2] , n[1] });
		}
	}

#ifdef INVERT_NORMALS
	for (int i = 0; i <= indices.size()-3; i+=3)
	{
		GLsizei v1 = indices[i];
		indices[i] = indices[i + 2];
		indices[i + 2] = v1;
	}
#endif
}

void GetVertices(FbxNode* pNode, std::vector<Mesh::Vertex>& vertices)
{
	FbxMesh* lMesh = pNode->GetMesh();
	for (int ci = 0; ci < lMesh->GetControlPointsCount(); ci++) {
		fbxsdk::FbxVector4* vp = lMesh->GetControlPoints();
		fbxsdk::FbxVector4 v = vp[ci];
		vertices.push_back(glm::vec3(v[0], v[2], v[1]));
		Mesh::Vertex& mv = vertices.back();
	}
}

//get mesh normals info
void GetNormals(FbxNode* pNode, std::vector<glm::vec3>& normals)
{
	if (!pNode)
		return;

	//get mesh
	FbxMesh* lMesh = pNode->GetMesh();
	if (lMesh)
	{
		//print mesh node name
		FBXSDK_printf("current mesh node: %s\n", pNode->GetName());

		//get the normal element
		FbxGeometryElementNormal* lNormalElement = lMesh->GetElementNormal();
		if (lNormalElement)
		{
			//mapping mode is by control points. The mesh should be smooth and soft.
			//we can get normals by retrieving each control point
			if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				//Let's get normals of each vertex, since the mapping mode of normal element is by control point
				for (int lVertexIndex = 0; lVertexIndex < lMesh->GetControlPointsCount(); lVertexIndex++)
				{
					int lNormalIndex = 0;
					//reference mode is direct, the normal index is same as vertex index.
					//get normals by the index of control vertex
					if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lNormalIndex = lVertexIndex;

					//reference mode is index-to-direct, get normals by the index-to-direct
					if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lNormalIndex = lNormalElement->GetIndexArray().GetAt(lVertexIndex);

					//Got normals of each vertex.
					FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
					//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
					//. . .
				}//end for lVertexIndex
			}//end eByControlPoint
			 //mapping mode is by polygon-vertex.
			 //we can get normals by retrieving polygon-vertex.
			else if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int lIndexByPolygonVertex = 0;
				//Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
				for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
				{
					//get polygon size, you know how many vertices in current polygon.
					int lPolygonSize = lMesh->GetPolygonSize(lPolygonIndex);
					//retrieve each vertex of current polygon.
					for (int i = 0; i < lPolygonSize; i++)
					{
						int lNormalIndex = 0;
						//reference mode is direct, the normal index is same as lIndexByPolygonVertex.
						if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
							lNormalIndex = lIndexByPolygonVertex;

						//reference mode is index-to-direct, get normals by the index-to-direct
						if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
							lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

						//Got normals of each polygon-vertex.
						FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
						normals.push_back({ lNormal[0], lNormal[2] , lNormal[1]});
						lIndexByPolygonVertex++;
					}//end for i //lPolygonSize
				}//end for lPolygonIndex //PolygonCount

			}//end eByPolygonVertex
		}//end if lNormalElement

	}//end if lMesh

	 //recursively traverse each node in the scene
	int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		GetNormals(pNode->GetChild(i), normals);
	}
}

/**
 * Read the material information
*/
void GetMaterials(FbxNode* pNode, FbxColor& Ambient, FbxColor& Diffuse, FbxColor& Specular, std::string& textureFile)
{
	if (!pNode)
		return;

	for (int iSrcObj = 0; iSrcObj < pNode->GetSrcObjectCount(); ++iSrcObj) 
	{
		FbxObject* obj = pNode->GetSrcObject<FbxObject>(iSrcObj);
		FbxSurfacePhong* materialPhong = pNode->GetSrcObject < FbxSurfacePhong >(iSrcObj);
		FbxSurfaceLambert* materialLamb = pNode->GetSrcObject< FbxSurfaceLambert >(iSrcObj);
		FbxSurfaceMaterial* material = pNode->GetSrcObject< FbxSurfaceMaterial>(iSrcObj);

		int i = 1;
		if (materialPhong != nullptr) {

			FbxPropertyT<FbxDouble3> lKFbxDouble3;

			lKFbxDouble3 = materialPhong->Ambient;
			Ambient.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

			lKFbxDouble3 = materialPhong->Diffuse;
			Diffuse.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

			lKFbxDouble3 = materialPhong->Specular;
			Specular.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

			for (int is = 0; is < materialPhong->Diffuse.GetSrcObjectCount(); ++is) {
				FbxObject* pobj = materialPhong->Diffuse.GetSrcObject<FbxObject>(is);
				FbxFileTexture* fTexture = materialPhong->Diffuse.GetSrcObject<FbxFileTexture>(is);
				if (fTexture != nullptr) {
					const char* fname = fTexture->GetFileName();
					std::cout << "Texture found: " << fname << std::endl;
					textureFile = fname;
				}
			}
		}
	}
}

void GetUVElement(FbxNode* pNode, FbxGeometryElementUV*& UVElement)
{
	if (!pNode->GetMesh())
		return;
	/// UV
	//get all UV set names
	FbxStringList lUVSetNameList;
	pNode->GetMesh()->GetUVSetNames(lUVSetNameList);
	const char* uvSetName;
	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		uvSetName = lUVSetName;
		UVElement = pNode->GetMesh()->GetElementUV(lUVSetName);
	}
}

void GetUVCoordinates(FbxNode* pNode, std::vector< glm::vec2 >& uv)
{
	FbxGeometryElementUV* UVElement = nullptr;
	GetUVElement(pNode, UVElement);
	/*UVs*/
	//iterating through the data by polygon
	//index array, where holds the index referenced to the uv data
	//index array, where holds the index referenced to the uv data
	if (UVElement != nullptr)
	{
		FbxMesh* lMesh = pNode->GetMesh();
		const bool lUseIndex = UVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? UVElement->GetIndexArray().GetCount() : 0;
		for (int i = 0; i < lIndexCount; ++i)
			uv.push_back({ 0.0f,0.0f });

		//iterating through the data by polygon
		const int lPolyCount = lMesh->GetPolygonCount();

		if (UVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = lMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = lMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? UVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = UVElement->GetDirectArray().GetAt(lUVIndex);

					uv[lPolyVertIndex] = glm::vec2(lUVValue.mData[0], lUVValue.mData[1]);
				}
			}
		}
		else if (UVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = lMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? UVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = UVElement->GetDirectArray().GetAt(lUVIndex);
						int lPolyVertIndex = lMesh->GetPolygonVertex(lPolyIndex, lVertIndex);
						uv[lPolyVertIndex] = glm::vec2(lUVValue.mData[0], 1.0-lUVValue.mData[1]);

						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

void GetSkeleton(FbxNode* pNode)
{
	const char* typeName = pNode->GetTypeName();
	const char* nodeName = pNode->GetName();
	FbxTransform& tr = pNode->GetTransform();
	FbxNodeAttribute* nodeAttr = pNode->GetNodeAttribute();
	FbxSkeleton* skeleton = (FbxSkeleton*)pNode->GetNodeAttribute();
	std::cout << "Typename: " << typeName << std::endl;

	if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		int lClusterCount = 0;
		int  lSkinCount = ((FbxGeometry*)pNode->GetNodeAttribute())->GetDeformerCount(FbxDeformer::eSkin);
		for (int i = 0; i < lSkinCount; ++i)
		{
			FbxSkin *lSkin = (FbxSkin*)((FbxGeometry*)pNode->GetNodeAttribute())->GetDeformer(i, FbxDeformer::eSkin);
			lClusterCount += lSkin->GetClusterCount();

			FbxGeometry *geom = lSkin->GetGeometry();
			int cpCount = lSkin->GetControlPointIndicesCount();
			int* iCPs = lSkin->GetControlPointIndices();
			double* blendWeights = lSkin->GetControlPointBlendWeights();
			for (int iCluster = 0; iCluster < lClusterCount; ++iCluster)
			{
				const FbxCluster* cluster = lSkin->GetCluster(iCluster);
				int iCount = cluster->GetControlPointIndicesCount();
				int* ICPS = cluster->GetControlPointIndices();
				double* weights = cluster->GetControlPointWeights();
				for (int iCP = 0; iCP < iCount; ++iCP)
				{
					int i = 1;
				}
			}
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
		GetSkeleton(pNode->GetChild(i));
}


void DisplayString(const char* pHeader, const char* pValue = "", const char* pSuffix = "")
{
	std::cout << pHeader << " " << pValue << " " << pSuffix;
}

/*ANIMATION*/
void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher = false);
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher = false);
void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher);
void DisplayCurveKeys(FbxAnimCurve* pCurve);
void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty);
void DisplayAnimation(FbxScene* pScene)
{
	int i;
	for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
		FbxString lOutputString = "Animation Stack Name: ";
		lOutputString += lAnimStack->GetName();
		lOutputString += "\n\n";
		FBXSDK_printf(lOutputString);
		DisplayAnimation(lAnimStack, pScene->GetRootNode(), true);
		DisplayAnimation(lAnimStack, pScene->GetRootNode(), false);
	}
}
void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher)
{
	int l;
	int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
	FbxString lOutputString;
	lOutputString = "Animation stack contains ";
	lOutputString += nbAnimLayers;
	lOutputString += " Animation Layer(s)\n";
	FBXSDK_printf(lOutputString);
	for (l = 0; l < nbAnimLayers; l++)
	{
		FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);
		lOutputString = "AnimLayer ";
		lOutputString += l;
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
		DisplayAnimation(lAnimLayer, pNode, isSwitcher);
	}
}
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher)
{
	int lModelCount;
	FbxString lOutputString;
	lOutputString = "     Node Name: ";
	lOutputString += pNode->GetName();
	lOutputString += "\n\n";
	FBXSDK_printf(lOutputString);
	DisplayChannels(pNode, pAnimLayer, DisplayCurveKeys, DisplayListCurveKeys, isSwitcher);
	FBXSDK_printf("\n");
	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		DisplayAnimation(pAnimLayer, pNode->GetChild(lModelCount), isSwitcher);
	}
}
void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher)
{
	FbxAnimCurve* lAnimCurve = NULL;
	// Display general curves.
	if (!isSwitcher)
	{
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TZ\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RZ\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SZ\n");
			DisplayCurve(lAnimCurve);
		}
	}
	// Display curves specific to a light or marker.
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_RED);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Red\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_GREEN);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Green\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_BLUE);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Blue\n");
			DisplayCurve(lAnimCurve);
		}
		// Display curves specific to a light.
		FbxLight* light = pNode->GetLight();
		if (light)
		{
			lAnimCurve = light->Intensity.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Intensity\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = light->OuterAngle.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Outer Angle\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = light->Fog.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Fog\n");
				DisplayCurve(lAnimCurve);
			}
		}
		// Display curves specific to a camera.
		FbxCamera* camera = pNode->GetCamera();
		if (camera)
		{
			lAnimCurve = camera->FieldOfView.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->FieldOfViewX.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View X\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->FieldOfViewY.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View Y\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->OpticalCenterX.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Optical Center X\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->OpticalCenterY.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Optical Center Y\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->Roll.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Roll\n");
				DisplayCurve(lAnimCurve);
			}
		}
		// Display curves specific to a geometry.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;
			int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
			for (int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
			{
				FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
				int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
				for (int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
				{
					FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
					const char* lChannelName = lChannel->GetName();
					lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
					if (lAnimCurve)
					{
						FBXSDK_printf("        Shape %s\n", lChannelName);
						DisplayCurve(lAnimCurve);
					}
				}
			}
		}
	}
	// Display curves specific to properties
	FbxProperty lProperty = pNode->GetFirstProperty();
	while (lProperty.IsValid())
	{
		if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
		{
			FbxString lFbxFCurveNodeName = lProperty.GetName();
			FbxAnimCurveNode* lCurveNode = lProperty.GetCurveNode(pAnimLayer);
			if (!lCurveNode) {
				lProperty = pNode->GetNextProperty(lProperty);
				continue;
			}
			FbxDataType lDataType = lProperty.GetPropertyDataType();
			if (lDataType.GetType() == eFbxBool || lDataType.GetType() == eFbxDouble || lDataType.GetType() == eFbxFloat || lDataType.GetType() == eFbxInt)
			{
				FbxString lMessage;
				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				};
				DisplayString(lMessage.Buffer());
				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
						DisplayCurve(lAnimCurve);
				}
			}
			else if (lDataType.GetType() == eFbxDouble3 || lDataType.GetType() == eFbxDouble4 || lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT))
			{
				char* lComponentName1 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_RED : (char*)"X";
				char* lComponentName2 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_GREEN : (char*)"Y";
				char* lComponentName3 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_BLUE : (char*)"Z";
				FbxString      lMessage;

				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				}
				DisplayString(lMessage.Buffer());
				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
					{
						DisplayString("        Component ", lComponentName1);
						DisplayCurve(lAnimCurve);
					}
				}
				for (int c = 0; c < lCurveNode->GetCurveCount(1U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(1U, c);
					if (lAnimCurve)
					{
						DisplayString("        Component ", lComponentName2);
						DisplayCurve(lAnimCurve);
					}
				}
				for (int c = 0; c < lCurveNode->GetCurveCount(2U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(2U, c);
					if (lAnimCurve)
					{
						DisplayString("        Component ", lComponentName3);
						DisplayCurve(lAnimCurve);
					}
				}
			}
			else if (lDataType.GetType() == eFbxEnum)
			{
				FbxString lMessage;
				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				};
				DisplayString(lMessage.Buffer());
				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
						DisplayListCurve(lAnimCurve, &lProperty);
				}
			}
		}
		lProperty = pNode->GetNextProperty(lProperty);
	} // while
}
static int InterpolationFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
	return 0;
}
static int ConstantmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
	return 0;
}
static int TangentmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
	if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
	if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
	return 0;
}
static int TangentweightFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
	return 0;
}
static int TangentVelocityFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
	return 0;
}
void DisplayCurveKeys(FbxAnimCurve* pCurve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };
	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;
	int lKeyCount = pCurve->KeyGetCount();
	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);
		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}
void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty)
{
	FbxTime   lKeyTime;
	int     lKeyValue;
	char    lTimeString[256];
	FbxString lListValue;
	FbxString lOutputString;
	int     lCount;
	int lKeyCount = pCurve->KeyGetCount();
	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<int>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);
		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " (";
		lOutputString += pProperty->GetEnumValue(lKeyValue);
		lOutputString += ")";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}
/*ANIAMTION*/

std::list< std::unique_ptr<omen::ecs::GameObject> > MeshProvider::loadObject(const std::string& filename)
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	// Load the scene.
	// The example can take a FBX file as an argument.
	FbxString lFilePath(filename.c_str());
	lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());

	///
	// Convert mesh, NURBS and patch into triangle mesh
	FbxGeometryConverter lGeomConverter(lSdkManager);
	lGeomConverter.Triangulate(lScene, true, true);

	// Split meshes per material, so that we only have one material per mesh (for VBO support)
	lGeomConverter.SplitMeshesPerMaterial(lScene, /*replace*/true);
	
	//get root node of the fbx scene
	FbxNode* lRootNode = lScene->GetRootNode();
	std::list< std::unique_ptr<omen::ecs::GameObject> > meshes;

	DisplayAnimation(lScene);

	int childCount = lRootNode->GetChildCount();
	for (int i = 0; i < childCount; ++i) {
		std::vector<Mesh::Vertex> vertices = {};
		std::vector<glm::vec3> normals = {};
		std::vector<glm::vec2> uv = {};
		std::vector<int> indices = {};
		std::vector<glm::vec3> tangents = {};
		std::vector<glm::vec3> bitangents = {};

		FbxColor Ambient;
		FbxColor Diffuse;
		FbxColor Specular;

		std::string textureFile = "";

	
		FbxNode* pNode = lRootNode->GetChild(i);
		const char* name = pNode->GetName();
		GetVertexIndices(pNode, indices, vertices, normals);
		GetSkeleton(pNode);
		//GetNormals(pNode, normals);
		GetMaterials(pNode, Ambient, Diffuse, Specular, textureFile);
		GetUVCoordinates(pNode, uv);
		for (int j = 0; j < pNode->GetChildCount(); ++j)
		{
			FbxNode* childNode = pNode->GetChild(j);
			const char* name = childNode->GetName();
			std::cout << "Child Node: " << name << std::endl;
		}
	
		// Create new Mesh
		std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

		// Set Texturemap
		std::unique_ptr<Material> material = std::make_unique<Material>();
		material->setDiffuseColor(glm::vec4(Diffuse.mRed, Diffuse.mGreen, Diffuse.mBlue, Diffuse.mAlpha));
		material->setSpecularColor(glm::vec4(Specular.mRed, Specular.mGreen, Specular.mBlue, Specular.mAlpha));
		if (!textureFile.empty())
		{
			Texture* t = new Texture(textureFile);
			material->setTexture(t);
		}
	
		mesh->setMaterial(std::move(material));
	
		mesh->setVertexIndices(indices);
		mesh->setVertices(vertices);
		mesh->setNormals(normals);
		mesh->setUVs(uv);

		/**/
		std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>(name);

		std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
		mc->setMesh(std::move(mesh));
		std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>(mc.get());
		obj->addCompnent(std::move(mr));
		obj->addCompnent(std::move(mc));

		FbxVector4 pos = pNode->EvaluateLocalTranslation()*0.01;
		obj->transform()->pos().x = pos[0];
		obj->transform()->pos().y = pos[1];
		obj->transform()->pos().z = pos[2];
		obj->setName(name);

		/**/

		meshes.push_back(std::move(obj));
	}
	//Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

	// Finally return the created mesh
	return meshes;
}

std::unique_ptr<Mesh> MeshProvider::createCube()
{
	glm::vec3 vertexData[] = {
		//bottom face
		{ 1.0f, -1.0f, 1.0f },
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{ -1.0f, -1.0f, 1.0f },
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},


		//top face
		{ -1.0f, 1.0f, -1.0f },
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{ -1.0f, 1.0f, 1.0f },
		{1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},

		//right face
		{ 1.0f, 1.0f, -1.0f },
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f },
		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},

		//front face
		{ 1.0f, 1.0f, 1.0f },
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{ -1.0f, 1.0f, 1.0f },
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},

		//left face
		{ -1.0f, 1.0f, 1.0f },
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f},
		{ -1.0f, 1.0f, -1.0f },
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		
		//back face
		{ 1.0f, -1.0f, -1.0f },
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{ -1.0f, -1.0f, -1.0f },
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f}
	};

	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	std::vector<Mesh::Vertex> vertices = {};
	std::vector<glm::vec3> normals = {};
	std::vector<glm::vec3> tangents = {};
	std::vector<glm::vec2> uv = {};
	for (int i = 0; i < size(vertexData); ++i) {
		vertices.push_back(vertexData[i]);
	}

	glm::vec3 max_pos;
	glm::vec3 min_pos;

	for (int i = 0; i < vertices.size(); i += 3) {
		glm::vec3& v1 = vertices[i];
		glm::vec3& v2 = vertices[(i + 1) % (vertices.size())];
		glm::vec3& v3 = vertices[(i + 2) % (vertices.size())];
		max_pos.x = max(v1.x, max_pos.x); max_pos.y = max(v1.y, max_pos.y); max_pos.z = max(v1.z, max_pos.z);
		min_pos.x = min(v1.x, min_pos.x); min_pos.y = min(v1.y, min_pos.y); min_pos.z = min(v1.z, min_pos.z);

		max_pos.x = max(v2.x, max_pos.x); max_pos.y = max(v2.y, max_pos.y); max_pos.z = max(v2.z, max_pos.z);
		min_pos.x = min(v2.x, min_pos.x); min_pos.y = min(v2.y, min_pos.y); min_pos.z = min(v2.z, min_pos.z);

		max_pos.x = max(v3.x, max_pos.x); max_pos.y = max(v3.y, max_pos.y); max_pos.z = max(v3.z, max_pos.z);
		min_pos.x = min(v3.x, min_pos.x); min_pos.y = min(v3.y, min_pos.y); min_pos.z = min(v3.z, min_pos.z);
	}

	for (int i = 0; i < vertices.size(); i+=3 ) {
		glm::vec3& v1 = vertices[i];
		glm::vec3& v2 = vertices[(i+1) % (vertices.size())];
		glm::vec3& v3 = vertices[(i+2) % (vertices.size())];
		glm::vec3 n = glm::normalize(glm::cross((v2 - v1),(v3 - v1)));
		normals.push_back(n);
		normals.push_back(n);
		normals.push_back(n);

		// Create cube mapped UV coordinates
		glm::vec2 uv1, uv2, uv3;

		float max_value = max(fabs(n.x), max(fabs(n.y), fabs(n.z)));
		if (fabs(n.x) == max_value) {
			if (n.z < 0) {
				uv1.x = fabs((v1.z - min_pos.z) / (max_pos.z - min_pos.z));
				uv1.y = fabs((v1.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.z - min_pos.z) / (max_pos.z - min_pos.z));
				uv2.y = fabs((v2.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.z - min_pos.z) / (max_pos.z - min_pos.z));
				uv3.y = fabs((v3.y - min_pos.y) / (max_pos.y - min_pos.y));
			}
			else {
				uv1.x = fabs((v1.z - max_pos.z) / (max_pos.z - min_pos.z));
				uv1.y = fabs((v1.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.z - max_pos.z) / (max_pos.z - min_pos.z));
				uv2.y = fabs((v2.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.z - max_pos.z) / (max_pos.z - min_pos.z));
				uv3.y = fabs((v3.y - max_pos.y) / (max_pos.y - min_pos.y));
			}
		}
		else 
		if (fabs(n.y) == max_value) {
			if (n.y < 0) {
				uv1.x = fabs((v1.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.z - min_pos.z) / (max_pos.z - min_pos.z));

				uv2.x = fabs((v2.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.z - min_pos.z) / (max_pos.z - min_pos.z));

				uv3.x = fabs((v3.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.z - min_pos.z) / (max_pos.z - min_pos.z));
			}
			else {
				uv1.x = fabs((v1.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.z - max_pos.z) / (max_pos.z - min_pos.z));

				uv2.x = fabs((v2.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.z - max_pos.z) / (max_pos.z - min_pos.z));

				uv3.x = fabs((v3.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.z - max_pos.z) / (max_pos.z - min_pos.z));
			}
		}
		else
		if (fabs(n.z) == max_value) {
			if (n.z < 0) {
				uv1.x = fabs((v1.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.y - min_pos.y) / (max_pos.y - min_pos.y));
			}
			else {
				uv1.x = fabs((v1.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.y - max_pos.y) / (max_pos.y - min_pos.y));
			}
		}
		
		uv.push_back(uv1);
		uv.push_back(uv2);
		uv.push_back(uv3);
	}

	int i = 0; 
	for (auto& v : vertices) {
		v.uv = uv[i];
		v.normal = normals[i];
		++i;
	}

	for (unsigned int i = 0; i < vertices.size()-3; i += 3) {
		glm::vec3& v0 = vertices[i];
		glm::vec3& v1 = vertices[i + 1];
		glm::vec3& v2 = vertices[i + 2];
		glm::vec2& uv0 = uv[i];
		glm::vec2& uv1 = uv[i + 1];
		glm::vec2& uv2 = uv[i + 2];

		glm::vec3 Edge1 = v1 - v0;
		glm::vec3 Edge2 = v2 - v0;

		float DeltaU1 = uv1.x - uv0.x;
		float DeltaV1 = uv1.y - uv0.y;
		float DeltaU2 = uv2.x - uv0.x;
		float DeltaV2 = uv2.y - uv0.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

		tangents.push_back(Tangent);

		vertices[i].tangent += Tangent;
		vertices[i+1].tangent += Tangent;
		vertices[i+2].tangent += Tangent;
	}

	tangents.clear();
	for (auto& v : vertices) {
		v.tangent = glm::normalize(v.tangent);
		tangents.push_back(v.tangent);
	}

	for (auto& t : tangents) {
		t = glm::normalize(t);
	}
		
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setTangents(tangents);
	mesh->setUVs(uv);
	//mesh->setVertexIndices(indices);
	return std::move(mesh);
}

std::unique_ptr<Mesh> MeshProvider::createPlane(float size, int subdiv)
{
	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
	material->setTexture(new Texture("textures/checker.jpg"));

	std::vector<glm::vec3> vertices = { { 0,0,0 },{ 1,0,0 },{ 1,0,1 },{0,0,1} };
	for (auto& v : vertices)
		v *= size;

	std::vector<glm::vec2> uv = { {0,0},{ 1,0 },{ 1,1 },{ 0,1 } };
	std::vector<glm::vec3> normals = { { 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 } };
		
	std::vector<GLsizei> indices;
	std::vector<GLsizei> indices1 = { 0,1,3,1,2,3 };
	for (int i = 0; i < vertices.size(); i+=4)
		for(auto index : indices1)
			indices.push_back(index+i);
	
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setUVs(uv);
	mesh->setVertexIndices(indices);
	mesh->setMaterial(std::move(material));
	return std::move(mesh);
}