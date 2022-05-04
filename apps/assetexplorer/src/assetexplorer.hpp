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

#include "xengine.hpp"

using namespace xengine;

class AssetExplorer : public Application, InputListener {
public:
    AssetExplorer(int argc, char *argv[])
            : Application(argc, argv) {
        window->setTitle("Asset Explorer");
        auto passes = std::vector<std::unique_ptr<RenderPass>>();
        passes.emplace_back(new PhongPass(*renderDevice));
        pipeline = std::make_unique<DeferredPipeline>(*renderDevice, std::move(passes));

        pipeline->getCompositor().setClearColor(ColorRGBA::grey(0.5, 255));

        window->getInput().addListener(*this);
    }

    ~AssetExplorer() {
        window->getInput().removeListener(*this);
    }

protected:
    void update(float deltaTime) override {
        auto &mouse = window->getInput().getMice().begin()->second;
        if (mouse.getButton(xengine::LEFT)
            && mouse.position.x > guiWidth + 10) {
            viewRotation.x -= deltaTime * mouseDelta.y * 50;
            viewRotation.y -= deltaTime * mouseDelta.x * 50;
        }

        drawViewport();
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

        ImGui::StyleColorsDark();

        ImGuiCompat::NewFrame(wnd, graphicsBackend);
        ImGui::NewFrame();

        ImGui::Begin("Contents", nullptr, ImGuiWindowFlags_NoMove);

        if (guiWidth > target.getSize().x)
            guiWidth = target.getSize().x;
        else
            guiWidth = ImGui::GetWindowSize().x;

        ImGui::SetWindowSize({guiWidth, (float) target.getSize().y});
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
                if (bundle.assets.find(typeid(Mesh)) != bundle.assets.end()) {
                    mesh = renderDevice->getAllocator().createMeshBuffer(bundle.get<Mesh>());
                }
            }
        }

        for (auto &pair: bundle.assets) {
            auto t = getType(pair.first);
            for (auto &mPair: pair.second) {
                drawNode(t, mPair.first, *mPair.second.front());
            }
        }

        ImGui::End();

        ImGui::Render();

        ImGuiCompat::DrawData(wnd,
                              target,
                              RenderOptions({}, target.getSize(), false, false, 1, {}, 1, false, false, false),
                              graphicsBackend);
    }

    void drawNode(AssetType type, const std::string &text, Asset &asset) {
        if (ImGui::TreeNode(text.c_str())) {
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

            switch (type) {
                case MESH:
                    if (ImGui::Button("Display Mesh")) {
                        mesh = renderDevice->getAllocator().createMeshBuffer(dynamic_cast<const Mesh &>(asset));
                    }
                    break;
            }

            ImGui::TreePop();
        }
    }

    void drawViewport() {
        auto winSize = window->getRenderTarget(graphicsBackend).getSize();
        auto aspectRatio = (float) winSize.x / (float) winSize.y;

        Scene s;
        s.camera.type = xengine::PERSPECTIVE;
        s.camera.transform.setPosition({0, 0, viewDistance});
        s.camera.aspectRatio = aspectRatio;

        s.skybox.color = ColorRGBA::blue();

        auto light = Light(LightType::LIGHT_DIRECTIONAL);
        light.direction = {0, 0, -1.0f};
        light.ambient = Vec3f(0.5f);
        s.lights.emplace_back(light);

        if (mesh) {
            Material mat;
            mat.diffuse = ColorRGBA::white(0.1);
            s.nodes.emplace_back(Scene::Node({{}, viewRotation, {1, 1, 1}}, mesh.get(), mat));
        }

        pipeline->getGeometryBuffer().setSamples(4);
        pipeline->getGeometryBuffer().setSize(window->getRenderTarget(graphicsBackend).getSize());

        pipeline->render(window->getRenderTarget(graphicsBackend), s);
    }

    void onMouseMove(double xPos, double yPos) override {
        auto &input = window->getInput();
        auto &mouse = input.getMice().begin()->second;
        mouseDelta = Vec2d(prevMousePos.x - xPos, prevMousePos.y - yPos);
        prevMousePos = {xPos, yPos};
    }

    void onMouseWheelScroll(double amount) override {
        viewDistance -= amount;
        if (viewDistance < 1) {
            viewDistance = 1;
        } else if (viewDistance > 10000) {
            viewDistance = 10000;
        }
    }

    std::string path;
    AssetBundle bundle;

    float guiWidth;

    std::unique_ptr<MeshBuffer> mesh;

    Vec3f viewRotation;
    float viewDistance = 10;

    Vec2d mouseDelta;
    Vec2d prevMousePos;

    std::unique_ptr<DeferredPipeline> pipeline;
};

#endif //XSAMPLES_ASSETEXPLORER_HPP