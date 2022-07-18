﻿#include "InstanceEditor.h"
#include "Stage.h"
#include "StageParams.h"
#include "Instance.h"
#include "Math.h"

const char* const INSTANCE_SEARCH_DESC = "Type in an instance name to search for.";

const Label SELECTED_INSTANCE_RESOLUTIN_LABEL = { "Selected Instance Resolution",
    "Resolution of the selected instance.\n\n"
    "This value is going to be overriden if \"Compute New Resolutions\" is used." };

const Label RESOLUTION_BASE_LABEL = { "Resolution Base",
    "How much larger the instance needs to be to have double the resolution compared to its smaller counterparts.\n\n"
    "2 is generally a great value to use."};

const Label RESOLUTION_BIAS_LABEL = { "Resolution Bias",
    "Modifies the output resolution of every instance by multiplying it using a number computed from this value.\n\n"
    "Larger values are going to yield larger resolutions.\n\n"
    "1-2 is great for low resolution bakes. Anything higher should be used for high resolution bakes to be shipped in mod releases."};

const Label RESOLUTION_MIN_LABEL = { "Resolution Minimum",
    "The minimum resolution of every instance.\n\n"
    "This should ideally not go any lower than 16."};

const Label RESOLUTION_MAX_LABEL = { "Resolution Maximum",
    "The maximum resolution of every instance.\n\n"
    "This should ideally not go any higher than 2048." };

const char* const COMPUTE_NEW_RESOLUTIONS_DESC =
    "Generates a new resolution for every instance using the parameters defined above.\n\n"
    "This is going to override custom instance resolutions.";

const char* const RESTORE_ORIGINAL_RESOLUTIONS_DESC =
    "Sets the resolution of every instance to their original resolutions contained in stage files.\n\n"
    "This is useful if you want to bake global illumination for a stage that already contains it.\n\n"
    "This is the default behavior when opening a stage in HedgeGI for the first time.";

void InstanceEditor::update(float deltaTime)
{
    if (!ImGui::CollapsingHeader("Instances"))
        return;

    const auto stage = get<Stage>();
    const auto params = get<StageParams>();

    ImGui::SetNextItemWidth(-1);
    const bool doSearch = ImGui::InputText("##SearchInstances", search, sizeof(search)) || strlen(search) > 0;

    tooltip(INSTANCE_SEARCH_DESC);

    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginListBox("##Instances"))
    {
        for (auto& instance : stage->getScene()->instances)
        {
            const uint16_t resolution = instance->getResolution(params->propertyBag);

            char name[1024];
            sprintf(name, "%s (%dx%d)", instance->name.c_str(), resolution, resolution);

            if (doSearch && !strstr(name, search))
                continue;

            if (ImGui::Selectable(name, selection == instance.get()))
                selection = instance.get();
        }

        ImGui::EndListBox();
    }

    if (ImGui::Button("Restore Original Resolutions"))
    {
        for (auto& instance : stage->getScene()->instances)
            instance->setResolution(params->propertyBag, instance->originalResolution);
    }

    tooltip(RESTORE_ORIGINAL_RESOLUTIONS_DESC);

    if (selection != nullptr)
    {
        uint16_t resolution = selection->getResolution(params->propertyBag);
        if (beginProperties("##Instance Settings"))
        {
            if (property(SELECTED_INSTANCE_RESOLUTIN_LABEL, ImGuiDataType_U16, &resolution))
                selection->setResolution(params->propertyBag, nextPowerOfTwo(resolution));

            endProperties();
            ImGui::Separator();
        }
    }

    if (beginProperties("##Resolution Settings"))
    {
        property(RESOLUTION_BASE_LABEL, ImGuiDataType_Float, &params->bakeParams.resolution.base);
        property(RESOLUTION_BIAS_LABEL, ImGuiDataType_Float, &params->bakeParams.resolution.bias);
        property(RESOLUTION_MIN_LABEL, ImGuiDataType_U16, &params->bakeParams.resolution.min);
        property(RESOLUTION_MAX_LABEL, ImGuiDataType_U16, &params->bakeParams.resolution.max);
        endProperties();
    }

    if (ImGui::Button("Compute New Resolutions"))
    {
        for (auto& instance : stage->getScene()->instances)
        {
            uint16_t resolution = nextPowerOfTwo((int)exp2f(params->bakeParams.resolution.bias + logf(getRadius(instance->aabb)) / logf(params->bakeParams.resolution.base)));
            resolution = std::max(params->bakeParams.resolution.min, std::min(params->bakeParams.resolution.max, resolution));

            instance->setResolution(params->propertyBag, resolution);
        }
    }

    tooltip(COMPUTE_NEW_RESOLUTIONS_DESC);
}