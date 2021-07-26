﻿#pragma once

#include "SceneEffect.h"

class Bitmap;
class Material;
class Mesh;
class Instance;
class Light;
class SHLightField;

class FileStream;

struct RaytracingContext
{
    const class Scene* scene{};
    RTCScene rtcScene{};
};

class Scene
{
    RTCScene rtcScene{};

public:
    ~Scene();

    std::vector<std::unique_ptr<const Bitmap>> bitmaps;
    std::vector<std::unique_ptr<const Material>> materials;
    std::vector<std::unique_ptr<const Mesh>> meshes;
    std::vector<std::unique_ptr<const Instance>> instances;

    std::vector<std::unique_ptr<Light>> lights;
    std::vector<std::unique_ptr<SHLightField>> shLightFields;

    AABB aabb;

    SceneEffect sceneEffect {};

    void buildAABB();

    RTCScene createRTCScene();
    RaytracingContext getRaytracingContext();

    void removeUnusedBitmaps();

    void read(const FileStream& file);
    void write(const FileStream& file) const;

    bool load(const std::string& filePath);
    void save(const std::string& filePath) const;
};
