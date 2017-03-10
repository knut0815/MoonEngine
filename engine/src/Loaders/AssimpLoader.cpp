
#include "AssimpLoader.h"
#include "Util/GLMUtil.h"
using namespace MoonEngine;


std::string textureStringForAssimpType(const aiTextureType & type)
{
	std::string typeString;
	switch(type)
	{
		case aiTextureType_DIFFUSE:
		typeString = "diffuse";
		break;
		case aiTextureType_SPECULAR:
		typeString = "specular";
		break;
		case aiTextureType_AMBIENT:
		typeString = "ambient";
		break;
		case aiTextureType_EMISSIVE:
		typeString = "emmission";
		break;
		case aiTextureType_NORMALS:
		typeString = "normal";
		break;
		case aiTextureType_SHININESS:
		typeString = "shine";
		break;
		default:
		LOG(ERROR, "Loading unknown texture type");
		typeString = "unknown";
		break;
	}
	return typeString;
}
void loadMaterialTextures(
	aiMaterial * mat, aiTextureType type, std::unordered_map<std::string, std::string> * textures)
{
	
	std::string texType = textureStringForAssimpType(type);
	if(mat->GetTextureCount(type) > 1)
	{
		LOG(WARN, "Unsupported engine feature, multiple textures of type");
	}
	//This for-loop isn't needed yet but we might support multiple textures per object later.
	for(GLuint i = 0; i < std::min((unsigned)1,mat->GetTextureCount(type)); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string texString =  std::string(str.C_Str());
		(*textures)[texType] = texString;
	}
	
}

bool processMesh(aiMesh * mesh, 
	aiMatrix4x4 transform,
	const aiScene * scene,
	std::vector<float> & data, 
	std::vector<unsigned short> & indices,
	AssimpModelInfo * outInfo)
{
	AssimpMeshInfo assimpMeshInfo = AssimpMeshInfo();
	unsigned baseVertex = data.size() / outInfo->stride();
	assimpMeshInfo.meshInfo.baseVertex = baseVertex;
	int baseIndex = indices.size();
	assimpMeshInfo.meshInfo.indexDataOffset = (GLvoid *)(indices.size() * sizeof(unsigned short));
	float minX = 1e20, maxX = -1e20; 
	float minY = 1e20, maxY = -1e20; 
	float minZ = 1e20, maxZ = -1e20;
	
	glm::mat4 M = GLMUtil::FromAssimp(transform);
	//data.reserve(mesh->mNumVertices * 5);
	for(GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec4 vert = M * glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,1);

		data.push_back(vert.x);
		data.push_back(vert.y);
		data.push_back(vert.z);
		minX = std::min(minX,vert.x);
		maxX = std::max(maxX,vert.x);
		minY = std::min(minY,vert.y);
		maxY = std::max(maxY,vert.y);
		minZ = std::min(minZ,vert.z);
		maxZ = std::max(maxZ,vert.z);
		
		if(outInfo->hasNormals())
		{
			if(mesh->HasNormals())
			{
				glm::vec4 nor = glm::transpose(glm::inverse(M)) * 
					glm::vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z,0);
				data.push_back(nor.x);
				data.push_back(nor.y);
				data.push_back(nor.z);				
			}
			else
			{
				LOG(WARN, "Mesh loaded with normals requested has no normals");
				data.push_back(0);
				data.push_back(0);
				data.push_back(0);	
			}

		}

		if(outInfo->hasTextureCoordinates())
		{
	      	if(mesh->HasTextureCoords(0)) // Does the mesh contain texture coordinates?
	      	{

	      		data.push_back(mesh->mTextureCoords[0][i].x);
	      		data.push_back(mesh->mTextureCoords[0][i].y);

	      	}
	      	else
	      	{
	      		LOG(WARN, "Mesh loaded with texture coordinates requested has no texture coordinates");
	      		data.push_back(0);
	      		data.push_back(0);
	      	}

	      }

	      if(outInfo->hasTangentBitangent())
	      {
	      	if(mesh->HasTangentsAndBitangents())
	      	{
	      		data.push_back(mesh->mTangents[i].x);
	      		data.push_back(mesh->mTangents[i].y);
	      		data.push_back(mesh->mTangents[i].z);

	      		data.push_back(mesh->mBitangents[i].x);
	      		data.push_back(mesh->mBitangents[i].y);
	      		data.push_back(mesh->mBitangents[i].z);


	      	}
	      	else
	      	{
	      		LOG(WARN, "Mesh loaded with tangents and bitangents requested has neither");
	      		data.push_back(0);
	      		data.push_back(0);
	      		data.push_back(0);

	      		data.push_back(0);
	      		data.push_back(0);
	      		data.push_back(0);
	      	}
	      }


	  }
	  for(GLuint i = 0; i < mesh->mNumFaces; i++)
	  {
	  	aiFace face = mesh->mFaces[i];
	  	for(GLuint j = 0; j < face.mNumIndices; j++)
	  		indices.push_back(face.mIndices[j]);
	  }
	  assimpMeshInfo.meshInfo.numTris = (indices.size() - baseIndex);
	  assimpMeshInfo.meshInfo.numVerts = data.size()/outInfo->stride() - baseVertex;
	  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	  loadMaterialTextures(material, aiTextureType_DIFFUSE, &(assimpMeshInfo.textures));
	  loadMaterialTextures(material, aiTextureType_SPECULAR, &(assimpMeshInfo.textures));
	  loadMaterialTextures(material, aiTextureType_NORMALS, &(assimpMeshInfo.textures));
	  assimpMeshInfo.box = BoundingBox(minX,maxX,minY,maxY,minZ,maxZ);
	  
	  outInfo->addMeshInfo(assimpMeshInfo);

	  return true;
	}

bool processNode(aiNode * node, aiMatrix4x4 transform, const aiScene * scene, std::vector<float> & data, std::vector<unsigned short> & indices, AssimpModelInfo * outInfo)
{

	aiMatrix4x4 trans = transform * node->mTransformation;
	bool OK = true;
	if(node != nullptr)
	{
		for(uint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
			OK &= processMesh(mesh, trans, scene, data, indices, outInfo);
		}
		for(int i = 0; i < node->mNumChildren; i++)
		{
			OK &= processNode(node->mChildren[i], trans, scene, data, indices, outInfo);
		}
		return OK;
	}
	return false;
}

bool AssimpLoader::LoadIntoBuffer(
		std::string fileName,
		GLBuffer * vertexBuffer,
		GLBuffer * indexBuffer,
		GLVertexArrayObject * vao,
		AssimpModelInfo * outInfo,
		unsigned int pFlags,
		bool smooth)
{
	
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(fileName, pFlags | aiProcess_Triangulate);
	bool OK = true;
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOG(ERROR, "Could not load assimp scene: " + std::string(importer.GetErrorString()));	
		OK = false;
	}
	std::vector<float> data;
	std::vector<unsigned short> indices;
	OK = processNode(scene->mRootNode, aiMatrix4x4(), scene, data, indices, outInfo);
	if(!OK)
	{
		LOG(ERROR, "Issue loading mesh");
		return false;
	}
	vertexBuffer->setData(sizeof(float)*data.size(), &data[0], GL_STATIC_DRAW);
	indexBuffer->setData(sizeof(unsigned short) * indices.size(), &indices[0], GL_STATIC_DRAW);
		//Configure VAO
	int stride = sizeof(float)*outInfo->stride();
	int texCoordsOffset  = 0;
	int tangentOffset = 0;
	int bitangentOffset = 0;
	vao->bindVertexBuffer(
		GL_VERTEX_POSITION_ATTRIBUTE,
		*vertexBuffer,
		3,
		GL_FLOAT,
		false,
		stride
		);
	if (outInfo->hasNormals())
	{
		LOG(INFO, "Loading mesh with normals");
		texCoordsOffset += sizeof(float) * 3;
		tangentOffset += sizeof(float) * 3;
		bitangentOffset += sizeof(float) * 3;
		vao->bindVertexBuffer(
			GL_VERTEX_NORMAL_ATTRIBUTE,
			*vertexBuffer,
			3,
			GL_FLOAT,
			false,
			stride,
			(GLvoid *) (sizeof(float) * 3)
			);
	}
	if (outInfo->hasTextureCoordinates())
	{
		LOG(INFO, "Loading mesh with textures");
		tangentOffset += sizeof(float) * 2;
		bitangentOffset += sizeof(float) * 2;
		vao->bindVertexBuffer(
			GL_VERTEX_TEXTURE_ATTRIBUTE,
			*vertexBuffer,
			2,
			GL_FLOAT,
			false,
			stride,
			(GLvoid *) texCoordsOffset
			);
	}
	if(outInfo->hasTangentBitangent())
	{
		LOG(INFO, "Loading mesh with Tangents and Bitangents");
		bitangentOffset += sizeof(float) * 3;
		vao->bindVertexBuffer(
			GL_VERTEX_TANGENT_ATTRIBUTE,
			*vertexBuffer,
			3,
			GL_FLOAT,
			false,
			stride,
			(GLvoid *) tangentOffset
			);
		vao->bindVertexBuffer(
			GL_VERTEX_BITANGENT_ATTRIBUTE,
			*vertexBuffer,
			3,
			GL_FLOAT,
			false,
			stride,
			(GLvoid *) bitangentOffset
			);

	}
	vao->bindElementBuffer(*indexBuffer);
	outInfo->setVertexArrayObject(vao);

	

	return true;

}
