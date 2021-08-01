﻿#pragma once

#include "BakeParams.h"
#include "BakePoint.h"
#include "Bitmap.h"
#include "Light.h"
#include "Material.h"
#include "Random.h"
#include "Scene.h"

class Camera;

class BakingFactory
{
    static std::mutex mutex;

public:
    template<TargetEngine targetEngine>
    static Color3 sampleSky(const RaytracingContext& raytracingContext, const Vector3& direction, const BakeParams& bakeParams);

    template <TargetEngine targetEngine, bool tracingFromEye>
    static Color4 pathTrace(const RaytracingContext& raytracingContext, 
        const Vector3& position, const Vector3& direction, const BakeParams& bakeParams);

    static Color4 pathTrace(const RaytracingContext& raytracingContext,
        const Vector3& position, const Vector3& direction, const BakeParams& bakeParams, bool tracingFromEye = false);

    template <typename TBakePoint>
    static void bake(const RaytracingContext& raytracingContext, std::vector<TBakePoint>& bakePoints, const BakeParams& bakeParams);

    static void bake(const RaytracingContext& raytracingContext, const Bitmap& bitmap,
        size_t width, size_t height, const Camera& camera, const BakeParams& bakeParams, size_t progress = 0);

    static std::lock_guard<std::mutex> lock()
    {
        return std::lock_guard(mutex);
    }
};

template <typename TBakePoint>
void BakingFactory::bake(const RaytracingContext& raytracingContext, std::vector<TBakePoint>& bakePoints, const BakeParams& bakeParams)
{
    const Light* sunLight = nullptr;
    for (auto& light : raytracingContext.scene->lights)
    {
        if (light->type != LightType::Directional)
            continue;

        sunLight = light.get();
        break;
    }

    const uint32_t lightSampleCount = bakeParams.lightSampleCount * TBakePoint::BASIS_COUNT;

    Matrix3 lightTangentToWorldMatrix;
    if (sunLight != nullptr)
        lightTangentToWorldMatrix = sunLight->getTangentToWorldMatrix();

    std::for_each(std::execution::par_unseq, bakePoints.begin(), bakePoints.end(), [&](TBakePoint& bakePoint)
    {
        if (!bakePoint.valid())
            return;

        bakePoint.begin();

        float faceFactor = 1.0f;

        for (uint32_t i = 0; i < lightSampleCount; i++)
        {
            const Vector3 tangentSpaceDirection = TBakePoint::sampleDirection(i, lightSampleCount, Random::next(), Random::next()).normalized();
            const Vector3 worldSpaceDirection = (bakePoint.tangentToWorldMatrix * tangentSpaceDirection).normalized();
            const Color4 radiance = pathTrace(raytracingContext, bakePoint.position, worldSpaceDirection, bakeParams);

            faceFactor += radiance[3];
            bakePoint.addSample(radiance.head<3>(), tangentSpaceDirection, worldSpaceDirection);
        }

        // If most rays point to backfaces, discard the pixel.
        // This will fix the shadow leaks when dilated.
        if constexpr ((TBakePoint::FLAGS & BAKE_POINT_FLAGS_DISCARD_BACKFACE) != 0)
        {
            if (faceFactor / (float)lightSampleCount < 0.5f)
            {
                bakePoint.discard();
                return;
            }
        }

        bakePoint.end(lightSampleCount);

        if constexpr ((TBakePoint::FLAGS & BAKE_POINT_FLAGS_AO) != 0)
        {
            if (bakeParams.aoStrength > 0.0f)
            {
                // Ambient occlusion
                float ambientOcclusion = 0.0f;

                for (uint32_t i = 0; i < bakeParams.aoSampleCount; i++)
                {
                    const Vector3 tangentSpaceDirection = TBakePoint::sampleDirection(i, bakeParams.aoSampleCount, Random::next(), Random::next()).normalized();
                    const Vector3 worldSpaceDirection = (bakePoint.tangentToWorldMatrix * tangentSpaceDirection).normalized();

                    RTCIntersectContext context {};
                    rtcInitIntersectContext(&context);

                    RTCRayHit query {};
                    query.ray.dir_x = worldSpaceDirection[0];
                    query.ray.dir_y = worldSpaceDirection[1];
                    query.ray.dir_z = worldSpaceDirection[2];
                    query.ray.org_x = bakePoint.position[0];
                    query.ray.org_y = bakePoint.position[1];
                    query.ray.org_z = bakePoint.position[2];
                    query.ray.tnear = 0.001f;
                    query.ray.tfar = INFINITY;
                    query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
                    query.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

                    rtcIntersect1(raytracingContext.rtcScene, &context, &query);

                    if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID)
                        continue;

                    const Vector2 baryUV { query.hit.v, query.hit.u };

                    const Mesh& mesh = *raytracingContext.scene->meshes[query.hit.geomID];
                    const Triangle& triangle = mesh.triangles[query.hit.primID];
                    const Vertex& a = mesh.vertices[triangle.a];
                    const Vertex& b = mesh.vertices[triangle.b];
                    const Vertex& c = mesh.vertices[triangle.c];

                    const Vector3 triNormal(query.hit.Ng_x, query.hit.Ng_y, query.hit.Ng_z);

                    if (mesh.type == MeshType::Opaque && triNormal.dot(worldSpaceDirection) >= 0.0f)
                        continue;

                    float alpha = mesh.type != MeshType::Opaque && mesh.material && mesh.material->textures.diffuse ?
                        mesh.material->textures.diffuse->pickColor(barycentricLerp(a.uv, b.uv, c.uv, baryUV)).w() : 1.0f;

                    if (mesh.type == MeshType::Punch && alpha < 0.5f)
                        continue;

                    ambientOcclusion += 1.0f / (bakeParams.aoFadeConstant + bakeParams.aoFadeLinear * query.ray.tfar + bakeParams.aoFadeQuadratic * query.ray.tfar * query.ray.tfar) * alpha;
                }

                ambientOcclusion = saturate(1.0f - ambientOcclusion / bakeParams.aoSampleCount * bakeParams.aoStrength);

                for (size_t i = 0; i < TBakePoint::BASIS_COUNT; i++)
                    bakePoint.colors[i] *= ambientOcclusion;
            }
        }

        if ((TBakePoint::FLAGS & BAKE_POINT_FLAGS_SHADOW) != 0 && sunLight != nullptr)
        {
            const size_t shadowSampleCount = (TBakePoint::FLAGS & BAKE_POINT_FLAGS_SOFT_SHADOW) != 0 ? bakeParams.shadowSampleCount : 1;

            // Shadows are more noisy when multi-threaded...?
            // Could it be related to the random number generator?
            const float phi = 2 * PI * Random::next();

            for (uint32_t i = 0; i < shadowSampleCount; i++)
            {
                Vector3 direction;

                if constexpr ((TBakePoint::FLAGS & BAKE_POINT_FLAGS_SOFT_SHADOW) != 0)
                {
                    const Vector2 vogelDiskSample = sampleVogelDisk(i, bakeParams.shadowSampleCount, phi);

                    direction = (lightTangentToWorldMatrix * Vector3(
                        vogelDiskSample[0] * bakeParams.shadowSearchRadius,
                        vogelDiskSample[1] * bakeParams.shadowSearchRadius, 1)).normalized();
                }
                else
                {
                    direction = sunLight->positionOrDirection;
                }

                Vector3 position = bakePoint.smoothPosition;

                float shadow = 0;
                size_t depth = 0;

                do
                {
                    RTCIntersectContext context {};
                    rtcInitIntersectContext(&context);

                    RTCRayHit query {};
                    query.ray.dir_x = -direction[0];
                    query.ray.dir_y = -direction[1];
                    query.ray.dir_z = -direction[2];
                    query.ray.org_x = position[0];
                    query.ray.org_y = position[1];
                    query.ray.org_z = position[2];
                    query.ray.tnear = bakeParams.shadowBias;
                    query.ray.tfar = INFINITY;
                    query.hit.geomID = RTC_INVALID_GEOMETRY_ID;
                    query.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

                    rtcIntersect1(raytracingContext.rtcScene, &context, &query);

                    if (query.hit.geomID == RTC_INVALID_GEOMETRY_ID)
                        break;

                    const Mesh& mesh = *raytracingContext.scene->meshes[query.hit.geomID];
                    const Triangle& triangle = mesh.triangles[query.hit.primID];
                    const Vertex& a = mesh.vertices[triangle.a];
                    const Vertex& b = mesh.vertices[triangle.b];
                    const Vertex& c = mesh.vertices[triangle.c];
                    const Vector2 hitUV = barycentricLerp(a.uv, b.uv, c.uv, { query.hit.v, query.hit.u });

                    const float alpha = mesh.type != MeshType::Opaque && mesh.material && mesh.material->textures.diffuse ? 
                        mesh.material->textures.diffuse->pickColor(hitUV)[3] : 1;

                    shadow += (1 - shadow) * (mesh.type == MeshType::Punch ? alpha > 0.5f : alpha);

                    position = barycentricLerp(a.position, b.position, c.position, { query.hit.v, query.hit.u });
                } while (shadow < 1.0f && ++depth < 8); // TODO: Some meshes get stuck in an infinite loop, intersecting each other infinitely. Figure out the solution instead of doing this.

                bakePoint.shadow += shadow;
            }

            bakePoint.shadow = 1 - bakePoint.shadow / shadowSampleCount;
        }
        else
        {
            bakePoint.shadow = 1.0f;
        }
    });
}