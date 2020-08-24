#include<assimp/Importer.hpp>
#include<assimp/Exporter.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<iostream>
#include<string>

int ConvertModelToOBJ(
	const std::string& src_filepath, 
	const std::string& dst_filepath,
	unsigned int importerFlags) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(src_filepath, importerFlags);
	if (!scene) {
		std::string error_message = importer.GetErrorString();
		std::cout << "Error: " << error_message << std::endl;

		return -1;
	}

	Assimp::Exporter exporter;
	aiReturn ret = exporter.Export(scene, "obj", dst_filepath, scene->mFlags);
	if (ret == aiReturn_FAILURE) {
		std::string error_message = exporter.GetErrorString();
		std::cout << "Error: " << error_message << std::endl;

		return -1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Error: Invalid number of command-line arguments." << std::endl;
		return -1;
	}

	auto src_filepath = std::string(argv[1]);
	auto dst_filepath = std::string(argv[2]);

	ConvertModelToOBJ(
		src_filepath, dst_filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate|
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	return 0;
}
