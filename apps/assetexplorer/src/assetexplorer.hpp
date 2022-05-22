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
        auto passes = std::vector<std::shared_ptr<RenderPass>>();
        passes.emplace_back(new GBufferPass(*renderDevice));
        passes.emplace_back(new PhongPass(*renderDevice));
        passes.emplace_back(new CompositePass(*renderDevice, ColorRGBA::grey(0.5, 255)));
        pipeline = std::make_unique<FrameGraphPipeline>(*renderDevice);
        pipeline->setPasses(passes);

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
            viewRotation = (Quaternion(Vec3f(-mouseDelta.y * 50 * deltaTime, -mouseDelta.x * 50 * deltaTime, 0))
                            * Quaternion(viewRotation)).getEulerAngles();
        }
        mouseDelta = {};

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
        } else if (index == typeid(Material)) {
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
                bundle = ResourceImporter().import(stream, std::filesystem::path(path).extension());
                mesh = renderDevice->getAllocator().createMeshBuffer(bundle.get<Mesh>());
            }
        }

        for (auto &pair: bundle.assets) {
            drawNode(pair.first, *pair.second);
        }

        ImGui::End();

        ImGui::Render();

        ImGuiCompat::DrawData(wnd,
                              target,
                              RenderOptions({}, target.getSize(), false, false, 1, {}, 1, false, false, false),
                              graphicsBackend);
    }

    void drawNode(const std::string &text, Resource &asset) {
        auto type = getType(asset.getTypeIndex());
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
                default:
                    break;
            }

            ImGui::TreePop();
        }
    }

    void drawViewport() {
        auto &target = window->getRenderTarget(graphicsBackend);
        auto winSize = target.getSize();
        auto aspectRatio = (float) winSize.x / (float) winSize.y;

        Scene s;
        s.camera.type = xengine::PERSPECTIVE;
        s.camera.transform.setPosition({0, 0, viewDistance});
        s.camera.aspectRatio = aspectRatio;
        s.camera.farClip = 10000;

        s.skybox.color = ColorRGBA::blue();

        auto light = Light(LightType::LIGHT_DIRECTIONAL);
        light.direction = {0, 0, -1.0f};
        light.ambient = Vec3f(0.5f);
        s.lights.emplace_back(light);

        if (mesh) {
            Material mat;
            mat.diffuse = ColorRGBA::white(0.1);
            Scene::Object o(ResourceHandle<Mesh>({}, nullptr, dynamic_cast<Resource *>(mesh.get())),
                            ResourceHandle<Material>());
            o.transform = {{}, viewRotation, {1, 1, 1}};
            s.objects.emplace_back(o);
        }

        pipeline->setRenderResolution(winSize);
        pipeline->setRenderSamples(4);
        pipeline->render(target, s);
    }

    void onMouseMove(double xPos, double yPos) override {
        auto &input = window->getInput();
        auto &mouse = input.getMice().begin()->second;
        mouseDelta = Vec2d(prevMousePos.x - xPos, prevMousePos.y - yPos);
        prevMousePos = {xPos, yPos};
    }

    void onMouseWheelScroll(double amount) override {
        viewDistance -= amount * 10;
        if (viewDistance < 1) {
            viewDistance = 1;
        } else if (viewDistance > 10000) {
            viewDistance = 10000;
        }
    }

    std::string path;
    ResourceBundle bundle;

    Scene scene;

    float guiWidth;

    std::unique_ptr<MeshBuffer> mesh;

    Vec3f viewRotation;
    float viewDistance = 10;

    Vec2d mouseDelta;
    Vec2d prevMousePos;

    std::unique_ptr<FrameGraphPipeline> pipeline;
};

#endif //XSAMPLES_ASSETEXPLORER_HPP
