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
#ifndef XSAMPLES_ASSETEXPLORER_HPP
#define XSAMPLES_ASSETEXPLORER_HPP

#include <fstream>
#include <filesystem>

#include "app/application.hpp"

#include "asset/assetimporter.hpp"

#include "compat/imguicompat.hpp"

using namespace xengine;

class AssetExplorer : public Application {
public:
    AssetExplorer(int argc, char *argv[])
            : Application(argc, argv) {
        window->setTitle("Asset Explorer");
    }

protected:
    void update(float deltaTime) override {
        drawGui();
        Application::update(deltaTime);
    }

private:
    enum AssetType {
        UNKNOWN,
        MESH,
        MATERIAL,
        TEXTURE,
        AUDIO
    };

    AssetType getType(std::type_index index) {
        if (index == typeid(Mesh)) {
            return MESH;
        } else if (index == typeid(AssetMaterial)) {
            return MATERIAL;
        } else if (index == typeid(Texture)) {
            return TEXTURE;
        } else if (index == typeid(Audio)) {
            return AUDIO;
        } else {
            return UNKNOWN;
        }
    }

    void drawGui() {
        auto &wnd = *window;
        auto &target = window->getRenderTarget(graphicsBackend);
        auto &ren = renderDevice->getRenderer();

        ren.renderClear(target, ColorRGBA::black(), 1);

        ImGui::StyleColorsDark();

        ImGuiCompat::NewFrame(wnd, graphicsBackend);
        ImGui::NewFrame();

        ImGui::Begin("Contents", nullptr, ImGuiWindowFlags_NoMove);

        ImGui::SetWindowSize({ImGui::GetWindowSize().x, (float) target.getSize().y});
        ImGui::SetWindowPos({0, 0});

        bool loadAsset = ImGui::Button("Reload Asset");

        std::string buffer = path;
        buffer.resize(5046);

        if (ImGui::InputText("Path", buffer.data(), buffer.size()))
            path = buffer;

        if (loadAsset) {
            std::ifstream stream(path);
            if (stream.is_open()) {
                bundle = AssetImporter::import(stream, std::filesystem::path(path).extension());
            }
        }

        for (auto &pair: bundle.assets) {
            auto t = getType(pair.first);
            for (auto &mPair: pair.second) {
                drawNode(t, mPair.first);
            }
        }

        ImGui::End();

        ImGui::Render();

        ImGuiCompat::DrawData(wnd,
                              target,
                              RenderOptions({}, target.getSize(), false, false, 1, {}, 1, false, false, false),
                              graphicsBackend);
    }

    void drawNode(AssetType type, const std::string &text) {
        if (ImGui::TreeNode(text.c_str())) {
            ImGui::SameLine();
            std::string str;
            switch (type) {
                case UNKNOWN:
                    str = "UNKNOWN";
                    break;
                case MESH:
                    str = "MESH";
                    break;
                case MATERIAL:
                    str = "MATERIAL";
                    break;
                case TEXTURE:
                    str = "TEXTURE";
                    break;
                case AUDIO:
                    str = "AUDIO";
                    break;
            }
            ImGui::Text("%s", str.c_str());
            ImGui::TreePop();
        }
    }

    std::string path;
    AssetBundle bundle;

    AssetType type;
    size_t index;
};

#endif //XSAMPLES_ASSETEXPLORER_HPP
