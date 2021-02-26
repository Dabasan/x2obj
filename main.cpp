#include<assimp/Importer.hpp>
#include<assimp/Exporter.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<iostream>
#include<string>

#include"cxxopts.hpp"

int ConvertModelFormat(
	const std::string& src_filepath, 
	const std::string& dst_filepath,
	unsigned int importerFlags) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(src_filepath, importerFlags);
	if (!scene) {
		std::string error_message = importer.GetErrorString();
		std::cerr << "Error: " << error_message << std::endl;

		return -1;
	}

	//出力ファイルの拡張子を取得し、出力ファイルのフォーマットを決定する。
	size_t last_index_of_dot = dst_filepath.find_last_of('.');
	if (last_index_of_dot == std::string::npos) {
		std::cerr << "Error: Cannot determine the output format." << std::endl;
		return -1;
	}

	auto extension = dst_filepath.substr(last_index_of_dot + 1);

	Assimp::Exporter exporter;
	size_t num_exporters = exporter.GetExportFormatCount();
	std::string format_id = "";
	for (int i = 0; i < num_exporters; i++) {
		const aiExportFormatDesc* format = exporter.GetExportFormatDescription(i);
		if (format->fileExtension == extension) {
			format_id = format->id;
			break;
		}
	}
	if (format_id == "") {
		std::cerr << "Error: Unsupported output format." << std::endl;
		return -1;
	}

	aiReturn ret = exporter.Export(scene, format_id, dst_filepath, scene->mFlags);
	if (ret == aiReturn_FAILURE) {
		std::string error_message = exporter.GetErrorString();
		std::cerr << "Error: " << error_message << std::endl;

		return -1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	const std::string VERSION_STR = "Model Converter v0.2.0";

	cxxopts::Options options("Model Converter","Converts model format with Assimp");
	options.add_options()
		//General
		("i,input_filepath", "Input filepath", cxxopts::value<std::string>())
		("o,output_filepath", "Output filepath (must contain extension)", cxxopts::value<std::string>())
		("h,help","Displays help")
		("v,version","Displays version")
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
		("aiProcessPreset_TargetRealtime_Fast","")
		("aiProcessPreset_TargetRealtime_MaxQuality","")
		("aiProcessPreset_TargetRealtime_Quality","")
		;
	auto result = options.parse(argc, argv);

	if (result.count("help") != 0) {
		std::cout << options.help() << std::endl;
		return 0;
	}
	else if (result.count("version") != 0) {
		std::cout << VERSION_STR << std::endl;
		return 0;
	}
	
	if (result.count("input_filepath") == 0) {
		std::cerr << "Error: You must specify input filepath" << std::endl;
		return -1;
	}
	else if (result.count("output_filepath") == 0) {
		std::cerr << "Error: You must specify output filepath" << std::endl;
		return -1;
	}

	auto input_filepath = result["input_filepath"].as<std::string>();
	auto output_filepath = result["output_filepath"].as<std::string>();

	unsigned int read_file_options = 0;
	//Assimp options
	if (result.count("aiProcess_CalcTangentSpace") != 0) {
		read_file_options |= aiProcess_CalcTangentSpace;
	}
	if (result.count("aiProcess_JoinIdenticalVertices") != 0) {
		read_file_options |= aiProcess_JoinIdenticalVertices;
	}
	if (result.count("aiProcess_MakeLeftHanded") != 0) {
		read_file_options |= aiProcess_MakeLeftHanded;
	}
	if (result.count("aiProcess_Triangulate") != 0) {
		read_file_options |= aiProcess_Triangulate;
	}
	if (result.count("aiProcess_RemoveComponent") != 0) {
		read_file_options |= aiProcess_RemoveComponent;
	}
	if (result.count("aiProcess_GenNormals") != 0) {
		read_file_options |= aiProcess_GenNormals;
	}
	if (result.count("aiProcess_GenSmoothNormals") != 0) {
		read_file_options |= aiProcess_GenSmoothNormals;
	}
	if (result.count("aiProcess_SplitLargeMeshes") != 0) {
		read_file_options |= aiProcess_SplitLargeMeshes;
	}
	if (result.count("aiProcess_PreTransformVertices") != 0) {
		read_file_options |= aiProcess_PreTransformVertices;
	}
	if (result.count("aiProcess_LimitBoneWeights") != 0) {
		read_file_options |= aiProcess_LimitBoneWeights;
	}
	if (result.count("aiProcess_ValidateDataStructure") != 0) {
		read_file_options |= aiProcess_ValidateDataStructure;
	}
	if (result.count("aiProcess_ImproveCacheLocality") != 0) {
		read_file_options |= aiProcess_ImproveCacheLocality;
	}
	if (result.count("aiProcess_RemoveRedundantMaterials") != 0) {
		read_file_options |= aiProcess_RemoveRedundantMaterials;
	}
	if (result.count("aiProcess_FixInfacingNormals") != 0) {
		read_file_options |= aiProcess_FixInfacingNormals;
	}
	if (result.count("aiProcess_SortByPType") != 0) {
		read_file_options |= aiProcess_SortByPType;
	}
	if (result.count("aiProcess_FindDegenerates") != 0) {
		read_file_options |= aiProcess_FindDegenerates;
	}
	if (result.count("aiProcess_FindInvalidData") != 0) {
		read_file_options |= aiProcess_FindInvalidData;
	}
	if (result.count("aiProcess_GenUVCoords") != 0) {
		read_file_options |= aiProcess_GenUVCoords;
	}
	if (result.count("aiProcess_TransformUVCoords") != 0) {
		read_file_options |= aiProcess_TransformUVCoords;
	}
	if (result.count("aiProcess_FindInstances") != 0) {
		read_file_options |= aiProcess_FindInstances;
	}
	if (result.count("aiProcess_OptimizeMeshes") != 0) {
		read_file_options |= aiProcess_OptimizeMeshes;
	}
	if (result.count("aiProcess_OptimizeGraph") != 0) {
		read_file_options |= aiProcess_OptimizeGraph;
	}
	if (result.count("aiProcess_FlipUVs") != 0) {
		read_file_options |= aiProcess_FlipUVs;
	}
	if (result.count("aiProcess_FlipWindingOrder") != 0) {
		read_file_options |= aiProcess_FlipWindingOrder;
	}
	if (result.count("aiProcess_SplitByBoneCount") != 0) {
		read_file_options |= aiProcess_SplitByBoneCount;
	}
	if (result.count("aiProcess_Debone") != 0) {
		read_file_options |= aiProcess_Debone;
	}
	//Assimp macros
	if (result.count("aiProcessPreset_TargetRealtime_Fast") != 0) {
		read_file_options |= aiProcessPreset_TargetRealtime_Fast;
	}
	if (result.count("aiProcessPreset_TargetRealtime_MaxQuality") != 0) {
		read_file_options |= aiProcessPreset_TargetRealtime_MaxQuality;
	}
	if (result.count("aiProcessPreset_TargetRealtime_Quality") != 0) {
		read_file_options |= aiProcessPreset_TargetRealtime_Quality;
	}

	ConvertModelFormat(input_filepath, output_filepath,read_file_options);

	return 0;
}
