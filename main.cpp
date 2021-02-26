#include<iostream>
#include<string>

#include<assimp/Importer.hpp>
#include<assimp/Exporter.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<assimp/version.h>

#include"cxxopts.hpp"

const std::string VERSION_STR = "Model Converter v0.2.0";

int ConvertModelFormat(
	const std::string& inputFilepath,
	const std::string& outputFilepath,
	unsigned int importerFlags) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(inputFilepath, importerFlags);
	if (!scene) {
		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
		return -1;
	}

	//Get the file extension and determine the output format
	size_t lastIndexOfDot = outputFilepath.find_last_of('.');
	if (lastIndexOfDot == std::string::npos) {
		std::cerr << "Error: Cannot determine the output format from the filepath specified" << std::endl;
		return -1;
	}

	auto extension = outputFilepath.substr(lastIndexOfDot + 1);

	Assimp::Exporter exporter;
	size_t numExporters = exporter.GetExportFormatCount();
	std::string formatID = "";
	for (int i = 0; i < numExporters; i++) {
		const aiExportFormatDesc* format = exporter.GetExportFormatDescription(i);
		if (format->fileExtension == extension) {
			formatID = format->id;
			break;
		}
	}
	if (formatID == "") {
		std::cerr << "Error: Unsupported output format" << std::endl;
		return -1;
	}

	aiReturn ret = exporter.Export(scene, formatID, outputFilepath, scene->mFlags);
	if (ret == aiReturn_FAILURE) {
		std::cerr << "Error: " << exporter.GetErrorString() << std::endl;
		return -1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		return 0;
	}

	cxxopts::Options options("Model Converter", "Converts model format with Assimp");
	options.add_options()
		//General
		("i,inputFilepath", "Input filepath", cxxopts::value<std::string>())
		("o,outputFilepath", "Output filepath (must contain extension)", cxxopts::value<std::string>())
		("h,help", "Displays help")
		("v,version", "Displays version")
		//Assimp options
		("aiProcess_CalcTangentSpace", "Calculates the tangents and bitangents for the imported meshes")
		("aiProcess_JoinIdenticalVertices", "Identifies and joins identical vertex data sets within all imported meshes")
		("aiProcess_MakeLeftHanded", "Converts all the imported data to a left-handed coordinate space")
		("aiProcess_Triangulate", "Triangulates all faces of all meshes")
		("aiProcess_RemoveComponent", "Removes some parts of the data structure (animations, materials, light sources, cameras, textures, vertex components)")
		("aiProcess_GenNormals", "Generates normals for all faces of all meshes")
		("aiProcess_GenSmoothNormals", "Generates smooth normals for all vertices in the mesh")
		("aiProcess_SplitLargeMeshes", "Splits large meshes into smaller sub-meshes")
		("aiProcess_PreTransformVertices", "Removes the node graph and pre-transforms all vertices with the local transformation matrices of their nodes")
		("aiProcess_LimitBoneWeights", "Limits the number of bones simultaneously affecting a single vertex to a maximum value")
		("aiProcess_ValidateDataStructure", "Validates the imported scene data structure")
		("aiProcess_ImproveCacheLocality", "Reorders triangles for better vertex cache locality")
		("aiProcess_RemoveRedundantMaterials", "Searches for redundant/unreferenced materials and removes them")
		("aiProcess_FixInfacingNormals", "This step tries to determine which meshes have normal vectors that are facing inwards and inverts them")
		("aiProcess_SortByPType", "This step splits meshes with more than one primitive type in homogeneous sub-meshes")
		("aiProcess_FindDegenerates", "This step searches all meshes for degenerate primitives and converts them to proper lines or points")
		("aiProcess_FindInvalidData", "This step searches all meshes for invalid data, such as zeroed normal vectors or invalid UV coords and removes/fixes them")
		("aiProcess_GenUVCoords", "This step converts non-UV mappings (such as spherical or cylindrical mapping) to proper texture coordinate channels")
		("aiProcess_TransformUVCoords", "This step applies per-texture UV transformations and bakes them into stand-alone vtexture coordinate channels")
		("aiProcess_FindInstances", "This step searches for duplicate meshes and replaces them with references to the first mesh")
		("aiProcess_OptimizeMeshes", "A postprocessing step to reduce the number of meshes")
		("aiProcess_OptimizeGraph", "A postprocessing step to optimize the scene hierarchy")
		("aiProcess_FlipUVs", "This step flips all UV coordinates along the y-axis and adjusts material settings and bitangents accordingly")
		("aiProcess_FlipWindingOrder", "This step adjusts the output face winding order to be CW")
		("aiProcess_SplitByBoneCount", "This step splits meshes with many bones into sub-meshes so that each su-bmesh has fewer or as many bones as a given limit")
		("aiProcess_Debone", "This step removes bones losslessly or according to some threshold")
		//Assimp macros
		("aiProcessPreset_TargetRealtime_Fast", "")
		("aiProcessPreset_TargetRealtime_MaxQuality", "")
		("aiProcessPreset_TargetRealtime_Quality", "")
		;
	auto result = options.parse(argc, argv);

	if (result.count("help") != 0) {
		std::cout << options.help() << std::endl;
		return 0;
	}
	if (result.count("version") != 0) {
		//Version of Model Converter
		std::cout << VERSION_STR << std::endl;
		std::cout << std::endl;
		//Version of Assimp
		std::cout << "Assimp v" << aiGetVersionMajor() << "." << aiGetVersionMinor() << "." << aiGetVersionRevision() << std::endl;
		std::cout << std::endl;
		//Legal string of Assimp
		std::cout << aiGetLegalString() << std::endl;

		return 0;
	}

	if (result.count("inputFilepath") == 0) {
		std::cerr << "Error: You must specify input filepath" << std::endl;
		return -1;
	}
	if (result.count("outputFilepath") == 0) {
		std::cerr << "Error: You must specify output filepath" << std::endl;
		return -1;
	}

	auto inputFilepath = result["inputFilepath"].as<std::string>();
	auto outputFilepath = result["outputFilepath"].as<std::string>();

	unsigned int readFileOptions = 0;
	//Assimp options
	if (result.count("aiProcess_CalcTangentSpace") != 0) {
		readFileOptions |= aiProcess_CalcTangentSpace;
	}
	if (result.count("aiProcess_JoinIdenticalVertices") != 0) {
		readFileOptions |= aiProcess_JoinIdenticalVertices;
	}
	if (result.count("aiProcess_MakeLeftHanded") != 0) {
		readFileOptions |= aiProcess_MakeLeftHanded;
	}
	if (result.count("aiProcess_Triangulate") != 0) {
		readFileOptions |= aiProcess_Triangulate;
	}
	if (result.count("aiProcess_RemoveComponent") != 0) {
		readFileOptions |= aiProcess_RemoveComponent;
	}
	if (result.count("aiProcess_GenNormals") != 0) {
		readFileOptions |= aiProcess_GenNormals;
	}
	if (result.count("aiProcess_GenSmoothNormals") != 0) {
		readFileOptions |= aiProcess_GenSmoothNormals;
	}
	if (result.count("aiProcess_SplitLargeMeshes") != 0) {
		readFileOptions |= aiProcess_SplitLargeMeshes;
	}
	if (result.count("aiProcess_PreTransformVertices") != 0) {
		readFileOptions |= aiProcess_PreTransformVertices;
	}
	if (result.count("aiProcess_LimitBoneWeights") != 0) {
		readFileOptions |= aiProcess_LimitBoneWeights;
	}
	if (result.count("aiProcess_ValidateDataStructure") != 0) {
		readFileOptions |= aiProcess_ValidateDataStructure;
	}
	if (result.count("aiProcess_ImproveCacheLocality") != 0) {
		readFileOptions |= aiProcess_ImproveCacheLocality;
	}
	if (result.count("aiProcess_RemoveRedundantMaterials") != 0) {
		readFileOptions |= aiProcess_RemoveRedundantMaterials;
	}
	if (result.count("aiProcess_FixInfacingNormals") != 0) {
		readFileOptions |= aiProcess_FixInfacingNormals;
	}
	if (result.count("aiProcess_SortByPType") != 0) {
		readFileOptions |= aiProcess_SortByPType;
	}
	if (result.count("aiProcess_FindDegenerates") != 0) {
		readFileOptions |= aiProcess_FindDegenerates;
	}
	if (result.count("aiProcess_FindInvalidData") != 0) {
		readFileOptions |= aiProcess_FindInvalidData;
	}
	if (result.count("aiProcess_GenUVCoords") != 0) {
		readFileOptions |= aiProcess_GenUVCoords;
	}
	if (result.count("aiProcess_TransformUVCoords") != 0) {
		readFileOptions |= aiProcess_TransformUVCoords;
	}
	if (result.count("aiProcess_FindInstances") != 0) {
		readFileOptions |= aiProcess_FindInstances;
	}
	if (result.count("aiProcess_OptimizeMeshes") != 0) {
		readFileOptions |= aiProcess_OptimizeMeshes;
	}
	if (result.count("aiProcess_OptimizeGraph") != 0) {
		readFileOptions |= aiProcess_OptimizeGraph;
	}
	if (result.count("aiProcess_FlipUVs") != 0) {
		readFileOptions |= aiProcess_FlipUVs;
	}
	if (result.count("aiProcess_FlipWindingOrder") != 0) {
		readFileOptions |= aiProcess_FlipWindingOrder;
	}
	if (result.count("aiProcess_SplitByBoneCount") != 0) {
		readFileOptions |= aiProcess_SplitByBoneCount;
	}
	if (result.count("aiProcess_Debone") != 0) {
		readFileOptions |= aiProcess_Debone;
	}
	//Assimp macros
	if (result.count("aiProcessPreset_TargetRealtime_Fast") != 0) {
		readFileOptions |= aiProcessPreset_TargetRealtime_Fast;
	}
	if (result.count("aiProcessPreset_TargetRealtime_MaxQuality") != 0) {
		readFileOptions |= aiProcessPreset_TargetRealtime_MaxQuality;
	}
	if (result.count("aiProcessPreset_TargetRealtime_Quality") != 0) {
		readFileOptions |= aiProcessPreset_TargetRealtime_Quality;
	}

	ConvertModelFormat(inputFilepath, outputFilepath, readFileOptions);

	return 0;
}
