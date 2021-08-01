﻿#include "BakeParams.h"
#include "PropertyBag.h"

void BakeParams::load(const std::string& filePath)
{
    INIReader reader(filePath);

    if (reader.ParseError() != 0)
        return;

    if (targetEngine == TargetEngine::HE2)
    {
        environmentColor.x() = reader.GetFloat("EnvironmentColorHE2", "R", 1.0f);
        environmentColor.y() = reader.GetFloat("EnvironmentColorHE2", "G", 1.0f);
        environmentColor.z() = reader.GetFloat("EnvironmentColorHE2", "B", 1.0f);
    }
    else
    {
        environmentColor.x() = reader.GetFloat("EnvironmentColorHE1", "R", 255.0f) / 255.0f;
        environmentColor.y() = reader.GetFloat("EnvironmentColorHE1", "G", 255.0f) / 255.0f;
        environmentColor.z() = reader.GetFloat("EnvironmentColorHE1", "B", 255.0f) / 255.0f;
    }

    lightBounceCount = reader.GetInteger("Baker", "LightBounceCount", 10);
    lightSampleCount = reader.GetInteger("Baker", "LightSampleCount", 32);
    russianRouletteMaxDepth = reader.GetInteger("Baker", "RussianRouletteMaxDepth", 4);

    shadowSampleCount = reader.GetInteger("Baker", "ShadowSampleCount", 64);
    shadowSearchRadius = reader.GetFloat("Baker", "ShadowSearchRadius", 0.01f);
    shadowBias = reader.GetFloat("Baker", "ShadowBias", 0.001f);

    aoSampleCount = reader.GetInteger("Baker", "AoSampleCount", 64);
    aoFadeConstant = reader.GetFloat("Baker", "AoFadeConstant", 1.0f);
    aoFadeLinear = reader.GetFloat("Baker", "AoFadeLinear", 0.01f);
    aoFadeQuadratic = reader.GetFloat("Baker", "AoFadeQuadratic", 0.01f);
    aoStrength = reader.GetFloat("Baker", "AoStrength", 0.0f);

    diffuseStrength = reader.GetFloat("Baker", "DiffuseStrength", 1.0f);
    diffuseSaturation = reader.GetFloat("Baker", "DiffuseSaturation", 1.0f);
    lightStrength = reader.GetFloat("Baker", "LightStrength", 1.0f);
    emissionStrength = reader.GetFloat("Baker", "EmissionStrength", 1.0f);

    resolutionBase = reader.GetFloat("Baker", "ResolutionBase", 2.0f);
    resolutionBias = reader.GetFloat("Baker", "ResolutionBias", 3.0f);
    resolutionOverride = (uint16_t)reader.GetInteger("Baker", "ResolutionOverride", -1);
    resolutionMinimum = (uint16_t)reader.GetInteger("Baker", "ResolutionMinimum", 16);
    resolutionMaximum = (uint16_t)reader.GetInteger("Baker", "ResolutionMaximum", 2048);

    denoiseShadowMap = reader.GetBoolean("Baker", "DenoiseShadowMap", true);
    optimizeSeams = reader.GetBoolean("Baker", "OptimizeSeams", true);
    denoiserType = (DenoiserType)reader.GetInteger("Baker", "DenoiserType", (long)DenoiserType::Optix);

    lightFieldMinCellRadius = reader.GetFloat("LightField", "MinCellRadius", 5.0f);
    lightFieldAabbSizeMultiplier = reader.GetFloat("LightField", "AabbSizeMultiplier", 1.0f);
}

void BakeParams::load(const PropertyBag& propertyBag)
{
    targetEngine = propertyBag.get(PROP("bakeParams.targetEngine"), TargetEngine::HE1);

    environmentColorMode = propertyBag.get(PROP("bakeParams.environmentColorMode"), EnvironmentColorMode::Sky);
    environmentColor.x() = propertyBag.get(PROP("bakeParams.environmentColor.x()"), 106.0f / 255.0f);
    environmentColor.y() = propertyBag.get(PROP("bakeParams.environmentColor.y()"), 113.0f / 255.0f);
    environmentColor.z() = propertyBag.get(PROP("bakeParams.environmentColor.z()"), 179.0f / 255.0f);
    environmentColorIntensity = propertyBag.get(PROP("bakeParams.environmentColorIntensity"), 1.0f);
    skyIntensity = propertyBag.get(PROP("bakeParams.skyIntensity"), 1.0f);

    lightBounceCount = propertyBag.get(PROP("bakeParams.lightBounceCount"), 10);
    lightSampleCount = propertyBag.get(PROP("bakeParams.lightSampleCount"), 32);
    russianRouletteMaxDepth = propertyBag.get(PROP("bakeParams.russianRouletteMaxDepth"), 4);

    shadowSampleCount = propertyBag.get(PROP("bakeParams.shadowSampleCount"), 64);
    shadowSearchRadius = propertyBag.get(PROP("bakeParams.shadowSearchRadius"), 0.01f);
    shadowBias = propertyBag.get(PROP("bakeParams.shadowBias"), 0.001f);

    aoSampleCount = propertyBag.get(PROP("bakeParams.aoSampleCount"), 64);
    aoFadeConstant = propertyBag.get(PROP("bakeParams.aoFadeConstant"), 1.0f);
    aoFadeLinear = propertyBag.get(PROP("bakeParams.aoFadeLinear"), 0.01f);
    aoFadeQuadratic = propertyBag.get(PROP("bakeParams.aoFadeQuadratic"), 0.01f);
    aoStrength = propertyBag.get(PROP("bakeParams.aoStrength"), 0.0f);

    diffuseStrength = propertyBag.get(PROP("bakeParams.diffuseStrength"), 1.0f);
    diffuseSaturation = propertyBag.get(PROP("bakeParams.diffuseSaturation"), 1.0f);
    lightStrength = propertyBag.get(PROP("bakeParams.lightStrength"), 1.0f);
    emissionStrength = propertyBag.get(PROP("bakeParams.emissionStrength"), 1.0f);

    resolutionBase = propertyBag.get(PROP("bakeParams.resolutionBase"), 2.0f);
    resolutionBias = propertyBag.get(PROP("bakeParams.resolutionBias"), 3.0f);
    resolutionOverride = propertyBag.get(PROP("bakeParams.resolutionOverride"), -1);
    resolutionMinimum = propertyBag.get(PROP("bakeParams.resolutionMinimum"), 16);
    resolutionMaximum = propertyBag.get(PROP("bakeParams.resolutionMaximum"), 2048);

    denoiseShadowMap = propertyBag.get(PROP("bakeParams.denoiseShadowMap"), true);
    optimizeSeams = propertyBag.get(PROP("bakeParams.optimizeSeams"), true);
    denoiserType = propertyBag.get(PROP("bakeParams.denoiserType"), DenoiserType::Optix);

    lightFieldMinCellRadius = propertyBag.get(PROP("bakeParams.lightFieldMinCellRadius"), 5.0f);
    lightFieldAabbSizeMultiplier = propertyBag.get(PROP("bakeParams.lightFieldAabbSizeMultiplier"), 1.0f);
}

void BakeParams::store(PropertyBag& propertyBag) const
{
    propertyBag.set(PROP("bakeParams.targetEngine"), targetEngine);

    propertyBag.set(PROP("bakeParams.environmentColorMode"), environmentColorMode);
    propertyBag.set(PROP("bakeParams.environmentColor.x()"), environmentColor.x());
    propertyBag.set(PROP("bakeParams.environmentColor.y()"), environmentColor.y());
    propertyBag.set(PROP("bakeParams.environmentColor.z()"), environmentColor.z());
    propertyBag.set(PROP("bakeParams.environmentColorIntensity"), environmentColorIntensity);
    propertyBag.set(PROP("bakeParams.skyIntensity"), skyIntensity);

    propertyBag.set(PROP("bakeParams.lightBounceCount"), lightBounceCount);
    propertyBag.set(PROP("bakeParams.lightSampleCount"), lightSampleCount);
    propertyBag.set(PROP("bakeParams.russianRouletteMaxDepth"), russianRouletteMaxDepth);

    propertyBag.set(PROP("bakeParams.shadowSampleCount"), shadowSampleCount);
    propertyBag.set(PROP("bakeParams.shadowSearchRadius"), shadowSearchRadius);
    propertyBag.set(PROP("bakeParams.shadowBias"), shadowBias);

    propertyBag.set(PROP("bakeParams.aoSampleCount"), aoSampleCount);
    propertyBag.set(PROP("bakeParams.aoFadeConstant"), aoFadeConstant);
    propertyBag.set(PROP("bakeParams.aoFadeLinear"), aoFadeLinear);
    propertyBag.set(PROP("bakeParams.aoFadeQuadratic"), aoFadeQuadratic);
    propertyBag.set(PROP("bakeParams.aoStrength"), aoStrength);

    propertyBag.set(PROP("bakeParams.diffuseStrength"), diffuseStrength);
    propertyBag.set(PROP("bakeParams.diffuseSaturation"), diffuseSaturation);
    propertyBag.set(PROP("bakeParams.lightStrength"), lightStrength);
    propertyBag.set(PROP("bakeParams.emissionStrength"), emissionStrength);

    propertyBag.set(PROP("bakeParams.resolutionBase"), resolutionBase);
    propertyBag.set(PROP("bakeParams.resolutionBias"), resolutionBias);
    propertyBag.set(PROP("bakeParams.resolutionOverride"), resolutionOverride);
    propertyBag.set(PROP("bakeParams.resolutionMinimum"), resolutionMinimum);
    propertyBag.set(PROP("bakeParams.resolutionMaximum"), resolutionMaximum);

    propertyBag.set(PROP("bakeParams.denoiseShadowMap"), denoiseShadowMap);
    propertyBag.set(PROP("bakeParams.optimizeSeams"), optimizeSeams);
    propertyBag.set(PROP("bakeParams.denoiserType"), denoiserType);

    propertyBag.set(PROP("bakeParams.lightFieldMinCellRadius"), lightFieldMinCellRadius);
    propertyBag.set(PROP("bakeParams.lightFieldAabbSizeMultiplier"), lightFieldAabbSizeMultiplier);
}