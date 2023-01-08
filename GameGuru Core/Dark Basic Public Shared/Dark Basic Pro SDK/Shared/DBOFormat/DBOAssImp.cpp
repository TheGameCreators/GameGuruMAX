//
// DBOAssImp Functions Implementation
//

// Includes


#include "DBOAssImp.h"
#include <vector>
#include <deque>
#include <set>
#include <unordered_set>

#ifdef WICKEDENGINE

// AssImp Includes
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/cfileio.h>

#include "CImageC.h"

char sLoadAssImpObjectError[1024] = "";
bool LocateTexture(char *name);

bool bImportFlipZYInvertZ = false;

const char *pestrcasestr(const char *arg1, const char *arg2);

aiScene *m_pScene = NULL;
aiMatrix4x4 m_GlobalInverseTransform;
aiMatrix4x4 m_ArmatureInverseTransform;
void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4 & ParentTransform);
const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
void LoadBones(const aiMesh* pMesh);
struct BoneInfo
{
	aiMatrix4x4 BoneOffset;
	aiMatrix4x4 FinalTransformation;
	aiMatrix4x4 NodeTransformation;
	aiMatrix4x4 GlobalTransform;
	bool bNodeFound = false;
};

std::vector<BoneInfo> m_BoneInfo;
std::map<std::string, int> m_BoneMapping;
int m_NumBones = 0;


aiNode* get_node_for_mesh(unsigned int meshIndex, aiNode* node)
{
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		if (node->mMeshes[i] == meshIndex) {
			return node;
		}
	}
	for (size_t i = 0; i < node->mNumChildren; ++i) {
		aiNode* ret = get_node_for_mesh(meshIndex, node->mChildren[i]);
		if (ret) { return ret; }
	}
	return nullptr;
}

aiMatrix4x4 get_world_transform(const aiNode* node, const aiScene* scene)
{
	std::vector<const aiNode*> node_chain;
	while (node != scene->mRootNode) {
		node_chain.push_back(node);
		node = node->mParent;
	}
	aiMatrix4x4 transform;
	for (auto n = node_chain.rbegin(); n != node_chain.rend(); ++n) {
		transform *= (*n)->mTransformation;
	}
	return transform;
}

aiMatrix4x4 FBXMatrixBone = {  //PE: Bone with collapsed nodes.
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 };

aiMatrix4x4 FBXMatrixBone_Translation = {  //PE: _Translation
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 };

aiMatrix4x4 FBXMatrixBone_Rotation = {  //PE: _Rotation
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 };

aiMatrix4x4 FBXMatrixBone_PreRotation = {  //PE: _PreRotation
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 };

aiMatrix4x4 FBXMatrixMesh = {  //PE: Mesh with collapsed nodes.
1.0, 0.0, 0.0, 0.0,
0.0, 1.0, 0.0, 0.0,
0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0 };

aiVector3D vOffset = { 0.0,0.0,0.0 }; // From _Translation offset.

bool recursive_ret = false;
bool AnySpecialFBXNodes(aiNode* pcNode)
{
	for (unsigned int i = 0; i < pcNode->mNumChildren; ++i) {
		if( pcNode->mName.length > 0) {
			if (pestrcasestr(pcNode->mName.C_Str(), "assimpfbx$_translation"))
			{
				if (pcNode->mChildren) {
					if (pcNode->mChildren[0]->mMeshes > 0) {
						//This node belong to a node with a mesh (not a bone).
						FBXMatrixMesh = pcNode->mTransformation;
					}
					else {
						FBXMatrixBone_Translation = pcNode->mTransformation;
						FBXMatrixBone = pcNode->mTransformation;
					}
				}
				recursive_ret = true;
			}
			if (pestrcasestr(pcNode->mName.C_Str(), "assimpfbx$_rotation"))
			{
				//Translation a4 = x , b4 = y , c4 = z
				//From a collapsed node get offset like this.
				if (pcNode->mChildren) {
					if (pcNode->mChildren[0]->mMeshes > 0) {
						//This node belong to a node with a mesh (not a bone).
						FBXMatrixMesh = pcNode->mTransformation;
					}
					else {
						FBXMatrixBone_Rotation = pcNode->mTransformation;
						FBXMatrixBone = pcNode->mTransformation;
					}
				}
				vOffset = { pcNode->mTransformation.a4,pcNode->mTransformation.b4,pcNode->mTransformation.c4 };
				//PE: ? Why do i need to invert the offset ?
				vOffset.x *= -1.0;
				vOffset.y *= -1.0;
				vOffset.z *= -1.0;
				recursive_ret = true;
			}
			if (pestrcasestr(pcNode->mName.C_Str(), "assimpfbx$_prerotation"))
			{
				if (pcNode->mChildren) {
					if (pcNode->mChildren[0]->mMeshes > 0) {
						//This node belong to a node with a mesh (not a bone).
						FBXMatrixMesh = pcNode->mTransformation;
					}
					else {
						FBXMatrixBone_PreRotation = pcNode->mTransformation;
						FBXMatrixBone = pcNode->mTransformation;
					}
				}
				recursive_ret = true;
			}
		}
		AnySpecialFBXNodes(pcNode->mChildren[i]);
	}
	return(recursive_ret);
}

void WalkNodes(aiNode* pcNode)
{
	for (unsigned int i = 0; i < pcNode->mNumChildren; ++i) {
		WalkNodes(pcNode->mChildren[i]);
	}
}

int strmatchcount( const char* str1, const char* str2 )
{
	int count = 0;
	while( *str1 && *str2 )
	{
		if ( *str1 != *str2 ) break;
		count++;
		str1++;
		str2++;
	}
	return count;
}

int FindTextureFile( const char* diffusePath, const char* findStr, char* outPath )
{
	char szFolder[ MAX_PATH ];
	char szDiffuseFilename[ MAX_PATH ];
	strcpy( szFolder, diffusePath );
	char szFindStr[ MAX_PATH ];
	strcpy( szFindStr, findStr );
	strlwr( szFindStr );

	// remove
	char* szSlash = strrchr( szFolder, '/' );
	char* szSlashB = strrchr( szFolder, '\\' );
	if ( szSlashB > szSlash ) szSlash = szSlashB;
	if ( szSlash ) 
	{
		strcpy( szDiffuseFilename, szSlash+1 );
		*szSlash = 0;
	}
	else return 0; // must be an absolute path

	uint32_t length = (uint32_t) strlen(szFolder);
	int size = MultiByteToWideChar( CP_UTF8, 0, szFolder, -1, 0, 0 );
	wchar_t *wzPath = new wchar_t[ size+5 ];
	MultiByteToWideChar( CP_UTF8, 0, szFolder, -1, wzPath, size );
	wcscat( wzPath, L"/*.*" );
	
	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = FindFirstFileW( wzPath, &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Failed to find files in folder
		return 0;
	}

	char szUTF8Path[ MAX_PATH ];
	char szUTF8PathLow[ MAX_PATH ];
	int maxMatchCount = 0;
	char szMaxMatchPath[ MAX_PATH ];

	do
	{
		if ( wcscmp( FindFileData.cFileName, L"." ) != 0 && wcscmp( FindFileData.cFileName, L".." ) != 0 )
		{
			if ( !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				// found a file
				WideCharToMultiByte( CP_UTF8, 0, FindFileData.cFileName, -1, szUTF8Path, MAX_PATH, 0, 0 );
				strcpy( szUTF8PathLow, szUTF8Path );
				strlwr( szUTF8PathLow );
				if ( strstr( szUTF8PathLow, szFindStr ) ) // does it contain what we're looking for, e.g. "metalness"
				{
					//// find the closest match to the diffuse filename
					//int matchCount = strmatchcount( szUTF8Path, szDiffuseFilename );
					//if ( matchCount > maxMatchCount)
					//{
					//	maxMatchCount = matchCount;
					//	strcpy( szMaxMatchPath, szFolder );
					//	strcat( szMaxMatchPath, "\\" );
					//	strcat( szMaxMatchPath, szUTF8Path );
					//}
					// Finding the closest match causes emissive textures to be picked up, even when a mesh should not have one applied
					// Instead, make sure it matches the diffuse filename apart from the last 14 characters (longest possibility could be roughness.png)
					int matchCount = strmatchcount(szUTF8Path, szDiffuseFilename);
					if (matchCount > maxMatchCount && matchCount > strlen(szDiffuseFilename) - 8)
					{
						maxMatchCount = matchCount;
						strcpy(szMaxMatchPath, szFolder);
						strcat(szMaxMatchPath, "\\");
						strcat(szMaxMatchPath, szUTF8Path);
					}
				}
			}
		}
	} while ( FindNextFileW( hFind, &FindFileData ) );
	FindClose( hFind );

	if ( maxMatchCount > 0 ) 
	{
		strcpy( outPath, szMaxMatchPath );
		return 1;
	}
	else
	{
		return 0;
	}
}

void SetNewVertData ( aiMesh* pAssImpMesh, int* pRemapVertexIndex, aiVector3D* pNewVertexData, aiVector3D* pNewNormalsData, aiVector3D* pNewTextureCoordsData, aiVector3D* pNewBitangentsData, aiVector3D* pNewTangentsData, int iVertexIndex, int iIndex0, int iIndex1, int iIndex2 )
{
	pRemapVertexIndex[iVertexIndex+0] = iIndex0;
	pRemapVertexIndex[iVertexIndex+1] = iIndex1;
	pRemapVertexIndex[iVertexIndex+2] = iIndex2;
	pNewVertexData[iVertexIndex + 0] = pAssImpMesh->mVertices[iIndex0];
	pNewVertexData[iVertexIndex + 1] = pAssImpMesh->mVertices[iIndex1];
	pNewVertexData[iVertexIndex + 2] = pAssImpMesh->mVertices[iIndex2];
	pNewNormalsData[iVertexIndex + 0] = pAssImpMesh->mNormals[iIndex0];
	pNewNormalsData[iVertexIndex + 1] = pAssImpMesh->mNormals[iIndex1];
	pNewNormalsData[iVertexIndex + 2] = pAssImpMesh->mNormals[iIndex2];
	if (pAssImpMesh->mTextureCoords[0])
	{
		pNewTextureCoordsData[iVertexIndex + 0] = pAssImpMesh->mTextureCoords[0][iIndex0];
		pNewTextureCoordsData[iVertexIndex + 1] = pAssImpMesh->mTextureCoords[0][iIndex1];
		pNewTextureCoordsData[iVertexIndex + 2] = pAssImpMesh->mTextureCoords[0][iIndex2];
	}
	if (pAssImpMesh->mTangents)
	{
		pNewBitangentsData[iVertexIndex + 0] = pAssImpMesh->mBitangents[iIndex0];
		pNewBitangentsData[iVertexIndex + 1] = pAssImpMesh->mBitangents[iIndex1];
		pNewBitangentsData[iVertexIndex + 2] = pAssImpMesh->mBitangents[iIndex2];
		pNewTangentsData[iVertexIndex + 0] = pAssImpMesh->mTangents[iIndex0];
		pNewTangentsData[iVertexIndex + 1] = pAssImpMesh->mTangents[iIndex1];
		pNewTangentsData[iVertexIndex + 2] = pAssImpMesh->mTangents[iIndex2];
	}
	else
	{
		// could generate them, but I think Wicked does not even use them!
		pNewBitangentsData[iVertexIndex + 0] = { 0,0,0 };
		pNewBitangentsData[iVertexIndex + 1] = { 0,0,0 };
		pNewBitangentsData[iVertexIndex + 2] = { 0,0,0 };
		pNewTangentsData[iVertexIndex + 0] = { 0,0,0 };
		pNewTangentsData[iVertexIndex + 1] = { 0,0,0 };
		pNewTangentsData[iVertexIndex + 2] = { 0,0,0 };
	}
}

bool LoadAssImpObject ( char* szFilename, sObject** ppObject, enumScalingMode eScalingMode )
{
	// get model path so we can prepend to texture path
	char pModelPath[MAX_PATH];
	char pModelFileName[MAX_PATH];
	strcpy(pModelFileName, szFilename);
	strcpy(pModelPath, "");
	for (int n = strlen(szFilename)-1; n > 0; n--)
	{
		if (szFilename[n] == '\\' || szFilename[n] == '/')
		{
			strcpy(pModelPath, szFilename);
			pModelPath[n+1] = 0;
			strcpy(pModelFileName, szFilename + n + 1);
			break;
		}
	}

	// setup properties for correct import
	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyInteger(props,AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT );
	
	//#define ONLYREQUIREDSTEPS
	#ifndef ONLYREQUIREDSTEPS
	// set preferences for import behavior
	unsigned int ppsteps =  aiProcess_CalcTangentSpace         | // LB: Some models (X) has no tangents, so calc them (does not overwrite if they exist from model) ## calculate tangents and bitangents if possible
							aiProcess_JoinIdenticalVertices    | // join identical vertices/ optimize indexing
							aiProcess_ValidateDataStructure    | // ## perform a full validation of the loader's output
							aiProcess_ImproveCacheLocality     | // improve the cache locality of the output vertices
							aiProcess_RemoveRedundantMaterials | // remove redundant materials
							aiProcess_FindDegenerates          | // remove degenerated polygons from the import
							aiProcess_FindInvalidData          | // ## detect invalid model data, such as invalid normal vectors
							aiProcess_GenUVCoords              | // ## convert spherical, cylindrical, box and planar mapping to proper UVs
							aiProcess_TransformUVCoords        | // ## preprocess UV transformations (scaling, translation ...)
							//aiProcess_FindInstances            | // search for instanced meshes and remove them by references to one master
							aiProcess_LimitBoneWeights         | // ## limit bone weights to 4 per vertex
							//aiProcess_OptimizeMeshes			  | // PE: Cant use will remove LOD. join small meshes, if possible;
							aiProcess_SplitByBoneCount         | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
							aiProcess_GenSmoothNormals		  | // generate smooth normal vectors if not existing
							aiProcess_SplitLargeMeshes         | // ## split large, unrenderable meshes into submeshes
							aiProcess_Triangulate			  | // ## triangulate polygons with more than 3 edges
							/*aiProcess_ConvertToLeftHanded	  |*/ // convert everything to D3D left handed space
							aiProcess_MakeLeftHanded           | // ## convert everything to OpenGL left handed space
							aiProcess_FlipUVs				   | // flips all UV coordinates along the y-axis and adjusts material settings and bitangents accordingly.
							aiProcess_SortByPType              | // ## make 'clean' meshes which consist of a single typ of primitives;
							0;
	#else
	unsigned int ppsteps = aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
							aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
							aiProcess_Triangulate | // triangulate polygons with more than 3 edges
							aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
							0;
	#endif

	// always with children, imported as single object
	int withChildren = 1;

	// remove scene graph and pre-transforms all vertices with the local transformation matrices of their nodes.
	if (withChildren == 0)
	{
		#ifndef ONLYREQUIREDSTEPS
		ppsteps |= aiProcess_PreTransformVertices;
		#endif
	}
	else
	{
		//PE: aiProcess_OptimizeGraph can delete some nodes on non animated objects, we take care of this later.
		//#ifndef ONLYREQUIREDSTEPS
		//ppsteps |= aiProcess_OptimizeGraph;
		//#endif
	}

	// perform the import from the model file into the scene object
	aiScene *pScene = (aiScene*)aiImportFileExWithProperties( szFilename, ppsteps, NULL, props);
	m_pScene = pScene;
	// no scene loaded
	if ( !pScene )
	{
		//PE: WICKED gamecore\projectileTypes\enhanced\m67\m67.x fail.
		sprintf (sLoadAssImpObjectError, "Failed to load: %s (%s)", szFilename, aiGetErrorString() );
		aiReleasePropertyStore(props);
		return false;
	}

	// must have meshes
	if (pScene->mNumMeshes < 1)
	{
		extern bool importer_havevalidobject();
		if (!importer_havevalidobject())
		{
			sprintf(sLoadAssImpObjectError, "Model has no meshes");
			aiReleasePropertyStore(props);
			return false;
		}
	}

	// check if have bones
	int hasBones = 0;
	bool bHasLOD = false;
	bool bHasAnimation = false;
	
	if (pScene->HasAnimations())
	{
		//PE: dont use aiProcess_PreTransformVertices if we have animations.
		bHasAnimation = true;
	}

	// assimp node list
	std::deque<aiNode*> nodeList;

	// scaling modes
	float modelScale = 1.0f;
	switch ( eScalingMode )
	{
		case eScalingMode_Meter: modelScale = 39.3701f; // 1 meter / inch = 39.3701 
			break;

		case eScalingMode_Inch: modelScale = 1.0f; // raw assumed to be 1 unit = 1 inch = 1.0f
			break;

		case eScalingMode_Centimeter: modelScale = 0.393701f; // 1 cm / inch = 0.393701
			break;

		case eScalingMode_Automatic:
			{
				// determine model size from vertices
				float modelMinX = 1000000000.0f;
				float modelMaxX = -1000000000.0f;
				float modelMinY = 1000000000.0f;
				float modelMaxY = -1000000000.0f;
				float modelMinZ = 1000000000.0f;
				float modelMaxZ = -1000000000.0f;

				nodeList.push_back(pScene->mRootNode);

				while (nodeList.size() > 0)
				{
					aiNode *pNode = nodeList[0];
					nodeList.pop_front();

					for (int i = 0; i < pNode->mNumChildren; i++)
					{
						nodeList.push_back(pNode->mChildren[i]);
					}

					for (int m = 0; m < pNode->mNumMeshes; m++)
					{
						aiMesh *pAssImpMesh = pScene->mMeshes[pNode->mMeshes[m]];
						aiVector3D meshScale(1, 1, 1);
						if (pAssImpMesh->HasBones())
						{
							aiVector3D bonePos, boneScale;
							aiQuaternion boneRot;
							pAssImpMesh->mBones[0]->mOffsetMatrix.Decompose(boneScale, boneRot, bonePos);
							meshScale = boneScale;
						}

						for (int v = 0; v < pAssImpMesh->mNumVertices; v++)
						{
							aiVector3D pos = pAssImpMesh->mVertices[v];
							pos.x *= meshScale.x;
							pos.y *= meshScale.y;
							pos.z *= meshScale.z;
							//pos = pNode->mTransformation * pos;

							if (pos.x < modelMinX) modelMinX = pos.x;
							if (pos.x > modelMaxX) modelMaxX = pos.x;
							if (pos.y < modelMinY) modelMinY = pos.y;
							if (pos.y > modelMaxY) modelMaxY = pos.y;
							if (pos.z < modelMinZ) modelMinZ = pos.z;
							if (pos.z > modelMaxZ) modelMaxZ = pos.z;
						}
					}
				}

				float modelSizeX = modelMaxX - modelMinX;
				float modelSizeY = modelMaxY - modelMinY;
				float modelSizeZ = modelMaxZ - modelMinZ;
				float modelSize = modelSizeX;
				if (modelSizeY > modelSize) modelSize = modelSizeY;
				if (modelSizeZ > modelSize) modelSize = modelSizeZ;

				// determine how much we need to scale the model to match a standard size
				modelScale = 60.0f / modelSize;
			}
			break;
	}

	for( int i = 0; i < pScene->mNumMeshes; i++ )
	{
		aiString mesh_name = pScene->mMeshes[i]->mName;
		LPSTR pRightFive = "";
		if (mesh_name.length >= 5) pRightFive = (char *) mesh_name.C_Str() + (mesh_name.length - 5);
		if ( (stricmp(pRightFive, "lod_1") == 0 || stricmp(pRightFive, "_lod1") == 0) || 
			 (stricmp(pRightFive, "lod_2") == 0 || stricmp(pRightFive, "_lod2") == 0) ||
			 (stricmp(pRightFive, "lod_3") == 0 || stricmp(pRightFive, "_lod3") == 0) )
		{
			bHasLOD = true;
		}
		if ( pScene->mMeshes[i]->HasBones() ) 
		{
			hasBones = 1;
		}
	}

	//PE: If we dont have bones , we need this , or some nodes can be lost.
	//LB: this merges meshes together, not desirable for all models imported, default is NO merge (possible import option!)
	bool bMergeAllMeshesWithSameMaterialReferences = false;
	if (!hasBones && !bHasAnimation && !bHasLOD && bMergeAllMeshesWithSameMaterialReferences == true)
	{
		//PE: Need to reload for this to work. assimp will delete old scene.
		//PE: aiProcess_PreTransformVertices will delete any animation so need to be done like this.
		ppsteps |= aiProcess_OptimizeMeshes;
		ppsteps |= aiProcess_PreTransformVertices;
		aiReleaseImport(pScene);
		pScene = (aiScene*)aiImportFileExWithProperties(szFilename, ppsteps, NULL, props);
		m_pScene = pScene;
		if (!pScene)
		{
			sprintf(sLoadAssImpObjectError, "Static object failed: %s (%s)", szFilename, aiGetErrorString());
			aiReleasePropertyStore(props);
			return false;
		}
		if (pScene->mNumMeshes < 1)
		{
			sprintf(sLoadAssImpObjectError, "Static model has no meshes");
			aiReleasePropertyStore(props);
			return false;
		}
		hasBones = 0;
	}

	// finally release the properties
	aiReleasePropertyStore(props);

	// create an object
	*ppObject = new sObject();
	sObject* pObject = *ppObject;

	// store name of model root
	std::string sRootName = pScene->mRootNode->mName.C_Str();

	// handle imbedded textures later (built-in textures?)
	//if (pScene->HasTextures()) 

	// materials store the texture references
	std::vector<std::string> sDiffuseTextureList;
	std::vector<std::string> sNormalsTextureList;
	std::vector<std::string> sMetalnessTextureList;
	std::vector<std::string> sRoughnessTextureList;
	std::vector<std::string> sAOTextureList;
	std::vector<std::string> sEmissiveTextureList;
	std::vector<std::string> sSurfaceTextureList; // generated from AO + roughness + metalness, stored for performance otherwise it is generated for every mesh instead of every material
	
	if (pScene->HasMaterials())
	{
		for (unsigned int ti = 0; ti < pScene->mNumMaterials; ti++)
		{
			const aiMaterial* pMaterial = pScene->mMaterials[ti];
			unsigned int diffuseCount = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
			unsigned int normalCount = pMaterial->GetTextureCount(aiTextureType_NORMALS);
			unsigned int emissiveCount = pMaterial->GetTextureCount(aiTextureType_EMISSIVE);
			
			// standard PBR textures
			unsigned int baseColorCount = pMaterial->GetTextureCount(aiTextureType_BASE_COLOR);
			unsigned int normalCameraCount = pMaterial->GetTextureCount(aiTextureType_NORMAL_CAMERA);
			unsigned int emissionColorCount = pMaterial->GetTextureCount(aiTextureType_EMISSION_COLOR);
			unsigned int metalnessCount = pMaterial->GetTextureCount(aiTextureType_METALNESS);
			unsigned int roughnessCount = pMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
			unsigned int aoCount = pMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
			
			aiString Path, FullPath;
			aiString diffusePath;

			// diffuse (or base color)
			bool bDiffuseFound = false;
			if ( diffuseCount > 0 && pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bDiffuseFound = true;
			}
			else if ( baseColorCount > 0 && pMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bDiffuseFound = true;
			}

			if ( !bDiffuseFound ) sDiffuseTextureList.push_back("");
			else
			{
				sDiffuseTextureList.push_back(FullPath.C_Str());
				diffusePath = FullPath;
			}
			
			// normal (or camera normal)
			bool bNormalFound = false;
			if ( normalCount > 0 && pMaterial->GetTexture(aiTextureType_NORMALS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bNormalFound = true;
			}
			else if ( normalCameraCount > 0 && pMaterial->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bNormalFound = true;
			}
			else
			{
				char szNormalPath[ MAX_PATH ];
				if ( diffusePath.length > 0 && FindTextureFile( diffusePath.C_Str(), "normal", szNormalPath ) )
				{
					FullPath = szNormalPath;
					bNormalFound = true;
				}
			}

			if ( !bNormalFound ) sNormalsTextureList.push_back("");
			else
			{
				sNormalsTextureList.push_back(FullPath.C_Str());
			}

			// metalness
			bool bMetalnessFound = false; 
			if ( metalnessCount > 0 && pMaterial->GetTexture(aiTextureType_METALNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bMetalnessFound = true;
			}
			else
			{
				char szMetalPath[ MAX_PATH ];
				if ( diffusePath.length > 0 )
				{
					if ( FindTextureFile( diffusePath.C_Str(), "metalness", szMetalPath )
					  || FindTextureFile( diffusePath.C_Str(), "metallic", szMetalPath ) )
					{
						FullPath = szMetalPath;
						bMetalnessFound = true;
					}
				}
			}

			if ( !bMetalnessFound ) sMetalnessTextureList.push_back("");
			else
			{
				sMetalnessTextureList.push_back(FullPath.C_Str());
			}

			// roughness
			bool bRoughnessFound = false; 
			if ( roughnessCount > 0 && pMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bRoughnessFound = true;
			}
			else
			{
				char szRoughnessPath[ MAX_PATH ];
				if ( diffusePath.length > 0 && FindTextureFile( diffusePath.C_Str(), "roughness", szRoughnessPath ) )
				{
					FullPath = szRoughnessPath;
					bRoughnessFound = true;
				}
			}

			if ( !bRoughnessFound ) sRoughnessTextureList.push_back("");
			else
			{
				sRoughnessTextureList.push_back(FullPath.C_Str());
			}

			// ambient occlusion
			bool bOcclusionFound = false; 
			if ( aoCount > 0 && pMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bOcclusionFound = true;
			}
			else
			{
				char szOcclusionPath[ MAX_PATH ];
				if ( diffusePath.length > 0 )
				{
					if ( FindTextureFile( diffusePath.C_Str(), "occlusion", szOcclusionPath ) 
					  || FindTextureFile( diffusePath.C_Str(), "_ao", szOcclusionPath ) )
					{
						FullPath = szOcclusionPath;
						bOcclusionFound = true;
					}
				}
			}

			if ( !bOcclusionFound ) sAOTextureList.push_back("");
			else
			{
				sAOTextureList.push_back(FullPath.C_Str());
			}

			// generate surface here otherwise it will be generated per mesh instead of per material
			// surface generation is slow due to DirectX::Compress()
			if ( diffusePath.length > 0 )
			{
				// get texture index into texture list
				int index = sDiffuseTextureList.size() - 1;

				// pull any texture names for ao, roughness and metalness
				char szOcclusionFile[ MAX_PATH ]; strcpy( szOcclusionFile, sAOTextureList[index].c_str() );
				char szRoughnessFile[ MAX_PATH ]; strcpy( szRoughnessFile, sRoughnessTextureList[index].c_str() );
				char szMetalnessFile[ MAX_PATH ]; strcpy( szMetalnessFile, sMetalnessTextureList[index].c_str() );

				// construct surface texture file name to sit alongside original files
				// LB: need to detect if this is outside MAX folders, if so, do this as a temp file (see importer and combine code)
				char szSurfaceFile[ MAX_PATH ];
				strcpy( szSurfaceFile, sDiffuseTextureList[index].c_str() );
				char* szExt = strrchr( szSurfaceFile, '.' );
				if ( szExt ) *szExt = 0;
				strcat( szSurfaceFile, "_surface.dds" );

				// AUTO DETECT SPECIFIC FILENAME CONVENTIONS SO MAX CAN MAKE A BEST GUESS FOR COLOR CHANNELS
				int iMAXCanMakeABestGuess = 0;
				for (int iTryAllThree = 0; iTryAllThree < 3; iTryAllThree++)
				{
					// try all three PBR textures that might have been specified
					char pTryThisFilename[ MAX_PATH ];
					if (iTryAllThree == 0 ) strcpy( pTryThisFilename, szOcclusionFile );
					if (iTryAllThree == 1 ) strcpy( pTryThisFilename, szRoughnessFile );
					if (iTryAllThree == 2 ) strcpy( pTryThisFilename, szMetalnessFile );
					char* szTryExt = strrchr( pTryThisFilename, '.' );
					if ( szTryExt ) *szTryExt = 0;

					// compare with known Exported Texture Files
					if (strstr (pTryThisFilename, "_metallicRoughness") != NULL) iMAXCanMakeABestGuess = 1; // SKETCHFAB GLTF
				}
				// submit PBR textures to create a new surface texture file representing correct PBR for this mesh
				if (iMAXCanMakeABestGuess > 0)
				{
					// we think we have found a known texture naming convention, assign suspected channels
					int iAOChannel = 0, iRoughChannel = 0, iMetalChannel = 0;
					if (iMAXCanMakeABestGuess == 1) 
					{
						// SKETCHFAB GLTF R=ao, G=roughness, B=metalness
						iAOChannel = 0; iRoughChannel = 1; iMetalChannel = 2; 

						// Also, if no szOcclusionFile specified, AO is stored in "_metallicRoughness" texture under RED
						if (strlen(szOcclusionFile) == 0 && strlen(szRoughnessFile)>0) strcpy (szOcclusionFile, szRoughnessFile);
						if (strlen(szOcclusionFile) == 0 && strlen(szMetalnessFile)>0) strcpy (szOcclusionFile, szMetalnessFile);
					}

					// generate surface texture knowing where data is within the channels
					ImageCreateSurfaceTextureChannels(szSurfaceFile, szOcclusionFile, szRoughnessFile, szMetalnessFile, iAOChannel, iRoughChannel, iMetalChannel, 0);
				}
				else
				{
					// standard surface created, pull from RED channel of any specified texture
					#ifdef WICKEDENGINE
					if (strlen(szOcclusionFile) > 0 || strlen(szRoughnessFile) > 0 || strlen(szMetalnessFile) > 0)
						ImageCreateSurfaceTexture(szSurfaceFile, szOcclusionFile, szRoughnessFile, szMetalnessFile);
					#else 
					ImageCreateSurfaceTexture(szSurfaceFile, szOcclusionFile, szRoughnessFile, szMetalnessFile);
					#endif
				}

				// add new surface texture to list
				sSurfaceTextureList.push_back( szSurfaceFile );
			}
			else
			{
				sSurfaceTextureList.push_back("");
			}

			// emissive
			bool bEmissiveFound = false; 
			if ( emissiveCount > 0 && pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bEmissiveFound = true;
			}
			else if ( emissionColorCount > 0 && pMaterial->GetTexture(aiTextureType_EMISSION_COLOR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
				FullPath.Set( pModelPath );
				FullPath.Append( Path.C_Str() );
				bEmissiveFound = true;
			}
			else
			{
				char szEmissivePath[ MAX_PATH ];
				if ( diffusePath.length > 0 && FindTextureFile( diffusePath.C_Str(), "emissive", szEmissivePath ) )
				{
					FullPath = szEmissivePath;
					bEmissiveFound = true;
				}
			}

			if ( !bEmissiveFound ) sEmissiveTextureList.push_back("");
			else
			{
				sEmissiveTextureList.push_back(FullPath.C_Str());
			}
		}
	}

	//PE: FBX , scan backward to find any special FBX nodes.
	aiNode *pFBXNode = pScene->mRootNode;

	m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
	m_GlobalInverseTransform.Inverse();

	recursive_ret = false;
	bImportFlipZYInvertZ = AnySpecialFBXNodes(pFBXNode);


	//actual bone nodes in fbx, without parenting-up
	std::unordered_set<std::string> setAllBoneNamesInScene;
	for (unsigned int m = 0; m < pScene->mNumMeshes; ++m)
	{
		aiMesh* pMesh = pScene->mMeshes[m];
		for (unsigned int b = 0; b < pMesh->mNumBones; ++b)
			setAllBoneNamesInScene.insert(pMesh->mBones[b]->mName.data);
	}

	// and a map of nodes by bone name, as finding them is annoying.
	struct SortNodeByName
	{
		bool operator()(const aiNode *lhs, const aiNode *rhs) const
		{
			return strcmp(lhs->mName.C_Str(), rhs->mName.C_Str()) < 0;
		}
	};

	aiMatrix4x4 mxTransIdentity;
	const std::string MAGIC_NODE_TAG = "_$AssimpFbx$";
	std::vector<std::set<const aiNode*, SortNodeByName>> skeleton_by_mesh(pScene->mNumMeshes);
	std::map<std::string, aiNode*> node_by_bone;
	for (size_t mi = 0; mi < pScene->mNumMeshes; ++mi) {
		const aiMesh* m = pScene->mMeshes[mi];
		std::set<const aiNode*, SortNodeByName> skeleton;
		for (size_t bi = 0; bi < m->mNumBones; ++bi) {
			const aiBone* b = m->mBones[bi];
			const std::string name(b->mName.C_Str());
			auto elem = node_by_bone.find(name);
			aiNode* n;
			if (elem != node_by_bone.end()) {
				n = elem->second;
			}
			else {
				n = pScene->mRootNode->FindNode(b->mName);
				if (!n) {
					// Failed to find node for bone
				}
				else {
					node_by_bone[name] = n;
				}
			}
			skeleton.insert(n);
			// mark all parent nodes as skeleton as well,
			// up until we find the root node,
			// or else the node containing the mesh,
			// or else the parent of a node containig the mesh.
			for (
				const aiNode* parent = n->mParent;
				parent && parent != pScene->mRootNode;
				parent = parent->mParent
				) {
				// if we've already done this node we can skip it all
				if (skeleton.count(parent)) {
					break;
				}
				// ignore fbx transform nodes as these will be collapsed later
				// TODO: cache this by aiNode*
				const std::string node_name(parent->mName.C_Str());
				if (node_name.find(MAGIC_NODE_TAG) != std::string::npos) {
					continue;
				}
				//not a bone in scene && no effect in transform
				if (setAllBoneNamesInScene.find(node_name) == setAllBoneNamesInScene.end()
					&& parent->mTransformation == mxTransIdentity) {
					continue;
				}
				// otherwise check if this is the root of the skeleton
				bool end = false;
				// is the mesh part of this node?
				for (size_t i = 0; i < parent->mNumMeshes; ++i) {
					if (parent->mMeshes[i] == mi) {
						end = true;
						break;
					}
				}
				// is the mesh in one of the children of this node?
				for (size_t j = 0; j < parent->mNumChildren; ++j) {
					aiNode* child = parent->mChildren[j];
					for (size_t i = 0; i < child->mNumMeshes; ++i) {
						if (child->mMeshes[i] == mi) {
							end = true;
							break;
						}
					}
					if (end) { break; }
				}

				// if it was the skeleton root we can finish here
				if (end) { break; }
			}
		}
		skeleton_by_mesh[mi] = skeleton;
	}

	nodeList.push_back( pScene->mRootNode );

	// keep track of node<->frame pairs
	std::vector<aiNode*> pairNode;
	std::vector<sFrame*> pairFrame;

	// create a frame for each node
	while( nodeList.size() > 0 )
	{
		// get node from node list (expands as we traverse the whole hierarchy)
		aiNode *pNode = nodeList[ 0 ];
		nodeList.pop_front();

		// create DBO frame (first one is parent root)
		sFrame* pFrame = new sFrame();
		if (pObject->pFrame == NULL) pObject->pFrame  = pFrame;
		strcpy(pFrame->szName, pNode->mName.C_Str());
		
		// record the node-frame-pair
		pairNode.push_back(pNode);
		pairFrame.push_back(pFrame);

		// attach frame to its parent
		sFrame* pThisFramesParent = NULL;
		if ( pNode->mParent ) 
		{
			// find parent of this node, and set frame parent that matches
			for (int n = 0; n < pairNode.size(); n++)
			{
				if (pairNode[n] == pNode->mParent) 
				{ 
					pThisFramesParent = pairFrame[n];
					pFrame->pParent = pThisFramesParent; 
					if (pThisFramesParent->pChild == NULL)
					{
						// add first child to parent
						pThisFramesParent->pChild = pFrame;
					}
					else
					{
						// subsequent frames added to siblind of the parents child
						sFrame* pAddToThisSibling = pThisFramesParent->pChild;
						if (pAddToThisSibling->pSibling == NULL)
						{
							pAddToThisSibling->pSibling = pFrame;
						}
						else
						{
							sFrame* pFindLastSibling = pAddToThisSibling->pSibling;
							while (pFindLastSibling->pSibling) { pFindLastSibling = pFindLastSibling->pSibling; }
							pFindLastSibling->pSibling = pFrame;
						}
					}
					break; 
				}
			}
		}

		pFrame->bHasTransformed = false;

		// get bone transform vectors and quat
		aiVector3D nodePos, nodeScale;
		aiQuaternion nodeRot;
		pNode->mTransformation.Decompose( nodeScale, nodeRot, nodePos );
		
		if ( (fabs(nodeScale.x-1) > 0.01f || fabs(nodeScale.y-1) > 0.01f || fabs(nodeScale.z-1) > 0.01f) )
		{
			int a=0; // for debugger breakpoint
		}

		pFrame->bRemovedScale = false;
		if ( pNode->mNumMeshes == 0 )
		{
			aiVector3D newScale = nodeScale;

			if ( pFrame->pParent && pFrame->pParent->bHasTransformed ) pFrame->bHasTransformed = true;
			else
			{
				if ( modelScale != 1.0f )//bNormalizeScale ) 
				{
					pFrame->bRemovedScale = true;
					newScale.Set( 1, 1, 1 ); // remove any scale that is applied near the root (bHasTransformed is false)
				}

				if ( fabs(nodeScale.x-1) > 0.01f || fabs(nodeScale.y-1) > 0.01f || fabs(nodeScale.z-1) > 0.01f
				  || fabs(nodePos.x) > 0.01f || fabs(nodePos.y) > 0.01f || fabs(nodePos.z) > 0.01f
				  || fabs(nodeRot.x) > 0.01f || fabs(nodeRot.y) > 0.01f || fabs(nodeRot.z) > 0.01f || fabs(nodeRot.w-1) > 0.01f )
				{
					pFrame->bHasTransformed = true;
				}
			}

			aiMatrix4x4 matNew( newScale, nodeRot, nodePos );

			// set the frame(node) bone space offset
			pFrame->matOriginal._11 = matNew.a1;
			pFrame->matOriginal._12 = matNew.b1;
			pFrame->matOriginal._13 = matNew.c1;
			pFrame->matOriginal._14 = matNew.d1;
			pFrame->matOriginal._21 = matNew.a2;
			pFrame->matOriginal._22 = matNew.b2;
			pFrame->matOriginal._23 = matNew.c2;
			pFrame->matOriginal._24 = matNew.d2;
			pFrame->matOriginal._31 = matNew.a3;
			pFrame->matOriginal._32 = matNew.b3;
			pFrame->matOriginal._33 = matNew.c3;
			pFrame->matOriginal._34 = matNew.d3;
			pFrame->matOriginal._41 = matNew.a4 * modelScale;
			pFrame->matOriginal._42 = matNew.b4 * modelScale;
			pFrame->matOriginal._43 = matNew.c4 * modelScale;
			pFrame->matOriginal._44 = matNew.d4;		
		}
		else
		{
			GGMatrixIdentity( &pFrame->matOriginal );
		}

		// add new nodes to nodelist using all children of the current node (thus traversing the list)
		for ( int i = 0; i < pNode->mNumChildren; i++ )
		{
			nodeList.push_back( pNode->mChildren[ i ] );
		}

		// check current node for meshes
		if ( pNode->mNumMeshes > 0 )
		{
			// go through all meshes of this node
			int count = 0;
			sFrame* pMasterFrame = pFrame;
			for ( int m = 0; m < pNode->mNumMeshes; m++ )
			{
				// for each scene mesh
				aiMesh *pAssImpMesh = pScene->mMeshes[ pNode->mMeshes[m] ];

				// create a DBO mesh
				sMesh* pDBOMesh = new sMesh();

				// associate it with a frame (this one or a sibling frame to it)
				if (pMasterFrame->pMesh == NULL)
				{
					// first mesh attaches to this frame
					pMasterFrame->pMesh = pDBOMesh;
					//LB: Need to name frames even if no LOD (so animations with no bones can find/animate frames)
					//if (bHasLOD && pAssImpMesh->mName.length > 0)
					if (pScene->HasAnimations() ) // LB no such function in Wicked Repo || pScene->hasSkeletons())
					{
						if (pAssImpMesh->mName.length > 0)
						{
							strcpy(pMasterFrame->szName, pAssImpMesh->mName.C_Str());
							//extern void importer_collectmeshname(char* meshName);
							//importer_collectmeshname((char*)pAssImpMesh->mName.C_Str());
						}
					}
					else if (pScene->HasMaterials())
					{
						aiString matName;
						pScene->mMaterials[pAssImpMesh->mMaterialIndex]->Get(AI_MATKEY_NAME, matName);
						if (pAssImpMesh->mName.length > 0)
							strcpy(pMasterFrame->szName, matName.C_Str());
						//extern void importer_collectmeshname(char*);
						//importer_collectmeshname((char*)matName.C_Str());
					}
				}
				else
				{
					// need to create a new frame to hold sybling meshes
					sFrame* pSiblingFrame = new sFrame();
					//PE: Give it the original mesh name ( for LOD support ).
					//LB: Need to name frames even if no LOD (so animations with no bones can find/animate frames)
					//if(bHasLOD && pAssImpMesh->mName.length > 0 )
					if (pScene->HasAnimations() ) // LB no such function in Wicked Repo || pScene->hasSkeletons())
					{
						if (pAssImpMesh->mName.length > 0)
							strcpy(pSiblingFrame->szName, pAssImpMesh->mName.C_Str());
						else
							strcpy(pSiblingFrame->szName, "sibling frame");
					}
					else if (pScene->HasMaterials())
					{
						aiString matName;
						pScene->mMaterials[pAssImpMesh->mMaterialIndex]->Get(AI_MATKEY_NAME, matName);
						if (pAssImpMesh->mName.length > 0)
							strcpy(pSiblingFrame->szName, matName.C_Str());
					}
					else
					{
						if (pAssImpMesh->mName.length > 0)
							strcpy(pSiblingFrame->szName, pAssImpMesh->mName.C_Str());
						else
							strcpy(pSiblingFrame->szName, "sibling frame");
					}
					
					pSiblingFrame->pMesh = pDBOMesh;

					// inform sibling of parent
					pSiblingFrame->pParent = pThisFramesParent;

					// attach sibling to master current frame
					sFrame* pFindFreeSiblingSlot = pMasterFrame;
					while (pFindFreeSiblingSlot->pSibling) pFindFreeSiblingSlot = pFindFreeSiblingSlot->pSibling;
					pFindFreeSiblingSlot->pSibling = pSiblingFrame;
				}
			
				aiVector3D boneScale( 1, 1, 1 );
				aiMatrix4x4 Identity;
				// modify input data from assimp if mesh has bones (from AGK code related to scale, may remove this..)
				if ( pAssImpMesh->HasBones() )
				{

					static bool firstbonefound = false;

					// create bone data for DBO mesh and copy from assimp mesh
					int iBoneCount = pAssImpMesh->mNumBones;
					pDBOMesh->dwBoneCount = iBoneCount;
					pDBOMesh->pBones = new sBone[iBoneCount];
					for (int b = 0; b < iBoneCount; b++)
					{
						strcpy(pDBOMesh->pBones[b].szName, pAssImpMesh->mBones[b]->mName.C_Str());
						int iInfluenceCount = pAssImpMesh->mBones[b]->mNumWeights;
						pDBOMesh->pBones[b].dwNumInfluences = iInfluenceCount;
						pDBOMesh->pBones[b].pWeights = new float[iInfluenceCount];
						pDBOMesh->pBones[b].pVertices = new DWORD[iInfluenceCount];
						for (int i = 0; i < iInfluenceCount; i++)
						{
							if (!pAssImpMesh->mBones[b]->mWeights)
							{
								//								//PE: bone have no weights.
								delete[] pDBOMesh->pBones[b].pWeights;
								delete[] pDBOMesh->pBones[b].pVertices;
								pDBOMesh->pBones[b].pVertices = NULL;
								pDBOMesh->pBones[b].pWeights = NULL;
								pDBOMesh->pBones[b].dwNumInfluences = 0;
								break;
							}
							pDBOMesh->pBones[b].pVertices[i] = pAssImpMesh->mBones[b]->mWeights[i].mVertexId;
							pDBOMesh->pBones[b].pWeights[i] = pAssImpMesh->mBones[b]->mWeights[i].mWeight;
						}

						GGMatrixIdentity(&pDBOMesh->pBones[b].matTranslation);
						
						//Inverse bind pose.
						aiVector3D pos, scale;
						aiQuaternion rot;
						pAssImpMesh->mBones[b]->mOffsetMatrix.Decompose( scale, rot, pos );
						if ( modelScale != 1.0f )
						{
							boneScale = scale;
							scale.Set( 1, 1, 1 );
						}
						aiMatrix4x4 final( scale, rot, pos );
						
						pDBOMesh->pBones[b].matTranslation._11 = final.a1;
						pDBOMesh->pBones[b].matTranslation._12 = final.b1;
						pDBOMesh->pBones[b].matTranslation._13 = final.c1;
						pDBOMesh->pBones[b].matTranslation._14 = final.d1;
						pDBOMesh->pBones[b].matTranslation._21 = final.a2;
						pDBOMesh->pBones[b].matTranslation._22 = final.b2;
						pDBOMesh->pBones[b].matTranslation._23 = final.c2;
						pDBOMesh->pBones[b].matTranslation._24 = final.d2;
						pDBOMesh->pBones[b].matTranslation._31 = final.a3;
						pDBOMesh->pBones[b].matTranslation._32 = final.b3;
						pDBOMesh->pBones[b].matTranslation._33 = final.c3;
						pDBOMesh->pBones[b].matTranslation._34 = final.d3;
						pDBOMesh->pBones[b].matTranslation._41 = final.a4 * modelScale;
						pDBOMesh->pBones[b].matTranslation._42 = final.b4 * modelScale;
						pDBOMesh->pBones[b].matTranslation._43 = final.c4 * modelScale;
						pDBOMesh->pBones[b].matTranslation._44 = final.d4;
					}
				}
				else
				{
					// no modification of assimp mesh data for static non-bone meshes
				}

				// count triangle indices from assimp mesh face data
				int iIndiceCount = 0;
				for (int f = 0; f < pAssImpMesh->mNumFaces; f++)
					iIndiceCount += pAssImpMesh->mFaces[f].mNumIndices;

				// populate DBO mesh with assimp mesh data
				int iVertexCount = pAssImpMesh->mNumVertices;

				// mesh needs pos, norm, tex0, binorm, tangent, weights, indices
				GGVERTEXELEMENT Declaration[MAX_FVF_DECL_SIZE];
				DWORD dwNumBytesPerVertex = 0;
				int iByteOffset = 0;
				Declaration[0].Usage = GGDECLUSAGE_POSITION;
				Declaration[0].Type = GGDECLTYPE_FLOAT3;
				Declaration[0].Stream = 0;
				Declaration[0].Method = GGDECLMETHOD_DEFAULT;
				Declaration[0].UsageIndex = 0;
				Declaration[0].Offset = iByteOffset;
				iByteOffset += 12;
				Declaration[1].Usage = GGDECLUSAGE_NORMAL;
				Declaration[1].Type = GGDECLTYPE_FLOAT3;
				Declaration[1].Stream = 0;
				Declaration[1].Method = GGDECLMETHOD_DEFAULT;
				Declaration[1].UsageIndex = 0;
				Declaration[1].Offset = iByteOffset;
				iByteOffset += 12;
				Declaration[2].Usage = GGDECLUSAGE_TEXCOORD;
				Declaration[2].Type = GGDECLTYPE_FLOAT2;
				Declaration[2].Stream = 0;
				Declaration[2].Method = GGDECLMETHOD_DEFAULT;
				Declaration[2].UsageIndex = 0;
				Declaration[2].Offset = iByteOffset;
				iByteOffset += 8;
				Declaration[3].Usage = GGDECLUSAGE_TANGENT;
				Declaration[3].Type = GGDECLTYPE_FLOAT3;
				Declaration[3].Stream = 0;
				Declaration[3].Method = GGDECLMETHOD_DEFAULT;
				Declaration[3].UsageIndex = 0;
				Declaration[3].Offset = iByteOffset;
				iByteOffset += 12;
				Declaration[4].Usage = GGDECLUSAGE_BINORMAL;
				Declaration[4].Type = GGDECLTYPE_FLOAT3;
				Declaration[4].Stream = 0;
				Declaration[4].Method = GGDECLMETHOD_DEFAULT;
				Declaration[4].UsageIndex = 0;
				Declaration[4].Offset = iByteOffset;
				iByteOffset += 12;
				int iLastEntry = 5;
				if (pAssImpMesh->HasBones())
				{
					Declaration[5].Usage = GGDECLUSAGE_TEXCOORD;
					Declaration[5].Type = GGDECLTYPE_FLOAT4;
					Declaration[5].Stream = 0;
					Declaration[5].Method = GGDECLMETHOD_DEFAULT;
					Declaration[5].UsageIndex = 0;
					Declaration[5].Offset = iByteOffset;
					iByteOffset += 16;
					Declaration[6].Usage = GGDECLUSAGE_TEXCOORD;
					Declaration[6].Type = GGDECLTYPE_FLOAT4;
					Declaration[6].Stream = 0;
					Declaration[6].Method = GGDECLMETHOD_DEFAULT;
					Declaration[6].UsageIndex = 0;
					Declaration[6].Offset = iByteOffset;
					iByteOffset += 16;
					iLastEntry = 7;
				}
				Declaration[iLastEntry] = GGDECLEND;
				Declaration[iLastEntry].Stream = 255;
				Declaration[iLastEntry].Method = GGDECLMETHOD_DEFAULT;
				Declaration[iLastEntry].UsageIndex = 0;
				Declaration[iLastEntry].Offset = iByteOffset;
				dwNumBytesPerVertex = iByteOffset;

				// if the index buffer a 32 bit buffer?
				int* pRemapVertexIndex = NULL;
				bool bThisMeshHasNoIndexBufferIgnoreAssImpFaceData = false;
				if (iVertexCount > 0xFFFF) 
				{
					// LB: DBO mesh still uses WORD index references (16-bit only) - can review down the road
					// for now, convert to a VERTEX ONLY mesh and remove index buffer to allow model through
					// sprintf(sLoadAssImpObjectError, "Vertex size exceeds 16-bit.");
					// return false;
					int iNewVertexCount = 0;
					for (int f = 0; f < pAssImpMesh->mNumFaces; f++)
					{
						iNewVertexCount += pAssImpMesh->mFaces[f].mNumIndices;
					}				
					int iVertexIndex = 0;
					aiVector3D* pNewVertexData = new aiVector3D[iNewVertexCount];
					aiVector3D* pNewNormalsData = new aiVector3D[iNewVertexCount];
					aiVector3D* pNewTextureCoordsData = new aiVector3D[iNewVertexCount];
					aiVector3D* pNewBitangentsData = new aiVector3D[iNewVertexCount];
					aiVector3D* pNewTangentsData = new aiVector3D[iNewVertexCount];
					pRemapVertexIndex = new int[iNewVertexCount];
					for (int f = 0; f < pAssImpMesh->mNumFaces; f++)
					{
						if (pAssImpMesh->mFaces[f].mNumIndices == 3)
						{
							// triangle - flip order of indices for GG compatibility with DBO
							int iIndex0 = pAssImpMesh->mFaces[f].mIndices[0];
							int iIndex1 = pAssImpMesh->mFaces[f].mIndices[2];
							int iIndex2 = pAssImpMesh->mFaces[f].mIndices[1];
							SetNewVertData (pAssImpMesh, pRemapVertexIndex, pNewVertexData, pNewNormalsData, pNewTextureCoordsData, pNewBitangentsData, pNewTangentsData, iVertexIndex, iIndex0, iIndex1, iIndex2);
							iVertexIndex+=3;
						}
						else
						{
							// more than three sides - assume "fan" pattern
							int iIndex0 = pAssImpMesh->mFaces[f].mIndices[0];
							int iIndex1 = pAssImpMesh->mFaces[f].mIndices[2];
							int iIndex2 = pAssImpMesh->mFaces[f].mIndices[1];
							SetNewVertData (pAssImpMesh, pRemapVertexIndex, pNewVertexData, pNewNormalsData, pNewTextureCoordsData, pNewBitangentsData, pNewTangentsData, iVertexIndex, iIndex0, iIndex1, iIndex2);
							iVertexIndex+=3;
							for (int i = 3; i < pAssImpMesh->mFaces[f].mNumIndices; i++)
							{
								iIndex1 = iIndex2;
								iIndex2 = pAssImpMesh->mFaces[f].mIndices[1];
								SetNewVertData (pAssImpMesh, pRemapVertexIndex, pNewVertexData, pNewNormalsData, pNewTextureCoordsData, pNewBitangentsData, pNewTangentsData, iVertexIndex, iIndex0, iIndex1, iIndex2);
								iVertexIndex+=3;
							}
						}
					}

					// replace vertex data with larger vertex set (accounts for index buffer arrangement of faces)
					SAFE_DELETE_ARRAY(pAssImpMesh->mVertices); // ZJ: Something somewhere writes past mVertices and causes heap corruption.
					SAFE_DELETE_ARRAY(pAssImpMesh->mNormals);
					SAFE_DELETE_ARRAY(pAssImpMesh->mTextureCoords[0]);
					SAFE_DELETE_ARRAY(pAssImpMesh->mBitangents);
					SAFE_DELETE_ARRAY(pAssImpMesh->mTangents);
					pAssImpMesh->mNumVertices = iNewVertexCount;
					pAssImpMesh->mVertices = pNewVertexData;
					pAssImpMesh->mNormals = pNewNormalsData;
					pAssImpMesh->mTextureCoords[0] = pNewTextureCoordsData;
					pAssImpMesh->mBitangents = pNewBitangentsData;
					pAssImpMesh->mTangents = pNewTangentsData;
					iVertexCount = iNewVertexCount;

					// no index buffer, all ordered now inside vertex buffer as triangles
					bThisMeshHasNoIndexBufferIgnoreAssImpFaceData = true;
					iIndiceCount = 0;
				}
				if ( SetupMeshDeclarationData ( pDBOMesh, Declaration, dwNumBytesPerVertex, iVertexCount, iIndiceCount )==false )
				{
					RunTimeError ( RUNTIMEERROR_B3DMESHLOADFAILED );
					return false;
				}

				// get the offset map
				sOffsetMap	offsetMap;
				GetFVFOffsetMapFixedForBones ( pDBOMesh, &offsetMap );

				// Need to handle cases where nodes are scaled non-uniformly (uncommon but usually fbx)
				aiVector3D scaleModifier(1, 1, 1);
				if (!pScene->HasAnimations())
				{
					if (nodeScale.x != nodeScale.y || nodeScale.x != nodeScale.z)
						scaleModifier = aiVector3D(nodeScale);
				}

				// copy vertex data from assimp mesh to DBO mesh
				float* pVertex = (float*)pDBOMesh->pVertexData;
				for (int n = 0; n < iVertexCount; n++)
				{
					// pos
					*((float*)pVertex + offsetMap.dwX + (offsetMap.dwSize * n)) = pAssImpMesh->mVertices[n].x * modelScale * boneScale.x * scaleModifier.x;
					*((float*)pVertex + offsetMap.dwY + (offsetMap.dwSize * n)) = pAssImpMesh->mVertices[n].y * modelScale * boneScale.y * scaleModifier.y;
					*((float*)pVertex + offsetMap.dwZ + (offsetMap.dwSize * n)) = pAssImpMesh->mVertices[n].z * modelScale * boneScale.z * scaleModifier.z;
					// normal
					*((float*)pVertex + offsetMap.dwNX + (offsetMap.dwSize * n)) = pAssImpMesh->mNormals[n].x;
					*((float*)pVertex + offsetMap.dwNY + (offsetMap.dwSize * n)) = pAssImpMesh->mNormals[n].y;
					*((float*)pVertex + offsetMap.dwNZ + (offsetMap.dwSize * n)) = pAssImpMesh->mNormals[n].z;
					// texture
					if (pAssImpMesh->mTextureCoords[0])
					{
						*((float*)pVertex + offsetMap.dwTU[0] + (offsetMap.dwSize * n)) = pAssImpMesh->mTextureCoords[0][n].x;
						*((float*)pVertex + offsetMap.dwTV[0] + (offsetMap.dwSize * n)) = pAssImpMesh->mTextureCoords[0][n].y;
					}
					// binormal and tangents
					if (pAssImpMesh->mTangents)
					{
						// binorm
						*((float*)pVertex + offsetMap.dwTU[1] + (offsetMap.dwSize * n)) = pAssImpMesh->mBitangents[n].x;
						*((float*)pVertex + offsetMap.dwTV[1] + (offsetMap.dwSize * n)) = pAssImpMesh->mBitangents[n].y;
						*((float*)pVertex + offsetMap.dwTZ[1] + (offsetMap.dwSize * n)) = pAssImpMesh->mBitangents[n].z;
						// tangent
						*((float*)pVertex + offsetMap.dwTU[2] + (offsetMap.dwSize * n)) = pAssImpMesh->mTangents[n].x;
						*((float*)pVertex + offsetMap.dwTV[2] + (offsetMap.dwSize * n)) = pAssImpMesh->mTangents[n].y;
						*((float*)pVertex + offsetMap.dwTZ[2] + (offsetMap.dwSize * n)) = pAssImpMesh->mTangents[n].z;
					}
					else
					{
						// LB: AssImp generates tangents! Wicked needs the TANGENT and it calculates the BINORMAL inside the shader
						*((float*)pVertex + offsetMap.dwTU[1] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTV[1] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTZ[1] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTU[2] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTV[2] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTZ[2] + (offsetMap.dwSize * n)) = 0;
					}
					// weight and indice
					if (pAssImpMesh->HasBones())
					{
						// clear before fill in (below)
						*((float*)pVertex + offsetMap.dwTU[3] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTV[3] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTZ[3] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTW[3] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTU[4] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTV[4] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTZ[4] + (offsetMap.dwSize * n)) = 0;
						*((float*)pVertex + offsetMap.dwTW[4] + (offsetMap.dwSize * n)) = 0;
					}
				}
				if (pAssImpMesh->HasBones())
				{
					// array to count bone influence instances on each vertex (can only have four)
					unsigned char* pWeightCount = new unsigned char[iVertexCount]; // just to count the weights as we find them
					memset(pWeightCount, 0, iVertexCount);

					// go through all bones and copy weights and indices to mesh
					for (UINT b = 0; b < pAssImpMesh->mNumBones; b++)
					{
						// the bone index
						UINT boneIndex = b;

						// now apply data to all vertices that use this bone
						for (UINT w = 0; w < pAssImpMesh->mBones[b]->mNumWeights; w++)
						{
							// have a weight
							if (!pAssImpMesh->mBones[b]->mWeights)
								continue;

							// vertex with the weight (original vertex index)
							UINT vertexIndex = pAssImpMesh->mBones[b]->mWeights[w].mVertexId;

							// only allow up to 4 weights per vertex!
							if (pWeightCount[vertexIndex] >= 4)
							{
								//MessageBoxA(NULL, "Object %d has more than 4 bone weights per vertex, AGK only supports 4 weights, the rest will be ignored", "", MB_OK);
								//break;
							}
							else
							{
								// find data
								int iSlot = pWeightCount[vertexIndex];
								float fWeight = pAssImpMesh->mBones[b]->mWeights[w].mWeight;

								// if vertex buffer was expanded (above), we need to apply weights and bone indexes to ALL vertices 
								// spawned from the original vertex list
								if (pRemapVertexIndex)
								{
									// we created a remap array which kept a reference of the original vertex indices
									for ( int iNewVertexIndex = 0; iNewVertexIndex < iVertexCount; iNewVertexIndex++ )
									{
										if (pRemapVertexIndex[iNewVertexIndex] == vertexIndex)
										{
											// weights
											if (iSlot == 0) *((float*)pVertex + offsetMap.dwTU[3] + (offsetMap.dwSize * iNewVertexIndex)) = fWeight;
											if (iSlot == 1) *((float*)pVertex + offsetMap.dwTV[3] + (offsetMap.dwSize * iNewVertexIndex)) = fWeight;
											if (iSlot == 2) *((float*)pVertex + offsetMap.dwTZ[3] + (offsetMap.dwSize * iNewVertexIndex)) = fWeight;
											if (iSlot == 3) *((float*)pVertex + offsetMap.dwTW[3] + (offsetMap.dwSize * iNewVertexIndex)) = fWeight;
											// indices
											if (iSlot == 0) *((float*)pVertex + offsetMap.dwTU[4] + (offsetMap.dwSize * iNewVertexIndex)) = boneIndex;
											if (iSlot == 1) *((float*)pVertex + offsetMap.dwTV[4] + (offsetMap.dwSize * iNewVertexIndex)) = boneIndex;
											if (iSlot == 2) *((float*)pVertex + offsetMap.dwTZ[4] + (offsetMap.dwSize * iNewVertexIndex)) = boneIndex;
											if (iSlot == 3) *((float*)pVertex + offsetMap.dwTW[4] + (offsetMap.dwSize * iNewVertexIndex)) = boneIndex;
										}
									}
								}
								else
								{
									// weights
									if (iSlot == 0) *((float*)pVertex + offsetMap.dwTU[3] + (offsetMap.dwSize * vertexIndex)) = fWeight;
									if (iSlot == 1) *((float*)pVertex + offsetMap.dwTV[3] + (offsetMap.dwSize * vertexIndex)) = fWeight;
									if (iSlot == 2) *((float*)pVertex + offsetMap.dwTZ[3] + (offsetMap.dwSize * vertexIndex)) = fWeight;
									if (iSlot == 3) *((float*)pVertex + offsetMap.dwTW[3] + (offsetMap.dwSize * vertexIndex)) = fWeight;
									// indices
									if (iSlot == 0) *((float*)pVertex + offsetMap.dwTU[4] + (offsetMap.dwSize * vertexIndex)) = boneIndex;
									if (iSlot == 1) *((float*)pVertex + offsetMap.dwTV[4] + (offsetMap.dwSize * vertexIndex)) = boneIndex;
									if (iSlot == 2) *((float*)pVertex + offsetMap.dwTZ[4] + (offsetMap.dwSize * vertexIndex)) = boneIndex;
									if (iSlot == 3) *((float*)pVertex + offsetMap.dwTW[4] + (offsetMap.dwSize * vertexIndex)) = boneIndex;
								}
								// keep count of influences on vertex known
								pWeightCount[vertexIndex]++;
							}
						}
					}

					// free temp weight influence count array
					delete[] pWeightCount;
				}
				SAFE_DELETE(pRemapVertexIndex);
	
				// create indices from assimp mesh data
				if (bThisMeshHasNoIndexBufferIgnoreAssImpFaceData == false)
				{
					iIndiceCount = 0;
					for (int f = 0; f < pAssImpMesh->mNumFaces; f++)
					{
						if (pAssImpMesh->mFaces[f].mNumIndices == 3)
						{
							// flip order of indices for GG compatibility with DBO
							pDBOMesh->pIndices[iIndiceCount + 0] = pAssImpMesh->mFaces[f].mIndices[0];
							pDBOMesh->pIndices[iIndiceCount + 1] = pAssImpMesh->mFaces[f].mIndices[2];
							pDBOMesh->pIndices[iIndiceCount + 2] = pAssImpMesh->mFaces[f].mIndices[1];
						}
						else
						{
							for (int i = 0; i < pAssImpMesh->mFaces[f].mNumIndices; i++)
							{
								pDBOMesh->pIndices[iIndiceCount + i] = pAssImpMesh->mFaces[f].mIndices[i];
							}
						}
						iIndiceCount += pAssImpMesh->mFaces[f].mNumIndices;
					}
				}
	
				// setup mesh drawing properties
				pDBOMesh->iPrimitiveType   = GGPT_TRIANGLELIST;
				pDBOMesh->iDrawVertexCount = pDBOMesh->dwVertexCount;
				if ( bThisMeshHasNoIndexBufferIgnoreAssImpFaceData == false )
					pDBOMesh->iDrawPrimitives  = pDBOMesh->dwIndexCount / 3;
				else
					pDBOMesh->iDrawPrimitives  = pDBOMesh->dwVertexCount / 3;

				// assign material and textures to mesh (from model data)
				bool bScanForTextures = false;
				if (pScene->HasMaterials())
				{
					char cFindTexture[MAX_PATH];
					int iMatIndex = pAssImpMesh->mMaterialIndex;
					
					// check for GLTF transparency
					aiString alphaMode;
					aiReturn ret = pScene->mMaterials[ iMatIndex ]->Get( "$mat.gltf.alphaMode", 0, 0, alphaMode );
					if ( ret == aiReturn_SUCCESS && strcmp(alphaMode.C_Str(), "BLEND") == 0 ) pDBOMesh->bTransparency = true;

					if (sDiffuseTextureList[iMatIndex].size() == 0) 
					{
						bScanForTextures = true;
					}
					else 
					{
						pDBOMesh->pTextures = new sTexture[7];
						pDBOMesh->dwTextureCount = 7;
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_DIFFUSE].pName, "");
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName, "");
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, "");
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, "");
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, "");
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName, "");
						strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName, "");

						//PE: Many have a sub folder called textures , check here first.

						if ( *(sDiffuseTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_DIFFUSE].pName, sDiffuseTextureList[iMatIndex].c_str());
						}

						if ( *(sNormalsTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_NORMAL].pName, sNormalsTextureList[iMatIndex].c_str());
						}

						if ( *(sMetalnessTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_METALNESS].pName, sMetalnessTextureList[iMatIndex].c_str());
						}

						if ( *(sRoughnessTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_ROUGHNESS].pName, sRoughnessTextureList[iMatIndex].c_str());
						}

						if ( *(sAOTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_OCCLUSION].pName, sAOTextureList[iMatIndex].c_str());
						}

						if ( *(sEmissiveTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_EMISSIVE].pName, sEmissiveTextureList[iMatIndex].c_str());
						}

						if ( *(sSurfaceTextureList[iMatIndex].c_str()) )
						{
							strcpy(pDBOMesh->pTextures[GG_MESH_TEXTURE_SURFACE].pName, sSurfaceTextureList[iMatIndex].c_str());
						}
					}
				}
				else 
				{
					bScanForTextures = true;
				}
				if(bScanForTextures)
				{
					//PE: Try to locate from object name.
					char FileOnly[MAX_PATH];
					char cFindTexture[MAX_PATH];

					strcpy(FileOnly, pModelFileName);
					for (int i = strlen(pModelFileName); i > 0; i--) {
						if (FileOnly[i] == '.') {
							FileOnly[i] = 0;
							break;
						}

					}
					bool bFoundTexture = false;
					for (int i = 0 ; i < 5 ; i++ )
					{
						char ext[MAX_PATH];
						char middle[MAX_PATH];
						if (i == 0) strcpy(ext, ".png");
						if (i == 1) strcpy(ext, ".jpg");
						if (i == 2) strcpy(ext, ".dds");
						if (i == 3) strcpy(ext, ".jpeg");
						if (i == 4) strcpy(ext, ".tga");
						for (int b = 0; b < 5; b++) 
						{
							if (b == 0) strcpy(middle, "_diff");
							if (b == 1) strcpy(middle, "_color");
							if (b == 2) strcpy(middle, "_d");
							if (b == 3) strcpy(middle, "");
							if (b == 4) strcpy(middle, "_dif");
							strcpy(cFindTexture, pModelPath);
							strcat(cFindTexture, FileOnly);
							strcat(cFindTexture, middle);
							strcat(cFindTexture, ext);
							if (LocateTexture(cFindTexture)) 
							{
								pDBOMesh->pTextures = new sTexture[1];
								pDBOMesh->dwTextureCount = 1;
								strcpy(pDBOMesh->pTextures[0].pName, cFindTexture);
								bFoundTexture = true;
								break;
							}
						}
						if (bFoundTexture)
							break;
					}
				}

				// keep track
				count++;
			}
		}
	} // end of nodeList

	/*
	// bone meshes will all belong to the root object, add them now that we know about the bones
	int iBoneMeshes = 0;
	for( int i = 0; i < pScene->mNumMeshes; i++ )
	{
		if ( pScene->mMeshes[i]->HasBones() ) 
		{
			iBoneMeshes++;
			for( int b = 0; b < pScene->mMeshes[i]->mNumBones; b++ )
			{
				aiVector3D pos, scale;
				aiQuaternion rot;
				pScene->mMeshes[i]->mBones[b]->mOffsetMatrix.Decompose( scale, rot, pos );
				if ( (fabs(scale.x-1) > 0.01f || fabs(scale.y-1) > 0.01f || fabs(scale.z-1) > 0.01f) )
				{
					//sprintf (sLoadAssImpObjectError, "Bone offset matrix contains scale values, it may not display correctly" );
				}
			}
		}
	}
	*/

	// load animations
	if ( pScene->HasAnimations() )
	{
		bool bModelMeshesNeedToBeLocalized = false;
		sAnimationSet* pLatestAnimSet = NULL;
		int iNumAnims = pScene->mNumAnimations;
		for (int a = 0; a < iNumAnims; a++)
		{
			// this assimp scene animation
			aiAnimation* pAssImpAnim = pScene->mAnimations[a];

			// work out milliseconds divide from FPS (so can align to how DBO animation speed works)
			//double dMillisecondDivide = 1.0f;
			//if ( pAssImpAnim->mTicksPerSecond > 0 ) dMillisecondDivide = 1000.0f / pAssImpAnim->mTicksPerSecond;
			//dMillisecondDivide = 1000.0f / 30.0f;
			//if (pAssImpAnim->mTicksPerSecond > 0)
			//{
			//	double dDBOMAXSpeed = pAssImpAnim->mTicksPerSecond / 30.0f; // 30fps = 1.0f  1000fps = 33.33f
			//	fGlobalAnimSpeedFromModel = ceil(()*dMillisecondDivide);
			//}
			// LB: To keep conpatibility with old DBO anim system and speeds, see WickedCall_SetObjectSpeed for speed*50 hack
			// so because of this imported model animation speeds need to be halved so they import as expected, and match DBOMAX old anim speeds
			pObject->fAnimSpeed = 0.5f;			

			// LB: As it happens, ASSIMP always exports animation keyframe times at 33.33ms and mTicksPerSecond seems to set the wrong speed!
			double dMillisecondDivide = 1000.0 / 30.0;
			if (pAssImpAnim->mTicksPerSecond == 0.0f)
			{
				// if from X File Import (pAssImpAnim->mTicksPerSecond=0), then we should NOT divide frame count and
				// anim time references by 33ms. Leave as is.
				dMillisecondDivide = 1.0;
			}

			// create DBO animset and associated anims
			sAnimationSet* pAnimSet = new sAnimationSet();
			strcpy(pAnimSet->szName, pAssImpAnim->mName.C_Str());
			if (pObject->pAnimationSet == NULL)
				pObject->pAnimationSet = pAnimSet;
			else
				pLatestAnimSet->pNext = pAnimSet;
			pLatestAnimSet = pAnimSet;
			pAnimSet->ulLength = pAssImpAnim->mDuration / dMillisecondDivide;
			sAnimation* pLatestAnim = NULL;

			// handle possible complex chains
			bool bBuildComplexChain = false;
			sAnimation* pBuildComplexChainItem = NULL;
			char pBuildComplexChainName[MAX_PATH];
			strcpy(pBuildComplexChainName, "");

			// go through all anim channels for this mesh
			for (int iBoneAnim = 0; iBoneAnim < pAssImpAnim->mNumChannels; iBoneAnim++)
			{
				// LB: turns out some FBX animations can have complex node chains (ie. multiple channels referring to same frame)
				// and if we combined them, the final position of frames is wrong, NOT SURE WHY THE RESULT DOES NOT ANIMATE PROPERLY!
				bool bContainsTranslation = true;
				bool bContainsScaling = true;
				bool bContainsRotation = true;
				LPSTR pThisChannelName = (char *)pAssImpAnim->mChannels[iBoneAnim]->mNodeName.C_Str();
				#define MAGIC_NODE_TAG "_$AssimpFbx$"
				char pNewTruncatedName[MAX_PATH];
				strcpy(pNewTruncatedName, "");
				if (strstr(pThisChannelName, MAGIC_NODE_TAG) != NULL)
				{
					// this is a complex chain node
					// and the animation data held looks like world offsets from the models center
					// so we need to ENSURE the mesh data is transformed into local space so the
					// animations will look correct (ran out of time for Friday build)
					bModelMeshesNeedToBeLocalized = true;

					// pull animation data from AssImp modified anim nodes
					bContainsTranslation = false;
					bContainsRotation = false;
					bContainsScaling = false;
					if (strnicmp (pThisChannelName + strlen(pThisChannelName) - 12, "_Translation", 12) == NULL)
					{
						strcpy (pNewTruncatedName, pThisChannelName);
						pNewTruncatedName[strlen(pNewTruncatedName)-strlen(MAGIC_NODE_TAG)-12] = 0;
						bContainsTranslation = true;
					}
					if (strnicmp (pThisChannelName + strlen(pThisChannelName) - 9, "_Rotation", 9) == NULL)
					{
						strcpy (pNewTruncatedName, pThisChannelName);
						pNewTruncatedName[strlen(pNewTruncatedName)-strlen(MAGIC_NODE_TAG)-9] = 0;
						bContainsRotation = true;
					}
					if (strnicmp (pThisChannelName + strlen(pThisChannelName) - 8, "_Scaling", 8) == NULL)
					{
						strcpy (pNewTruncatedName, pThisChannelName);
						pNewTruncatedName[strlen(pNewTruncatedName)-strlen(MAGIC_NODE_TAG)-8] = 0;
						bContainsScaling = true;
					}
					// just allow this one through so I can compare!
					if (pBuildComplexChainItem != NULL)
					{
						if (strcmp (pNewTruncatedName, pBuildComplexChainName) != NULL)
						{
							strcpy (pBuildComplexChainName, "");
							pBuildComplexChainItem = NULL;
							bBuildComplexChain = false;
						}
					}
					if (bBuildComplexChain == false)
					{
						strcpy (pBuildComplexChainName, pNewTruncatedName);
						bBuildComplexChain = true;
					}

					// use truncated name for scanning for frame ptr
					pThisChannelName = pNewTruncatedName;
				}
				else
				{
					// resume regular anim nodes
					strcpy (pBuildComplexChainName, "");
					pBuildComplexChainItem = NULL;
					bBuildComplexChain = false;
				}

				// proceed to try and find node name in frame hierarchy
				sFrame* pAnimFrame = FindFrame( pThisChannelName, pObject->pFrame);
				if (!pAnimFrame)
				{
					//Must exists.
					continue;
				}

				// create new anim to hold frame-specific bone anim
				sAnimation* pUseThisAnim = NULL;
				if (pBuildComplexChainItem == NULL)
				{
					sAnimation* pAnim = new sAnimation;
					strcpy(pAnim->szName, pThisChannelName);
					pAnim->dwNumPositionKeys = 0;
					pAnim->dwNumRotateKeys = 0;
					pAnim->dwNumScaleKeys = 0;
					if (pAnimSet->pAnimation == NULL)
						pAnimSet->pAnimation = pAnim;
					else
						pLatestAnim->pNext = pAnim;
					pLatestAnim = pAnim;
					if (bBuildComplexChain == true) pBuildComplexChainItem = pAnim;
					pUseThisAnim = pAnim;
				}
				else
				{
					pUseThisAnim = pBuildComplexChainItem;
				}

				// populate with time stamp and data
				if (bContainsTranslation == true)
				{
					pUseThisAnim->dwNumPositionKeys = pAssImpAnim->mChannels[iBoneAnim]->mNumPositionKeys;
					if (pUseThisAnim->dwNumPositionKeys > 0)
					{
						pUseThisAnim->pPositionKeys = new sPositionKey[pUseThisAnim->dwNumPositionKeys];
						for (int j = 0; j < pUseThisAnim->dwNumPositionKeys; j++)
						{
							pUseThisAnim->pPositionKeys[j].dwTime = (float)(pAssImpAnim->mChannels[iBoneAnim]->mPositionKeys[j].mTime/dMillisecondDivide);
							pUseThisAnim->pPositionKeys[j].vecPos.x = (pAssImpAnim->mChannels[iBoneAnim]->mPositionKeys[j].mValue.x) * modelScale;
							pUseThisAnim->pPositionKeys[j].vecPos.y = (pAssImpAnim->mChannels[iBoneAnim]->mPositionKeys[j].mValue.y) * modelScale;
							pUseThisAnim->pPositionKeys[j].vecPos.z = (pAssImpAnim->mChannels[iBoneAnim]->mPositionKeys[j].mValue.z) * modelScale;
						}
					}
				}
				if (bContainsRotation == true)
				{
					pUseThisAnim->dwNumRotateKeys = pAssImpAnim->mChannels[iBoneAnim]->mNumRotationKeys;
					if (pUseThisAnim->dwNumRotateKeys > 0)
					{
						pUseThisAnim->pRotateKeys = new sRotateKey[pUseThisAnim->dwNumRotateKeys];
						for (int j = 0; j < pUseThisAnim->dwNumRotateKeys; j++)
						{
							pUseThisAnim->pRotateKeys[j].dwTime = (float)(pAssImpAnim->mChannels[iBoneAnim]->mRotationKeys[j].mTime/dMillisecondDivide);
							float fQx = pAssImpAnim->mChannels[iBoneAnim]->mRotationKeys[j].mValue.x;
							float fQy = pAssImpAnim->mChannels[iBoneAnim]->mRotationKeys[j].mValue.y;
							float fQz = pAssImpAnim->mChannels[iBoneAnim]->mRotationKeys[j].mValue.z;
							float fQw = pAssImpAnim->mChannels[iBoneAnim]->mRotationKeys[j].mValue.w;
							pUseThisAnim->pRotateKeys[j].Quaternion.x = fQx;
							pUseThisAnim->pRotateKeys[j].Quaternion.y = fQy;
							pUseThisAnim->pRotateKeys[j].Quaternion.z = fQz;
							pUseThisAnim->pRotateKeys[j].Quaternion.w = fQw;
						}
					}
				}
				if (bContainsScaling == true)
				{
					pUseThisAnim->dwNumScaleKeys = pAssImpAnim->mChannels[iBoneAnim]->mNumScalingKeys;
					if (pUseThisAnim->dwNumScaleKeys > 0)
					{
						pUseThisAnim->pScaleKeys = new sScaleKey[pUseThisAnim->dwNumScaleKeys];
						for (int j = 0; j < pUseThisAnim->dwNumScaleKeys; j++)
						{
							pUseThisAnim->pScaleKeys[j].dwTime = (float)(pAssImpAnim->mChannels[iBoneAnim]->mScalingKeys[j].mTime/dMillisecondDivide);
							if (pAnimFrame->bRemovedScale)
							{
								pUseThisAnim->pScaleKeys[j].vecScale.x = 1.0f;
								pUseThisAnim->pScaleKeys[j].vecScale.y = 1.0f;
								pUseThisAnim->pScaleKeys[j].vecScale.z = 1.0f;
							}
							else
							{
								pUseThisAnim->pScaleKeys[j].vecScale.x = pAssImpAnim->mChannels[iBoneAnim]->mScalingKeys[j].mValue.x;
								pUseThisAnim->pScaleKeys[j].vecScale.y = pAssImpAnim->mChannels[iBoneAnim]->mScalingKeys[j].mValue.y;
								pUseThisAnim->pScaleKeys[j].vecScale.z = pAssImpAnim->mChannels[iBoneAnim]->mScalingKeys[j].mValue.z;
							}
						}
					}
				}
			}
		}
	}
		
	// finished with assimp data
	aiReleaseImport(pScene);

	// success
	return true;
}

void LoadBones(const aiMesh* pMesh)
{
	m_BoneMapping.clear();
	m_BoneInfo.clear();
	m_NumBones = 0;

	for (int i = 0; i < pMesh->mNumBones; i++) {
		int BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}
		m_BoneMapping[BoneName] = BoneIndex;
		m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
	}
}

const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void InitScaleTransform(aiMatrix4x4 &transform , float ScaleX, float ScaleY, float ScaleZ)
{
	transform[0][0] = ScaleX; transform[0][1] = 0.0f;   transform[0][2] = 0.0f;   transform[0][3] = 0.0f;
	transform[1][0] = 0.0f;   transform[1][1] = ScaleY; transform[1][2] = 0.0f;   transform[1][3] = 0.0f;
	transform[2][0] = 0.0f;   transform[2][1] = 0.0f;   transform[2][2] = ScaleZ; transform[2][3] = 0.0f;
	transform[3][0] = 0.0f;   transform[3][1] = 0.0f;   transform[3][2] = 0.0f;   transform[3][3] = 1.0f;
}

void InitTranslationTransform(aiMatrix4x4 &m,float x, float y, float z)
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


aiMatrix4x4 CalculateGlobalTransform(const aiNode* pNode, const aiNode* pNodeParent )
{
	// concatenate all parent transforms to get the global transform for this node
	aiMatrix4x4 mGlobalTransform = pNode->mTransformation;
	const aiNode * node = pNodeParent;
	while (node) {
		mGlobalTransform = node->mTransformation * mGlobalTransform;
		node = node->mParent;
	}
	return(mGlobalTransform);
}

void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4 & ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	aiMatrix4x4 BindPose;
	bool bFoundAnim = false;
	if (pNodeAnim) {

		aiVector3D pos = pNodeAnim->mPositionKeys[0].mValue;
		aiVector3D scale = pNodeAnim->mScalingKeys[0].mValue;
		aiQuaternion rot = pNodeAnim->mRotationKeys[0].mValue;
		BindPose = aiMatrix4x4(scale, rot, pos);

		aiVector3D Scaling = scale;
		aiMatrix4x4 ScalingM;
		InitScaleTransform(ScalingM, Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ = rot;
		aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation = pos;
		aiMatrix4x4 TranslationM;
		InitTranslationTransform(TranslationM, Translation.x, Translation.y, Translation.z);

		// Combine the above transformations
		//NodeTransformation = TranslationM * RotationM * ScalingM;


		bFoundAnim = true;
	}

	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		int BoneIndex = m_BoneMapping[NodeName];


		m_BoneInfo[BoneIndex].GlobalTransform = CalculateGlobalTransform(pNode, pNode->mParent);


		//m_GlobalInverseTransform to inverse(parentTransform)

//		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation *
//			m_BoneInfo[BoneIndex].BoneOffset;

		aiMatrix4x4 m_ParentInverseTransform = ParentTransform;
		m_ParentInverseTransform.Inverse();
//		m_BoneInfo[BoneIndex].FinalTransformation = m_ParentInverseTransform * GlobalTransformation *
//			m_BoneInfo[BoneIndex].BoneOffset;

		//m_ArmatureInverseTransform
//		m_BoneInfo[BoneIndex].FinalTransformation = m_ArmatureInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		//BindPose

//		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;

		m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].GlobalTransform;

		m_BoneInfo[BoneIndex].NodeTransformation = NodeTransformation;
		m_BoneInfo[BoneIndex].bNodeFound = bFoundAnim;
	}

	for (int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

#else

bool LoadAssImpObject(char* pModelFilename, sObject** ppObject, enumScalingMode eScalingMode)
{
	return false;
}

#endif