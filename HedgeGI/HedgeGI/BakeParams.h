﻿#pragma once

class PropertyBag;

enum TargetEngine
{
    TARGET_ENGINE_HE1,
    TARGET_ENGINE_HE2
};

enum DenoiserType
{
    DENOISER_TYPE_NONE,
    DENOISER_TYPE_OPTIX,
    DENOISER_TYPE_OIDN
};

struct BakeParams
{
    TargetEngine targetEngine;

    Color3 environmentColor;

    uint32_t lightBounceCount {};
    uint32_t lightSampleCount {};
    uint32_t russianRouletteMaxDepth {};

    uint32_t shadowSampleCount {};
    float shadowSearchRadius {};
    float shadowBias {};

    uint32_t aoSampleCount {};
    float aoFadeConstant {};
    float aoFadeLinear {};
    float aoFadeQuadratic {};
    float aoStrength {};

    float diffuseStrength {};
    float diffuseSaturation {};
    float lightStrength {};
    float emissionStrength {};

    float resolutionBase {};
    float resolutionBias {};
    int16_t resolutionOverride {};
    uint16_t resolutionMinimum {};
    uint16_t resolutionMaximum {};

    bool denoiseShadowMap {};
    bool optimizeSeams {};
    DenoiserType denoiserType {};

    float lightFieldMinCellRadius {};
    float lightFieldAabbSizeMultiplier {};

    BakeParams() : targetEngine(TARGET_ENGINE_HE1) {}
    BakeParams(const TargetEngine targetEngine) : targetEngine(targetEngine) {}

    void load(const std::string& filePath);

    void load(const PropertyBag& propertyBag);
    void store(PropertyBag& propertyBag) const;
};