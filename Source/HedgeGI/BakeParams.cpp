﻿#include "BakeParams.h"
#include "PropertyBag.h"

#include "OidnDenoiserDevice.h"
#include "OptixDenoiserDevice.h"

void BakeParams::load(const PropertyBag& propertyBag)
{
    targetEngine = propertyBag.get(PROP("bakeParams.targetEngine"), TargetEngine::HE1);

    environmentColorMode = propertyBag.get(PROP("bakeParams.environmentColorMode"), EnvironmentColorMode::Sky);
    environmentColor.x() = propertyBag.get(PROP("bakeParams.environmentColor.x()"), 106.0f / 255.0f);
    environmentColor.y() = propertyBag.get(PROP("bakeParams.environmentColor.y()"), 113.0f / 255.0f);
    environmentColor.z() = propertyBag.get(PROP("bakeParams.environmentColor.z()"), 179.0f / 255.0f);
    environmentColorIntensity = propertyBag.get(PROP("bakeParams.environmentColorIntensity"), 1.0f);
    skyIntensity = propertyBag.get(PROP("bakeParams.skyIntensity"), 1.0f);
    secondaryEnvironmentColor.x() = propertyBag.get(PROP("bakeParams.secondaryEnvironmentColor.x()"), 106.0f / 255.0f);
    secondaryEnvironmentColor.y() = propertyBag.get(PROP("bakeParams.secondaryEnvironmentColor.y()"), 113.0f / 255.0f);
    secondaryEnvironmentColor.z() = propertyBag.get(PROP("bakeParams.secondaryEnvironmentColor.z()"), 179.0f / 255.0f);

    lightBounceCount = propertyBag.get(PROP("bakeParams.lightBounceCount"), 10);
    lightSampleCount = propertyBag.get(PROP("bakeParams.lightSampleCount"), 32);
    russianRouletteMaxDepth = propertyBag.get(PROP("bakeParams.russianRouletteMaxDepth"), 4);

    shadowSampleCount = propertyBag.get(PROP("bakeParams.shadowSampleCount"), 64);
    shadowSearchRadius = propertyBag.get(PROP("bakeParams.shadowSearchRadius"), 0.01f);
    shadowBias = propertyBag.get(PROP("bakeParams.shadowBias"), 0.001f);

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
    denoiserType = propertyBag.get(PROP("bakeParams.denoiserType"), 
        OptixDenoiserDevice::available ? DenoiserType::Optix : OidnDenoiserDevice::available ? DenoiserType::Oidn : DenoiserType::None);

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
    propertyBag.set(PROP("bakeParams.secondaryEnvironmentColor.x()"), secondaryEnvironmentColor.x());
    propertyBag.set(PROP("bakeParams.secondaryEnvironmentColor.y()"), secondaryEnvironmentColor.y());
    propertyBag.set(PROP("bakeParams.secondaryEnvironmentColor.z()"), secondaryEnvironmentColor.z());

    propertyBag.set(PROP("bakeParams.lightBounceCount"), lightBounceCount);
    propertyBag.set(PROP("bakeParams.lightSampleCount"), lightSampleCount);
    propertyBag.set(PROP("bakeParams.russianRouletteMaxDepth"), russianRouletteMaxDepth);

    propertyBag.set(PROP("bakeParams.shadowSampleCount"), shadowSampleCount);
    propertyBag.set(PROP("bakeParams.shadowSearchRadius"), shadowSearchRadius);
    propertyBag.set(PROP("bakeParams.shadowBias"), shadowBias);

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

DenoiserType BakeParams::getDenoiserType() const
{
    if (!OidnDenoiserDevice::available && denoiserType == DenoiserType::Oidn) return DenoiserType::None;
    if (!OptixDenoiserDevice::available && denoiserType == DenoiserType::Optix) return DenoiserType::None;

    return denoiserType;
}