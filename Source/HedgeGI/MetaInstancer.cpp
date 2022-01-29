﻿#include "MetaInstancer.h"
#include "Math.h"
#include "Utilities.h"

struct MtiHeader
{
    uint32_t signature;
    uint32_t version;
    uint32_t instanceCount;
    uint32_t instanceSize;
    uint32_t unused0;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t instancesOffset;
};

struct MtiInstance
{
    float positionX;
    float positionY;
    float positionZ;

    uint8_t type;
    uint8_t sway;

    // Gets applied after sway animation
    uint8_t pitchAfterSway;
    uint8_t yawAfterSway;

    // Gets applied before sway animation
    int16_t pitchBeforeSway;
    int16_t yawBeforeSway;

    // Shadow
    uint8_t colorA;

    // GI color
    uint8_t colorR;
    uint8_t colorG;
    uint8_t colorB;
};

MetaInstancer::Instance::Instance()
    : position(Vector3::Zero()), type(0), sway(1), rotation(Quaternion::Identity()), color(Color4::Ones())
{
}

void MetaInstancer::save(hl::stream& stream)
{
    MtiHeader mtiHeader =
    {
        0x4D544920, // signature
        1, // version
        (uint32_t)instances.size(), // instanceCount
        sizeof(MtiInstance), // instanceSize
        0, // unused0
        0, // unused1
        0, // unused2,
        sizeof(MtiHeader) // instancesOffset
    };

    hl::endian_swap(mtiHeader.signature);
    hl::endian_swap(mtiHeader.version);
    hl::endian_swap(mtiHeader.instanceCount);
    hl::endian_swap(mtiHeader.instanceSize);
    hl::endian_swap(mtiHeader.instancesOffset);

    stream.write_obj(mtiHeader);

    for (auto& instance : instances)
    {
        const Vector3 rotation = instance.rotation.toRotationMatrix().eulerAngles(0, 1, 0);

        MtiInstance mtiInstance =
        {
            instance.position.x(), // positionX
            instance.position.y(), // positionY
            instance.position.z(), // positionZ
            instance.type, // type
            (uint8_t)meshopt_quantizeUnorm(instance.sway, 8), // sway
            (uint8_t)meshopt_quantizeUnorm(fmodf(rotation.x(), 2 * PI) / (2 * PI), 8), // pitchAfterSway
            0, // yawAfterSway
            (int16_t)meshopt_quantizeSnorm(rotation.z() / PI, 16), // pitchBeforeSway
            (int16_t)meshopt_quantizeSnorm(rotation.y() / PI, 16), // yawBeforeSway
            (uint8_t)meshopt_quantizeUnorm(instance.color.w(), 8), // colorA
            (uint8_t)meshopt_quantizeUnorm(instance.color.x(), 8), // colorR
            (uint8_t)meshopt_quantizeUnorm(instance.color.y(), 8), // colorG
            (uint8_t)meshopt_quantizeUnorm(instance.color.z(), 8) // colorB
        };

        hl::endian_swap(mtiInstance.positionX);
        hl::endian_swap(mtiInstance.positionY);
        hl::endian_swap(mtiInstance.positionZ);
        hl::endian_swap(mtiInstance.pitchBeforeSway);
        hl::endian_swap(mtiInstance.yawBeforeSway);

        stream.write_obj(mtiInstance);
    }
}

void MetaInstancer::save(const std::string& filePath)
{
    hl::file_stream stream(toNchar(filePath.c_str()).data(), hl::file::mode::write);
    save(stream);
}