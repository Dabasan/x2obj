#include<assimp/Importer.hpp>
#include<assimp/Exporter.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include<iostream>
#include<string>

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
	if (argc != 3) {
		std::cerr << "Error: Invalid number of command-line arguments." << std::endl;
		return -1;
	}

	auto src_filepath = std::string(argv[1]);
	auto dst_filepath = std::string(argv[2]);

	ConvertModelFormat(
		src_filepath, dst_filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	return 0;
}
