// gltf-wizard.cpp : Defines the entry point for the application.
//

#include "gltf-wizard.h"
#include <filesystem>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"


int traverseNode(const tinygltf::Model& model, const tinygltf::Node& node, int level) {

    for (int i = 0; i < level; ++i) std::cout << "  ";
    std::cout << node.name << "\n";

    for (int childNodeIndex : node.children) {

        const tinygltf::Node& node = model.nodes[childNodeIndex];
        traverseNode(model, node, level + 1);
    }
    return 0;
}

int main()
{
	std::cout << "Hello glTF wizard." << std::endl;
    tinygltf::TinyGLTF glTFLoader;

    std::string path = "C:\\Users\\Matt\\Desktop\\ParksRedHill_2019-07-24_exporting_glTF\\interior\\walls_building1.gltf";

    tinygltf::Model model;
    std::string errorMessage;
    std::string warningMessage;
    bool ok = glTFLoader.LoadASCIIFromFile(&model, &errorMessage, &warningMessage, path);
    
    if (ok) {
        std::cout << "Good" << std::endl;
    }

    std::cout << "Default scene is " << model.defaultScene << "\n";
    std::cout << "All scenes\n";

    for (const tinygltf::Scene scene : model.scenes)
    {
        std::cout << " Scene: " << scene.name;

        for (int nodeIndex : scene.nodes) {
            const tinygltf::Node& node = model.nodes[nodeIndex];
            traverseNode(model, node, 1);
        }
    }

	return 0;
}
