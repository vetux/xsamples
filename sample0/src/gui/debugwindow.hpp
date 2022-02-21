/**
 *  xEngine-Samples - Example applications demonstrating the xEngine
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

#include "display/window.hpp"

#include "gui/stringformat.hpp"

class DebugWindow {
public:
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

        if (ImGui::BeginTabItem("Settings")) {
            if (ImGui::TreeNode("Window")) {
                std::vector<std::string> modes;
                std::vector<const char *> cModeStr;
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

                ImGui::InputInt("Swap Interval", &swapInterval);

                if (swapInterval < 0)
                    swapInterval = 0;

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Render Pipeline")) {
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

                ImGui::InputFloat("FPS Limit", &fpsLimit);

                if (fpsLimit < 0)
                    fpsLimit = 0;

                ImGui::Checkbox("Draw debug overlay", &drawDebug);

                ImGui::TreePop();
            }

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

    bool getDrawDebug() {
        return drawDebug;
    }

private:
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
    bool drawDebug = false;

    std::vector<float> frameRateHistory;
    std::vector<float> frameTimeHistory;
    std::vector<float> drawCallHistory;
};

#endif //MANA_DEBUGWINDOW_HPP
