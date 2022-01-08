/**
 *  Mana - 3D Game Engine
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MANA_DEBUGWINDOW_HPP
#define MANA_DEBUGWINDOW_HPP

#include <cmath>

#include "imgui.h"
#include "implot.h"

#include "platform/display/window.hpp"

#include "gui/stringformat.hpp"

class DebugWindow {
public:
    struct LayerTreeItem {
        LayerTreeItem() = default;

        LayerTreeItem(int order, const Compositor::Layer &layer)
                : order(order), layer(layer) {}

        int order = 0;
        bool active = true;
        bool pinned = false;
        Compositor::Layer layer;
    };

    void swapItems(int item0, int item1) {
        auto bitem0 = items.at(item0);
        auto bitem1 = items.at(item1);
        bitem0.order = item1;
        bitem1.order = item0;
        items.at(item0) = bitem1;
        items.at(item1) = bitem0;
    }

    void drawLayerNodeButtons(LayerTreeItem &item) {
        if (!item.active) {
            ImGui::SameLine();
            if (ImGui::SmallButton(
                    ("Enable###0_" + std::to_string(item.order)).c_str())) {
                item.active = true;
            }
        } else if (!item.pinned) {
            ImGui::SameLine();
            if (ImGui::SmallButton(
                    ("Disable###1_" + std::to_string(item.order)).c_str())) {
                item.active = false;
            }

            ImGui::SameLine();
            if (ImGui::SmallButton(
                    ("Up###2_" + std::to_string(item.order)).c_str())) {
                auto nIndex = item.order;

                bool foundDeactivated = false;
                //Decrement nIndex until it doesnt point to a deactivated item
                while (true) {
                    if (--nIndex < 0)
                        nIndex = static_cast<int>(items.size() - 1);
                    if (nIndex == item.order) {
                        break;
                    } else if (items.at(nIndex).active && !items.at(nIndex).pinned) {
                        foundDeactivated = true;
                        break;
                    }
                }

                if (foundDeactivated)
                    swapItems(nIndex, item.order);
            }

            ImGui::SameLine();
            if (ImGui::SmallButton(
                    ("Down###3_" + std::to_string(item.order)).c_str())) {
                auto nIndex = item.order;

                bool foundDeactivated = false;
                //Increment nIndex until it doesnt point to a deactivated item
                while (true) {
                    if (++nIndex >= items.size())
                        nIndex = 0;
                    if (nIndex == item.order) {
                        break;
                    } else if (items.at(nIndex).active && !items.at(nIndex).pinned) {
                        foundDeactivated = true;
                        break;
                    }
                }

                if (foundDeactivated)
                    swapItems(nIndex, item.order);
            }
        }
    }

    void drawLayerNode(int nodeIndex, LayerTreeItem &item) {
        if (ImGui::TreeNode((void *) (intptr_t) nodeIndex, "%s", item.layer.name.c_str())) {
            drawLayerNodeButtons(item);

            ImGui::Text("Color Texture: %s", item.layer.color.c_str());

            if (!item.layer.depth.empty())
                ImGui::Text("Depth Texture: %s", item.layer.depth.c_str());
            else
                ImGui::Text("No Depth Texture");

            ImGui::Text("Depth Test:    %s",
                        formatDepthTestMode(item.layer.depthTestMode).c_str());
            ImGui::Text("Blend Source:  %s",
                        formatBlendMode(item.layer.colorBlendModeSource).c_str());
            ImGui::Text("Blend Destination: %s",
                        formatBlendMode(item.layer.colorBlendModeDest).c_str());

            ImGui::TreePop();
        } else {
            drawLayerNodeButtons(item);
        }
    }

    void drawFrameTimeGraph() {
        if (ImPlot::BeginPlot("Frame Graph")) {
            std::vector<float> x;
            for (int i = 0; i < frameRateHistory.size(); i++)
                x.emplace_back(i);

            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);
            ImPlot::SetupAxisLimits(::ImAxis_X1, 0, 100);

            ImPlot::PlotLine("Frames Per Second", x.data(), frameRateHistory.data(), frameRateHistory.size());
            ImPlot::PlotLine("Frame Time (ms)", x.data(), frameTimeHistory.data(), frameTimeHistory.size());
            ImPlot::PlotLine("Number of Draw Calls", x.data(), drawCallHistory.data(), drawCallHistory.size());

            ImPlot::EndPlot();
        }
    }

    void draw() {
        frameRateHistory.emplace(frameRateHistory.begin(), ImGui::GetIO().Framerate);
        if (frameRateHistory.size() >= 10000)
            frameRateHistory.erase(frameRateHistory.end() - 1);

        frameTimeHistory.emplace(frameTimeHistory.begin(), 1000.0f / ImGui::GetIO().Framerate);
        if (frameTimeHistory.size() >= 10000)
            frameTimeHistory.erase(frameTimeHistory.end() - 1);

        drawCallHistory.emplace(drawCallHistory.begin(), drawCalls);
        if (drawCallHistory.size() >= 10000)
            drawCallHistory.erase(drawCallHistory.end() - 1);

        ImGui::StyleColorsDark();

        ImGui::Begin("Debug Window");

        ImGui::BeginTabBar("TabBar");

        if (ImGui::BeginTabItem("Rendering")) {
            if(ImGui::TreeNode("Window")){
                std::vector<std::string> modes;
                std::vector<const char*> cModeStr;
                for (auto &mode: videoModes) {
                    std::string str = std::to_string(mode.width);
                    str += "x";
                    str += std::to_string(mode.height);
                    str += "@";
                    str += std::to_string(mode.refreshRate);
                    str += "            "; // ImGui::ListBox seems to be bugged the items need to have a minimum length otherwise random chars are displayed

                    modes.emplace_back(str);
                    cModeStr.emplace_back((modes.end() - 1)->c_str());
                }

                ImGui::ListBox("",
                               &selectedVideoMode,
                               reinterpret_cast<const char *const *>(cModeStr.data()),
                               cModeStr.size());
                ImGui::Checkbox("Fullscreen", &fullscreen);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Render Layers")) {
                std::map<int, std::vector<std::reference_wrapper<LayerTreeItem>>> activeItems;
                std::vector<std::reference_wrapper<LayerTreeItem>> inactiveItems;
                std::vector<std::reference_wrapper<LayerTreeItem>> pinnedItems;

                for (auto &item: items) {
                    if (item.pinned)
                        pinnedItems.emplace_back(item);
                    else if (item.active)
                        activeItems[item.order].emplace_back(item);
                    else
                        inactiveItems.emplace_back(item);
                }

                if (ImGui::TreeNode("Active")) {
                    int nodeIndex = 0;
                    for (auto &pair: activeItems) {
                        for (auto &item: pair.second)
                            drawLayerNode(nodeIndex++, item);
                    }
                    for (auto &item: pinnedItems) {
                        drawLayerNode(nodeIndex++, item);
                    }
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Inactive")) {
                    int nodeIndex = 0;
                    for (auto &item: inactiveItems)
                        drawLayerNode(nodeIndex++, item);
                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            ImGui::Separator();

            int res[2];

            res[0] = frameBufferSize.x;
            res[1] = frameBufferSize.y;
            ImGui::InputInt2("Framebuffer Resolution", res, ImGuiInputTextFlags_ReadOnly);

            res[0] = (int) ((float) frameBufferSize.x * resScale);
            res[1] = (int) ((float) frameBufferSize.y * resScale);
            ImGui::InputInt2("Render Resolution", res, ImGuiInputTextFlags_ReadOnly);

            ImGui::SliderFloat("Resolution Scale", &resScale, 0.1, 3, "%.1f");

            ImGui::InputInt("MSAA Samples", &samples);
            if (samples > maxSamples)
                samples = maxSamples;
            else if (samples < 1)
                samples = 1;

            ImGui::InputInt("Swap Interval", &swapInterval);

            if (swapInterval < 0)
                swapInterval = 0;

            ImGui::InputFloat("FPS Limit", &fpsLimit);

            if (fpsLimit < 0)
                fpsLimit = 0;

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Profiling")) {
            drawFrameTimeGraph();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Camera")) {
            ImGui::InputFloat3("Position", (float *) (&camera.transform.getPosition()), "%.3f");
            auto euler = camera.transform.getRotation().getEulerAngles();
            ImGui::InputFloat3("Rotation", (float *) (&euler), "%.3f");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();

        ImGui::Separator();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS, %ld Draw Calls, %ld Polygons)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate,
                    drawCalls,
                    polyCount);

        ImGui::End();
    }

    std::vector<Compositor::Layer> getSelectedLayers() const {
        std::vector<Compositor::Layer> pin;
        std::map<int, std::vector<Compositor::Layer>> tmp;
        for (auto &item: items)
            if (item.active) {
                if (item.pinned) {
                    pin.emplace_back(item.layer);
                } else {
                    tmp[item.order].emplace_back(item.layer);
                }
            }

        std::vector<Compositor::Layer> ret;
        for (auto &pair: tmp)
            for (auto &layer: pair.second)
                ret.emplace_back(layer);
        for (auto &layer: pin)
            ret.emplace_back(layer);
        return ret;
    }

    void resetSelection() {
        int i = 0;
        for (auto &item: items) {
            item.order = i++;
            item.active = true;
        }
    }

    void setLayers(const std::vector<Compositor::Layer> &l) {
        items.clear();
        int i = 0;
        for (auto &layer: l) {
            items.emplace_back(LayerTreeItem(i++, static_cast<Compositor::Layer>(layer)));
        }
    }

    void setLayerActive(const std::string &name, bool active) {
        for (auto &item: items) {
            if (item.layer.name == name) {
                item.active = active;
                break;
            }
        }
    }

    bool getLayerActive(const std::string &name) const {
        for (auto &item: items) {
            if (item.layer.name == name) {
                return item.active;
            }
        }
        throw std::runtime_error("Invalid layer " + name);
    }

    void setLayerPinned(const std::string &name, bool pin) {
        for (auto &item: items) {
            if (item.layer.name == name) {
                item.pinned = pin;
                break;
            }
        }
    }

    bool getLayerPinned(const std::string &name) const {
        for (auto &item: items) {
            if (item.layer.name == name) {
                return item.pinned;
            }
        }
        throw std::runtime_error("Invalid layer " + name);
    }

    void setSamples(int value) {
        samples = value;
    }

    int getSamples() const {
        return samples;
    }

    void setMaxSamples(int value) {
        maxSamples = value;
        if (samples > value)
            samples = value;
    }

    int getSwapInterval() const {
        return swapInterval;
    }

    void setDrawCalls(unsigned long value) {
        drawCalls = value;
    }

    void setFpsLimit(float value) {
        fpsLimit = value;
    }

    float getFpsLimit() const {
        return fpsLimit;
    }

    void setScene(EntityManager &value) {
    }

    void setPolyCount(size_t value) {
        polyCount = value;
    }

    void setFrameBufferSize(Vec2i size) {
        frameBufferSize = size;
    }

    Vec2i getRenderResolution() const {
        Vec2i ret;
        ret.x = (int) ((float) frameBufferSize.x * resScale);
        ret.y = (int) ((float) frameBufferSize.y * resScale);
        return ret;
    }

    void setCamera(const Camera &val) {
        camera = val;
    }

    void setVideoModes(const std::vector<VideoMode> &value) {
        videoModes = value;
    }

    int getSelectedVideoMode() {
        return selectedVideoMode;
    }

    bool getFullScreen() {
        return fullscreen;
    }

private:
    std::string appendButtonLabelId(const std::string &label, int index) {
        return label + "###" + std::to_string(index);
    }

    std::vector<LayerTreeItem> items;
    int maxSamples = 0;
    int samples = 0;
    int swapInterval = 0;
    unsigned long drawCalls = 0;
    float fpsLimit = 0;
    size_t polyCount = 0;
    float resScale = 1;
    Vec2i frameBufferSize = {};
    Camera camera;
    std::vector<VideoMode> videoModes;
    int selectedVideoMode = 0;
    bool fullscreen = false;

    std::vector<float> frameRateHistory;
    std::vector<float> frameTimeHistory;
    std::vector<float> drawCallHistory;
};

#endif //MANA_DEBUGWINDOW_HPP
