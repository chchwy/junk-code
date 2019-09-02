// SceneDataGen.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "json.hpp"

using json = nlohmann::json;

json cameraPos;

json Plans2d;

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

int getLevelNumber(int apartmentNumber)
{
    if (apartmentNumber <= 3) return 0;
    if (apartmentNumber <= 7) return 1;
    if (apartmentNumber <= 12) return 2;
    if (apartmentNumber <= 17) return 3;
    if (apartmentNumber <= 21) return 4;
}

std::pair<int, int> getApartmentRange(int level) {
    if (level == 0) return std::make_pair(1, 3);
    if (level == 1) return std::make_pair(4, 7);
    if (level == 2) return std::make_pair(8, 12);
    if (level == 3) return std::make_pair(13, 17);
    if (level == 4) return std::make_pair(18, 21);
}

std::string apartmentNumberStr(int i) {
    char buffer[1024];
    memset(buffer, 0, 1024);
    sprintf_s(buffer, 1024, "%03d", i);
    std::string apartmentNumber = std::string(buffer);
    return apartmentNumber;
}

json getCameraObject(json camPosArray)
{
    json cameraObject = json::object();
    cameraObject["center"] = camPosArray;
    cameraObject["alpha"] = 45;
    cameraObject["beta"] = 54;
    cameraObject["distance"] = 30;
    cameraObject["maxDistance"] = 60;
    cameraObject["minDistance"] = 15;
    return cameraObject;
}

json getModelsArray(int apartment)
{
    std::string apartmentNumber = apartmentNumberStr(apartment);

    json modelsArray = json::array();

    json plan2d = json::object();
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

    Plans2d.push_back(plan2d);

    json plan3d = json::object();
    plan3d["name"] = "3D FloorPlan " + apartmentNumber;

    int levelNumber = getLevelNumber(std::stoi(apartmentNumber));

    std::stringstream sout;
    sout << "floorplan/building1_level" << levelNumber << ".glb";
    plan3d["path"] = sout.str();
    plan3d["type"] = "floorplan3D";
    json transform3d = json::object();
    {
        transform3d["position"] = json::array({ 0, 0, 0 });
        transform3d["rotation"] = json::array({ 0, 0, 0 });
        transform3d["scale"] = json::array({ 1, 1, 1 });
    }
    plan3d["transform"] = transform3d;
    plan3d["nodeVisibility"] = json::object();
    {
        std::pair<int, int> range = getApartmentRange(levelNumber);
        int min = range.first;
        int max = range.second;
        for (int i = min; i <= max; ++i) {
            std::stringstream sout;
            sout << "apartment_geometry/Block2_3/Building_1_vqxyyp/level_" << levelNumber
                << "/apartment" << apartmentNumberStr(i);

            std::string nodePath = sout.str();
            plan3d["nodeVisibility"][nodePath] = bool(i == apartment);

            /*
            "apartment_geometry/Block2_3/Building_1_vqxyyp/level_4/apartment018": false,
            "apartment_geometry/Block2_3/Building_1_vqxyyp/level_4/apartment019" : true,
            "apartment_geometry/Block2_3/Building_1_vqxyyp/level_4/apartment020" : false,
            "apartment_geometry/Block2_3/Building_1_vqxyyp/level_4/apartment021" : false,
            "apartment_geometry/Block2_3/Building_1_vqxyyp/level_4/Corridor_nvabyz" : false,
            */
        }
        std::stringstream sout;
        sout << "apartment_geometry/Block2_3/Building_1_vqxyyp/level_" << levelNumber << "/Corridor";
        plan3d["nodeVisibility"][sout.str()] = false;
    }
    plan3d["pickable"] = false;
    modelsArray.push_back(plan3d);

    return modelsArray;
}

int main()
{
    std::cout << "Hello World!\n";

    cameraPos = prepareCameraPos();
    Plans2d = json::array();

    json j = json::array();
    
    for (int i = 1; i <= 21; ++i)
    {
        std::string apartmentNumber = apartmentNumberStr(i);
        
        char buffer[1024];
        sprintf_s(buffer, 1024, "00000000-0000-0000-0000-%012d", i);
        std::string sceneGUID = std::string(buffer);

        json scene = json::object();
        scene["name"] = "Block23_Apt_" + apartmentNumber;
        scene["sceneId"] = sceneGUID;
        scene["type"] = "unit";
        scene["camera"] = getCameraObject(findCameraPosByName(apartmentNumber));
        scene["models"] = getModelsArray(i);
        scene["components"] = json::array({ json::object({ {"name", "FloorPlanSwitch"} }) });
        
        j.push_back(scene);
    }

    std::string str = j.dump(2); // stringify
    std::cout << str;

    std::ofstream fout("scenedata.json", std::ofstream::out);
    fout << str;
    fout.flush();
    fout.close();

    std::string plans2d_string = Plans2d.dump(2);
    std::cout << plans2d_string;

    std::ofstream fout2("floorplan2.json", std::ofstream::out);
    fout2 << plans2d_string;
    fout2.flush();
    fout2.close();
    return 0;
}
