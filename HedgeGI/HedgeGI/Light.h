﻿#pragma once

class Scene;

enum LightType
{
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT
};

class Light
{
public:
    LightType type{};
    Vector3 positionOrDirection;
    Color3 color;
    float innerRange{};
    float outerRange{};

    Matrix3 getTangentToWorldMatrix() const;

    void read(const FileStream& file);
    void write(const FileStream& file) const;
};
