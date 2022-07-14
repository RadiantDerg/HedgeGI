﻿#include "BakingFactoryWindow.h"
#include "AppData.h"
#include "FileDialog.h"
#include "Math.h"
#include "OidnDenoiserDevice.h"
#include "OptixDenoiserDevice.h"
#include "Stage.h"
#include "StageParams.h"
#include "StateManager.h"

const Label OUTPUT_DIR_LABEL = { "Output Directory",
    "Directory where resulting files are going to be saved.\n\n"
    "This directory might not exist if the .hgi file was received from a different system." };

const char* const BROWSE_DESC = "Brings a dialog to select a directory where the resulting files are going to be saved.";

const char* const OPEN_IN_EXPLORER_DESC = "Opens the output directory in Explorer.";

const char* const CLEAN_DIR_DESC = "Removes files generated by the baker.";

const Label MODE_GLOBAL_ILLUMINATION_LABEL = { "Global Illumination",
    "Bakes lighting data for static terrain.\n\n"
    "This is the default mode." };

const Label MODE_LIGHT_FIELD_LABEL = { "Light Field",
    "Bakes lighting data for dynamic objects.\n\n"
    "This mode is significantly faster than global illumination mode." };

const Label ENGINE_HE1_LABEL = { "Hedgehog Engine 1",
    "HE1 is used for Sonic Generations and Sonic Lost World." };

const Label ENGINE_HE2_LABEL = { "Hedgehog Engine 2",
    "HE2 is used for Sonic Forces and PBR shaders mod." };

const Label MIN_CELL_RADIUS_LABEL = { "Minimum Cell Radius",
    "Minimum size for a cell in the light field tree.\n\n"
    "Smaller values are going to produce more accurate light field at the cost of higher disk and memory space requirements.\n\n"
    "1-2 is good for accuracy whereas 5-10 is good for disk space and bake speed.\n\n"
    "Please note that you might run out of memory if the cell radius is too small. You should watch your memory usage to ensure this doesn't happen.\n\n"
    "This option has no effect if \"Use Pre-generated Light Field Tree\" is enabled."};

const Label AABB_SIZE_MULTIPLIER_LABEL = { "AABB Size Multiplier",
    "Size multiplier for the area the light field tree covers.\n\n"
    "You can increase this in case the light field fails to cover enough space in the air.\n\n"
    "This option has no effect if \"Use Pre-generated Light Field Tree\" is enabled." };

const Label USE_EXISTING_LIGHT_FIELD_TREE_LABEL = { "Use Pre-generated Light Field Tree",
    "Uses the pre-generated light field tree data contained in stage files.\n\n"
    "This is useful if you want to bake light field for a stage that already contains one.\n\n"
    "The size of the resulting light field data is going to be nearly the same as the original." };

const Label DENOISE_SHADOW_MAP_LABEL = { "Denoise Shadow Map",
    "Denoises the resulting shadow map using the specified denoiser type.\n\n"
    "Disabling this is going to cause shadow maps to look noisy.\n\n"
    "Please note that this option is not supported by oidn yet.\n\n"
    "Recommended to be enabled." };

const Label OPTIMIZE_SEAMS_LABEL = { "Optimize Seams",
    "Tries to smooth hard seams in resulting images.\n\n"
    "You can leave this enabled as it is not a major performance hit.\n\n"
    "Recommended to be enabled." };

const Label SKIP_EXISTING_FILES_LABEL = { "Skip Existing Files",
    "Skips instances that already have their resulting images in the output directory." };

const Label DENOISER_NONE_LABEL = { "None",
    "Disables denoising. This is going to cause resulting images to look really noisy." };

const Label DENOISER_OPTIX_AI_LABEL = { "Optix AI",
    "Denoises on the GPU.\n\n"
    "This runs faster than oidn, but it can have slight artifacts in dark areas. Requires a NVIDIA GPU on the latest drivers.\n\n"
    "Recommended to be used." };

const Label DENOISER_OIDN_LABEL = { "oidn",
    "Denoises on the CPU.\n\n"
    "This runs slower than Optix AI, but it handles dark areas better." };

const Label RESOLUTION_OVERRIDE_LABEL = { "Resolution Override",
    "Makes every instance get baked at the specified resolution, regardless of their original settings.\n\n"
    "Set to -1 to disable this option." };

const Label RESOLUTION_SUPERSAMPLE_SCALE = { "Resolution Supersample Scale",
    "Makes every instance get baked at a higher resolution than the original, and downscales it back to the original resolution.\n\n"
    "This is going to make resulting images look cleaner, but it will take significantly longer to bake." };

const char* const BAKE_DESC = "Bakes the current stage.";

#define PACK_DESC_ "\n\nFor Sonic Generations, please ensure your stage has correctly gone through the Pre-Render pass in GI Atlas Converter."

const char* const BAKE_AND_PACK_DESC = "Bakes the current stage and immediately packs the result into the files." PACK_DESC_;

const char* const PACK_DESC = "Packs the result of a precedent bake into the stage files." PACK_DESC_;

BakingFactoryWindow::BakingFactoryWindow() : visible(true)
{
}

BakingFactoryWindow::~BakingFactoryWindow()
{
    get<AppData>()->getPropertyBag().set(PROP("application.showBakingFactory"), visible);
}

void BakingFactoryWindow::initialize()
{
    visible = get<AppData>()->getPropertyBag().get<bool>(PROP("application.showBakingFactory"), true);
}

void BakingFactoryWindow::update(const float deltaTime)
{
    if (!visible)
        return;

    if (!ImGui::Begin("Baking Factory", &visible))
        return ImGui::End();

    const auto stage = get<Stage>();
    const auto params = get<StageParams>();

    if (ImGui::CollapsingHeader("Output"))
    {
        if (beginProperties("##Baking Factory Settings"))
        {
            char outputDirPath[1024];
            strcpy(outputDirPath, params->outputDirectoryPath.c_str());

            if (property(OUTPUT_DIR_LABEL, outputDirPath, sizeof(outputDirPath), -32))
                params->outputDirectoryPath = outputDirPath;

            ImGui::SameLine();

            if (ImGui::Button("..."))
            {
                if (const std::string newOutputDirectoryPath = FileDialog::openFolder(L"Open Output Folder"); !newOutputDirectoryPath.empty())
                    params->outputDirectoryPath = newOutputDirectoryPath;
            }

            tooltip(BROWSE_DESC);

            endProperties();
        }

        const float buttonWidth = (ImGui::GetWindowSize().x - ImGui::GetStyle().ItemSpacing.x * 2) / 2;

        if (ImGui::Button("Open in Explorer", { buttonWidth, 0 }))
        {
            std::filesystem::create_directory(params->outputDirectoryPath);
            std::system(("explorer \"" + params->outputDirectoryPath + "\"").c_str());
        }

        tooltip(OPEN_IN_EXPLORER_DESC);

        ImGui::SameLine();

        if (ImGui::Button("Clean Directory", { buttonWidth, 0 }))
            stage->clean();
        
        tooltip(CLEAN_DIR_DESC);
    }

    if (ImGui::CollapsingHeader("Baker"))
    {
        if (beginProperties("##Mode Settings"))
        {
            property("Mode",
                {
                    { MODE_GLOBAL_ILLUMINATION_LABEL, BakingFactoryMode::GI },
                    { MODE_LIGHT_FIELD_LABEL, BakingFactoryMode::LightField }
                },
                params->mode
                );

            params->dirty |= property("Engine",
                {
                    { ENGINE_HE1_LABEL, TargetEngine::HE1},
                    { ENGINE_HE2_LABEL, TargetEngine::HE2}
                },
                params->bakeParams.targetEngine
                );

            endProperties();
            ImGui::Separator();
        }

        if (beginProperties("##Mode Specific Settings"))
        {
            if (params->mode == BakingFactoryMode::LightField && params->bakeParams.targetEngine == TargetEngine::HE1)
            {
                property(MIN_CELL_RADIUS_LABEL, ImGuiDataType_Float, &params->bakeParams.lightField.minCellRadius);
                property(AABB_SIZE_MULTIPLIER_LABEL, ImGuiDataType_Float, &params->bakeParams.lightField.aabbSizeMultiplier);
                property(USE_EXISTING_LIGHT_FIELD_TREE_LABEL, params->useExistingLightField);
            }
            else if (params->mode == BakingFactoryMode::GI)
            {
                property(DENOISE_SHADOW_MAP_LABEL, params->bakeParams.postProcess.denoiseShadowMap);
                property(OPTIMIZE_SEAMS_LABEL, params->bakeParams.postProcess.optimizeSeams);
                property(SKIP_EXISTING_FILES_LABEL, params->skipExistingFiles);

                // Denoiser types need special handling since they might not be available
                if (OptixDenoiserDevice::available || OidnDenoiserDevice::available)
                {
                    const Label* labels[] =
                    {
                        &DENOISER_NONE_LABEL,
                        &DENOISER_OPTIX_AI_LABEL,
                        &DENOISER_OIDN_LABEL
                    };

                    const bool flags[] =
                    {
                        true,
                        OptixDenoiserDevice::available,
                        OidnDenoiserDevice::available
                    };

                    beginProperty("Denoiser Type");
                    if (ImGui::BeginCombo("##Denoiser Type", labels[(size_t)params->bakeParams.getDenoiserType()]->name))
                    {
                        for (size_t i = 0; i < _countof(labels); i++)
                        {
                            if (flags[i] && ImGui::Selectable(labels[i]->name))
                                params->bakeParams.postProcess.denoiserType = (DenoiserType)i;

                            tooltip(labels[i]->desc);
                        }
                        ImGui::EndCombo();
                    }
                }

                if (property(RESOLUTION_OVERRIDE_LABEL, ImGuiDataType_S16, &params->bakeParams.resolution.override) && params->bakeParams.resolution.override >= 0)
                    params->bakeParams.resolution.override = (int16_t)nextPowerOfTwo(params->bakeParams.resolution.override);

                if (property(RESOLUTION_SUPERSAMPLE_SCALE, ImGuiDataType_U64, &params->resolutionSuperSampleScale))
                    params->resolutionSuperSampleScale = nextPowerOfTwo(std::max<size_t>(1, params->resolutionSuperSampleScale));
            }

            endProperties();
        }

        const float buttonWidth = (ImGui::GetWindowSize().x - ImGui::GetStyle().ItemSpacing.x * 3) / 3;

        if (ImGui::Button("Bake", { buttonWidth / 2, 0 }))
            get<StateManager>()->bake();

        tooltip(BAKE_DESC);

        ImGui::SameLine();

        if (ImGui::Button("Bake and Pack", { buttonWidth * 2, 0 }))
            get<StateManager>()->bakeAndPack();

        tooltip(BAKE_AND_PACK_DESC);

        ImGui::SameLine();

        if (ImGui::Button("Pack", { buttonWidth / 2, 0 }))
            get<StateManager>()->pack();

        tooltip(PACK_DESC);
    }

    ImGui::End();
}
