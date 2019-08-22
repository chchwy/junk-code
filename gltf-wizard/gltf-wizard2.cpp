// gltf-wizard.cpp : Defines the entry point for the application.
//

#define STBI_MSC_SECURE_CRT
#include "gltf-wizard.h"
#include <filesystem>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

using std::cout;

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
            cout << "GLTF is good" << std::endl;
            
            if (!warningMessage.empty()) {
                cout << "Warnings:" << warningMessage << std::endl;
            }
        } 
        else
        {
            cout << "ERROR:" << errorMessage << std::endl;
            if (!warningMessage.empty()) {
                cout << "Warnings:" << warningMessage << std::endl;
            }
            return;
        }

        cout << "Default scene is " << model.defaultScene << "\n";
        cout << "All scenes\n";

        for (tinygltf::Scene scene : model.scenes)
        {
            cout << " Scene: " << scene.name;

            for (int nodeIndex : scene.nodes) {
                tinygltf::Node& node = model.nodes[nodeIndex];
                traverseNode(model, node, 1);
            }
        }

        stats();
    }

    int traverseNode(tinygltf::Model& model, tinygltf::Node& node, int level)
    {
        //for (int i = 0; i < level; ++i) cout << "  ";
        //cout << node.name << "\n";

        if (node.mesh != -1)
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
        std::string newName = "";

        int pos = oldName.rfind("_");
        
        if (pos != std::string::npos) {
            std::string nextPart = oldName.substr(pos + 1);
            //cout << nextPart << std::endl;
            newName = nextPart;
        }

        int digitPos = 0;
        for (int i = 0; i < newName.size(); ++i)
        {
            if (isdigit(newName[i]))
            {
                digitPos = i;
                break;
            }
        }

        newName = newName.substr(digitPos);
        

        int number = std::stoi(newName);
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        sprintf_s(buffer, 1024, "%03d", number);

        newName = buffer;
        cout << newName << std::endl;
        return oldName;
    }

    void stats()
    {
        cout << "Node=" << model.nodes.size() << "\n";
        cout << "Mesh=" << model.meshes.size() << "\n";
        cout << "Materials=" << model.materials.size() << "\n";
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
	cout << "Hello glTF wizard." << std::endl;
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
