#include "MeshProvider.h"
#include "MathUtils.h"

#include <fbxsdk.h>
#include <fbxsdk/utils/fbxgeometryconverter.h>
#include <fbxsdk/scene/fbxaxissystem.h>
#include "Common/Common.h"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

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
						normals.push_back({ lNormal[0], lNormal[1] , lNormal[2]});
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


std::unique_ptr<Mesh> MeshProvider::loadObject(const std::string& filename)
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

	FbxColor Ambient;
	FbxColor Diffuse;
	FbxColor Specular;

	std::string textureFile = "";
	std::cout << "RootNode: " << lRootNode->GetName() << std::endl;
	for (int i = 0; i < lRootNode->GetChildCount(); ++i) {
		FbxNode* pNode = lRootNode->GetChild(i);
		std::cout << "Child: " << lRootNode->GetChild(i)->GetName() << std::endl;
		FbxMesh* lMesh = pNode->GetMesh();
		
		GetNormals(pNode, normals);

		if (lMesh) {
			for (int iSrcObj = 0; iSrcObj < pNode->GetSrcObjectCount(); ++iSrcObj) {
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
			
			/// UV
			//get all UV set names
			FbxStringList lUVSetNameList;
			lMesh->GetUVSetNames(lUVSetNameList);
			const FbxGeometryElementUV* lUVElement = nullptr;
			const char* uvSetName;
			//iterating over all uv sets
			for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
			{
				//get lUVSetIndex-th uv set
				const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
				uvSetName = lUVSetName;
				lUVElement = lMesh->GetElementUV(lUVSetName);
			}
			

			for (int ci = 0; ci < lMesh->GetControlPointsCount(); ci++) {
				fbxsdk::FbxVector4* vp = lMesh->GetControlPoints();
				fbxsdk::FbxVector4 v = vp[ci];
				vertices.push_back(glm::vec3(v[0], v[2], v[1]));
				Mesh::Vertex& mv = vertices.back();
			}
			if (vertices.empty())
				continue;

			/*UVs*/
			//iterating through the data by polygon
			//index array, where holds the index referenced to the uv data
			//index array, where holds the index referenced to the uv data
			if (lUVElement != nullptr) {
				const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
				const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
				uv.reserve(vertices.size());
				for (int i = 0; i < vertices.size(); ++i)
					uv.push_back({ 0.0f,0.0f });

				//iterating through the data by polygon
				const int lPolyCount = lMesh->GetPolygonCount();

				if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
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
							int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

							lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

							uv[lPolyVertIndex] = glm::vec2(lUVValue.mData[0], lUVValue.mData[1]);
						}
					}
				}
				else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
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
								int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

								lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
								int lPolyVertIndex = lMesh->GetPolygonVertex(lPolyIndex, lVertIndex);
								uv[lPolyVertIndex] = glm::vec2(lUVValue.mData[0], lUVValue.mData[1]);

								lPolyIndexCounter++;
							}
						}
					}
				}
			}

				/*UVs*/

				/*uv.reserve(vertices.size());
				for (int i = 0; i < vertices.size(); ++i)
					uv.push_back({ 0.0f,0.0f });*/

					//GetNormals(pNode, normals);
				int polygons = lMesh->GetPolygonCount();
				int vertices = lMesh->GetPolygonVertexCount();
				for (int pi = 0; pi < polygons; pi++) 
				{
					int polySize = lMesh->GetPolygonSize(pi);
					for (int pii = 0; pii < polySize; ++pii) 
					{
						fbxsdk::FbxVector4* vp = lMesh->GetControlPoints();
						int* polylist = lMesh->GetPolygonVertices();
						int v = polylist[lMesh->GetPolygonVertexIndex(pi) + pii];
						indices.push_back(v);
						FbxVector2 pUV;
						bool pUnmapped;
						if (lUVElement != nullptr) {
							lMesh->GetPolygonVertexUV(pi, pii, uvSetName, pUV, pUnmapped);
							float s = pUV.mData[0];
							float t = pUV.mData[1];
							s = fabs(s) < 0.001f ? 0.0f : s;
							t = fabs(t) < 0.001f ? 0.0f : s;
							uv[v] = glm::vec2(s, t);
						}						
					}
				}
			}
	}
	
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	
	// Set Texturemap
	Texture* t = new Texture(textureFile);
	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(Diffuse.mRed, Diffuse.mGreen, Diffuse.mBlue, Diffuse.mAlpha));
	material->setSpecularColor(glm::vec4(Specular.mRed, Specular.mGreen, Specular.mBlue, Specular.mAlpha));
	material->setTexture(t);
	mesh->setMaterial(std::move(material));

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
	std::vector<glm::vec2> actualUVs;
	std::vector<GLsizei> actualIndices;
	actualNormals.reserve(vertices.size());
	for (int i = 0; i < indices.size(); i += 3) {
		int vi1 = indices[i + 2];
		int vi2 = indices[i + 1];
		int vi3 = indices[i + 0];
		Mesh::Vertex& v1 = vertices[vi1];
		Mesh::Vertex& v2 = vertices[vi2];
		Mesh::Vertex& v3 = vertices[vi3];
		glm::vec3 n = glm::normalize(glm::cross((v2.pos - v1.pos), (v3.pos - v1.pos)));
		actualVertices.push_back(v1);
		actualVertices.push_back(v2);
		actualVertices.push_back(v3);
		actualIndices.push_back(i+2);
		actualIndices.push_back(i+1);
		actualIndices.push_back(i);
		v1.normal = ((v1.normal + n)*0.5f);
		v2.normal = ((v2.normal + n)*0.5f);
		v3.normal = ((v3.normal + n)*0.5f);
		actualNormals.push_back(v1.normal);
		actualNormals.push_back(v2.normal);
		actualNormals.push_back(v3.normal);
		actualUVs.push_back(v1.uv);
		actualUVs.push_back(v2.uv);
		actualUVs.push_back(v3.uv);
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
		/*normals.push_back(v1.normal);
		normals.push_back(v2.normal);
		normals.push_back(v3.normal);
		*/
		//uv.push_back(v1.uv);
		//uv.push_back(v2.uv);
		//uv.push_back(v3.uv);
	}

	// Calc tangents
	for (int i = 0; i < vertices.size() - 3; i += 3) {

		// Shortcuts for vertices
		glm::vec3 & v0 = vertices[i + 0];
		glm::vec3 & v1 = vertices[i + 1];
		glm::vec3 & v2 = vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2 & uv0 = uv.empty()?glm::vec2(0.0f,0.0f) : uv[i + 0];
		glm::vec2 & uv1 = uv.empty() ? glm::vec2(0.0f, 0.0f) : uv[i + 1];
		glm::vec2 & uv2 = uv.empty() ? glm::vec2(0.0f, 0.0f) : uv[i + 2];

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
	mesh->setVertexIndices(actualIndices);
	mesh->setVertices(actualVertices);
	mesh->setNormals(normals);
	
	mesh->setUVs(actualUVs);
	
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

std::unique_ptr<Mesh> MeshProvider::createPlane()
{
	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
	material->setTexture(new Texture("textures/checker.jpg"));

	float size = 10.0f;
	std::vector<glm::vec3> vertices = { { -size,0,-size },{ size,0,-size },{ size,0,size },{-size,0,size} };

	std::vector<glm::vec2> uv = { {0,0},{ 1,0 },{ 1,1 },{ 0,1 } };
	std::vector<glm::vec3> normals = { { 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 } };
	/*float max = 100;
	float scale = 10.0/max;
	for (float x = 0; x < max; ++x)
		for (float y = 0; y < max; ++y) {
			float fy  = sin(scale*(scale*x/(max))*(180/3.14))+cos(scale*(scale*y/ (max))*(180/3.14));
			float fy2 = fy;
			vertices.push_back({ scale*(-0.5f+x), fy2,  scale*(0.5f+y)});
			vertices.push_back({ scale*(-0.5f+x), fy, scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,  scale*(0.5f+y)});
			uv.push_back({ 0, 1 });
			uv.push_back({ 0, 0 });
			uv.push_back({ 1, 1 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
		
			vertices.push_back({ scale*(-0.5f+x), fy,  scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,  scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,   scale*(0.5f+y)});
			uv.push_back({ 0, 0 });
			uv.push_back({ 1, 0 });
			uv.push_back({ 1, 1 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
		}*/
		
	std::vector<GLsizei> indices = { 0,1,3,1,2,3 };
	
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setUVs(uv);
	mesh->setVertexIndices(indices);
	mesh->setMaterial(std::move(material));
	return std::move(mesh);
}