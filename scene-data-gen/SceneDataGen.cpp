// SceneDataGen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "json.hpp"

using json = nlohmann::json;

json cameraPos;

json prepareCameraPos()
{
    std::ifstream fin("campos.json");
    json campos;
    fin >> campos;
    fin.close();
    //std::cout<< campos.dump(2);
    return campos;
}

json findCameraPosByName(std::string apartmentNumber) {
    for (auto c : cameraPos)
    {
        if (c["name"] == apartmentNumber)
        {
            return c["pos"];
        }
    }
    return json::array();
}

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
    json transform = json::object();
    {
        transform["position"] = findCameraPosByName(apartmentNumber);
        transform["rotation"] = json::array({ 0, -36, 0 });
        transform["scale"] = json::array({ 1.8, 1, 1.8 });
    }
    plan2d["transform"] = transform;
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
    plan3d["path"] = "floorplan/building1_level4.glb";
    plan3d["type"] = "floorplan3D";
    plan3d["transform"] = json::object();
    plan3d["nodeVisibility"] = json::object();
    plan3d["pickable"] = false;
    modelsArray.push_back(plan3d);

    return modelsArray;
}

int main()
{
    std::cout << "Hello World!\n";

    cameraPos = prepareCameraPos();

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
        scene["camera"] = getCameraObject(findCameraPosByName(apartmentNumber));
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
