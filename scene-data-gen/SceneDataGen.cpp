// SceneDataGen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "json.hpp"

using json = nlohmann::json;

struct Vec3 {
    float x = 0;
    float y = 0;
    float z = 0;
    Vec3() {}
    Vec3(float px, float py, float pz) {
        x = px; y = py; z = pz;
    }
};

std::map<std::string, Vec3> positionMap = {
    { "019", Vec3(114.3, 11, 29.9) },
    { "020", Vec3(123.82, 11.1, 38.33) }
};

json getCameraObject(json camPosArray)
{
    json cameraObject = json::object();
    /*{
        "center": [114.189, 12.785, 29.044],
        "alpha": 45,
        "beta": 54,
        "distance": 30,
        "maxDistance": 60,
        "minDistance": 15,
    }*/
    cameraObject["center"] = camPosArray;
    cameraObject["alpha"] = 45;
    cameraObject["beta"] = 54;
    cameraObject["distance"] = 30;
    cameraObject["maxDistance"] = 60;
    cameraObject["minDistance"] = 15;
    return cameraObject;
}

json getModelsArray(std::string apartmentNumber)
{
    json modelsArray = json::array();

    json plan2d = json::object();
    /*{
          "name": "FloorPlan2D 019 ",
          "path": "floorplan/FloorPlan019.glb",
          "transform": {
            "position": [114.189, 10, 29.044],
            "rotation": [0, 133, 0],
            "scale":[10 ,10, 10],
          },
          "nodeVisibility":{},
          "type": "floorplan2D",
          "pickable": false,
      }*/
    plan2d["name"] = "2D FloorPlan " + apartmentNumber;
    plan2d["path"] = "floorplan/fp_b23_a_" + apartmentNumber + ".gltf";
    plan2d["type"] = "floorplan2D";
    plan2d["transform"] = json::object();
    plan2d["nodeVisibility"] = json::object();
    plan2d["pickable"] = false;
    modelsArray.push_back(plan2d);

    json plan3d = json::object();
    /*
    {
          "name": "FloorPlan3D 019",
          "path": "floorplan/Apartment019d.glb",
          "transform": {
            "position": [0, 0, 0],
            "rotation": [0, 0, 0],
            "scale":[1 ,1, 1],
          },
          "type": "floorplan3D",
          "pickable": false,
    },*/
    plan3d["name"] = "3D FloorPlan " + apartmentNumber;
    plan3d["path"] = "floorplan/Apartment" + apartmentNumber + ".glb";
    plan2d["type"] = "floorplan3D";
    plan3d["transform"] = json::object();
    plan2d["nodeVisibility"] = json::object();
    plan2d["pickable"] = false;
    modelsArray.push_back(plan3d);

    return modelsArray;
}

json prepareCameraPos()
{
    std::ifstream fin("campos.json");
    json campos;
    fin >> campos;
    std::cout<< campos.dump(2);
    return campos;
}

json findCameraPosByName(json camPos, std::string apartmentNumber) {
    for (auto c : camPos)
    {
        if (c["name"] == apartmentNumber)
        {
            //std::cout << c["name"] << c["pos"] << std::endl;
            return c["pos"];
        }
    }
    return json::array();
}

int main()
{
    json camPos = prepareCameraPos();

    std::cout << "Hello World!\n";

    json j = json::array();
    
    for (int i = 1; i <= 21; ++i)
    {
        char buffer[1024];
        sprintf_s(buffer, 1024, "%03d", i);
        std::string apartmentNumber = std::string(buffer);
        
        sprintf_s(buffer, 1024, "00000000-0000-0000-0000-%012d", i);
        std::string sceneGUID = std::string(buffer);

        json scene = json::object();
        scene["name"] = "Block23_Apt_" + apartmentNumber;
        scene["sceneId"] = sceneGUID;
        scene["type"] = "unit";
        scene["camera"] = getCameraObject(findCameraPosByName(camPos, apartmentNumber));
        scene["models"] = getModelsArray(apartmentNumber);
        scene["components"] = json::array({ json::object({ {"name", "FloorPlanSwitch"} }) });
        
        j.push_back(scene);
    }

    std::string str = j.dump(2); // stringify
    std::cout << str;

    std::ofstream fout("scenedata.json", std::ofstream::out);
    fout << str;
    fout.flush();
    fout.close();
    return 0;
}
