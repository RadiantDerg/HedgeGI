#pragma once

// DirectXTex
#include <DirectXTex.h>

// Embree
#include <rtcore.h>

// HedgeLib
#include <hedgelib/hl_internal.h>
#include <hedgelib/hl_math.h>
#include <hedgelib/hl_endian.h>
#include <hedgelib/hl_text.h>
#include <hedgelib/io/hl_file.h>
#include <hedgelib/io/hl_hh.h>
#include <hedgelib/io/hl_path.h>
#include <hedgelib/io/hl_stream.h>
#include <hedgelib/archives/hl_archive.h>
#include <hedgelib/archives/hl_hh_archive.h>
#include <hedgelib/archives/hl_pacx.h>
#include <hedgelib/models/hl_hh_model.h>
#include <hedgelib/materials/hl_hh_material.h>
#include <hedgelib/textures/hl_hh_texture.h>
#include <hedgelib/terrain/hl_hh_terrain_instance_info.h>

#include "hl_hh_light.h"
#include "hl_hh_shlf.h"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unsupported/Eigen/AlignedVector3>

using Vector2 = Eigen::Vector2f;
using Vector3 = Eigen::AlignedVector3<float>;
using Vector4 = Eigen::Vector4f;
using Color3 = Eigen::Array3f;
using Color4 = Eigen::Array4f;
using Matrix3 = Eigen::Matrix3f;
using Matrix4 = Eigen::Matrix4f;
using Affine3 = Eigen::Affine3f;
using AABB = Eigen::AlignedBox3f;

// std
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <execution>
#include <filesystem>
#include <random>
#include <unordered_set>
#include <vector>

// parallel_hashmap
#include <phmap.h>

// Other
#include <INIReader.h>

#undef min
#undef max

// HedgeGI
#include "FileStream.h"
#include "Math.h"
#include "Path.h"
#include "Random.h"
#include "RaytracingDevice.h"