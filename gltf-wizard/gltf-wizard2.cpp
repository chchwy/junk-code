// gltf-wizard.cpp : Defines the entry point for the application.
//

#define STBI_MSC_SECURE_CRT
#include "gltf-wizard.h"
#include <filesystem>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

class CNode {

};

bool startsWith(const std::string& str, const std::string& prefix)
{
    return (str.rfind(prefix, 0) == 0);
}

bool contains(const std::string& str, const std::string& niddle) {
    return (str.find(niddle) != std::string::npos);
}

class GLTFWizard {

public:

    void Load(std::string path)
    {
        std::string errorMessage;
        std::string warningMessage;
        bool ok = glTFLoader.LoadASCIIFromFile(&model, &errorMessage, &warningMessage, path);

        if (ok)
        {
            std::cout << "GLTF is good" << std::endl;
            
            if (!warningMessage.empty()) {
                std::cout << "Warnings:" << warningMessage << std::endl;
            }
        } 
        else
        {
            std::cout << "ERROR:" << errorMessage << std::endl;
            if (!warningMessage.empty()) {
                std::cout << "Warnings:" << warningMessage << std::endl;
            }
            return;
        }

        std::cout << "Default scene is " << model.defaultScene << "\n";
        std::cout << "All scenes\n";

        for (tinygltf::Scene scene : model.scenes)
        {
            std::cout << " Scene: " << scene.name;

            for (int nodeIndex : scene.nodes) {
                tinygltf::Node& node = model.nodes[nodeIndex];
                traverseNode(model, node, 1);
            }
        }

        stats();
    }

    int traverseNode(tinygltf::Model& model, tinygltf::Node& node, int level)
    {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << node.name << "\n";

        node.name = rectifyName(node.name);

        for (int childNodeIndex : node.children)
        {
            tinygltf::Node& node = model.nodes[childNodeIndex];
            traverseNode(model, node, level + 1);
        }
        return 0;
    }

    std::string rectifyName(const std::string& oldName)
    {
        int pos = oldName.find("_");
        std::cout << "pos=" << pos << "\n";
        if (pos != std::string::npos) {
            std::string nextPart = oldName.substr(pos);
            std::cout << nextPart << std::endl;
        }

        if (startsWith(oldName, "appt")) {

        }
        return oldName;
    }

    void stats()
    {
        std::cout << "Node=" << model.nodes.size() << "\n";
        std::cout << "Mesh=" << model.meshes.size() << "\n";
        std::cout << "Materials=" << model.materials.size() << "\n";
    }

    void write(std::string path)
    {

    }

public:
    tinygltf::TinyGLTF glTFLoader;
    tinygltf::TinyGLTF glTFWriter;
    tinygltf::Model model;
};

int main()
{
	std::cout << "Hello glTF wizard." << std::endl;
    std::string path = "C:\\Temp\\highlight\\highlight_meshes_a123.gltf";

    GLTFWizard wizard;
    wizard.Load(path);

    
    tinygltf::Model& model2 = wizard.model;
    model2.asset.version = "2.0";
    model2.asset.generator = "glTF Wizard!";

    tinygltf::TinyGLTF writer;
    writer.WriteGltfSceneToFile(&model2, "C:\\temp\\highlight\\out\\out.gltf");
    
	return 0;
}
