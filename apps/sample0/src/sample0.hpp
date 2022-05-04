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

#ifndef MANA_SAMPLEAPPLICATION_HPP
#define MANA_SAMPLEAPPLICATION_HPP

#include "xengine.hpp"

#include <filesystem>
#include <memory>
#include <fstream>

#include "systems/playerinputsystem.hpp"
#include "components/playercontrollercomponent.hpp"
#include "systems/transformanimationsystem.hpp"
#include "components/transformanimationcomponent.hpp"

#include "gui/debugwindow.hpp"

#include "io/byte.hpp"

#include <iostream>

using namespace xengine;

// Helper function to create pak files from a specified directory, with hardcoded file name format.
// In a real application the user would have to decide on a strategy on how / if to store split paks.
// The pak file format retrieval complexity should not be affected by the pak file size,
// the use of pak splitting is for example when a filesystem does not support large files or
// cloud storage with file size limits.
static void createPackFromDirectory(const std::string &dir, long chunkSize) {
    auto entries = Pak::readEntries(dir);
    auto chunks = Pak::createPak(entries, chunkSize);

    auto cs = dir + ".";
    auto ce = ".pak";

    int ci = 0;
    for (auto &c: chunks) {
        auto fileName = cs;
        fileName += std::to_string(ci);
        fileName += ce;

        std::ofstream fs(fileName);
        fs.write(chunks.at(ci).data(), chunks.at(ci).size());
        ci++;
    }
}

static std::unique_ptr<PakArchive> loadPackArchive(const std::string &pakName, Archive &archive) {
    std::vector<std::unique_ptr<std::istream>> streams;

    auto cs = pakName + ".";
    auto ce = ".pak";

    for (int i = 0; i < 100; i++) {
        std::string fileName = "/";
        fileName += cs;
        fileName += std::to_string(i);
        fileName += ce;

        if (archive.exists(fileName)) {
            streams.emplace_back(archive.open(fileName));
        }
    }

    return std::make_unique<PakArchive>(std::move(streams));
}

class Sample0 : public Application, InputListener {
public:
    Sample0(int argc, char *argv[])
            : Application(argc,
                          argv),
                          archive(std::make_unique<DirectoryArchive>(std::filesystem::current_path().string() + "/assets")){
        imPlotContext = ImPlot::CreateContext();

        window->setSwapInterval(0);
        renderDevice->getRenderer().renderClear(window->getRenderTarget(graphicsBackend), bgColor, 1);
        window->swapBuffers();

        ren2d = std::make_unique<Renderer2D>(*renderDevice);
    }

    ~Sample0() override {
        ImPlot::DestroyContext(imPlotContext);
    };

protected:
    void start() override {
        {
            auto s = archive->open("fonts/roboto/Roboto-Regular.ttf");
            font = Font::createFont(*s);
        }

        textRenderer = std::make_unique<TextRenderer>(*font, *renderDevice);

        assetManager = std::make_unique<AssetManager>(*archive);
        assetRenderManager = std::make_unique<AssetRenderManager>(*assetManager, renderDevice->getAllocator());

        std::vector<std::unique_ptr<RenderPass>> passes;
        drawLoadingScreen(0.1, "Loading Passes...");
        passes.emplace_back(std::move(std::make_unique<SkyboxPass>(*renderDevice)));
        drawLoadingScreen(0.2, "Loading Passes...");
        passes.emplace_back(std::move(std::make_unique<PhongPass>(*renderDevice)));
        drawLoadingScreen(0.3, "Loading Passes...");
        passes.emplace_back(std::move(std::make_unique<ForwardPass>(*renderDevice)));
        drawLoadingScreen(0.4, "Loading Passes...");
        passes.emplace_back(std::move(std::make_unique<DebugPass>(*renderDevice)));
        drawLoadingScreen(0.5, "Loading Passes...");

        pipeline = std::make_unique<DeferredPipeline>(*renderDevice,
                                                      std::move(passes));

        drawLoadingScreen(0.6, "Initializing Systems...");

        audioDevice = AudioDevice::createDevice(xengine::OpenAL);

        renderSystem = new RenderSystem(window->getRenderTarget(graphicsBackend),
                                        *renderDevice,
                                        *archive,
                                        *assetManager,
                                        *assetRenderManager,
                                        *pipeline);

        //Move is required because the ECS destructor deletes the system pointers.
        ecs = std::move(ECS(
                {
                        new PlayerInputSystem(window->getInput()),
                        new TransformAnimationSystem(),
                        new AudioSystem(*audioDevice, *assetManager),
                        renderSystem
                }
        ));
        ecs.start();

        drawLoadingScreen(0.7, "Loading Scene...");

        int maxSamples = renderDevice->getMaxSampleCount();
        debugWindow.setMaxSamples(maxSamples);
        debugWindow.setSamples(1);

        auto &device = *renderDevice;

        auto stream = archive->open("/scene.json");
        ecs.getEntityManager() << JsonProtocol().deserialize(*stream);

        auto &entityManager = ecs.getEntityManager();
        auto &componentManager = entityManager.getComponentManager();

        cameraEntity = entityManager.getByName("MainCamera");

        componentManager.create<PlayerControllerComponent>(cameraEntity);

        auto islandEntity = entityManager.getByName("Island");
        componentManager.create<TransformAnimationComponent>(islandEntity, {{},
                                                                            {0, 0, 0}});

        auto planeEntity = entityManager.getByName("Plane");
        componentManager.create<TransformAnimationComponent>(planeEntity, {{},
                                                                           {5.57281, 4.985, 7.78}});

        auto sphereEntity = entityManager.getByName("Sphere");
        componentManager.create<TransformAnimationComponent>(sphereEntity, {{},
                                                                            {7.151281, 61.985, 24.78}});
        window->getInput().addListener(*this);

        drawLoadingScreen(1, "Loading Finished!");

        Application::start();
    }

    void stop() override {
        window->getInput().removeListener(*this);

        ecs.getEntityManager().clear();
        ecs.stop();
        ecs = ECS();

        debugWindow = {};

        Application::stop();
    }

    void update(float deltaTime) override {
        auto frameStart = std::chrono::high_resolution_clock::now();

        auto &wnd = *window;

        wnd.update();

        auto wndSize = wnd.getFramebufferSize();

        auto &entityManager = ecs.getEntityManager();
        auto &componentManager = entityManager.getComponentManager();

        if (deltaTime > 0) {
            float fps = 1.0f / deltaTime;
            float alpha = 0.9;
            fpsAverage = alpha * fpsAverage + (1.0 - alpha) * fps;
        }

        debugWindow.setDrawCalls(drawCalls);
        debugWindow.setPolyCount(renderSystem->getPolyCount());
        debugWindow.setFrameBufferSize(wnd.getFramebufferSize());
        debugWindow.setVideoModes(display.getPrimaryMonitor()->getSupportedVideoModes());

        pipeline->getGeometryBuffer().setSamples(debugWindow.getSamples());
        pipeline->getGeometryBuffer().setSize(debugWindow.getRenderResolution());

        wnd.setSwapInterval(debugWindow.getSwapInterval());

        renderDevice->getRenderer().debugDrawCallRecordStart();

        ecs.update(deltaTime);

        if (showDebugWindow)
            drawDebugWindow();

        dynamic_cast<DebugPass &>(*(pipeline->getPasses().end() - 1)->get()).setEnabled(debugWindow.getDrawDebug());

        drawCalls = renderDevice->getRenderer().debugDrawCallRecordStop();

        fpsLimit = debugWindow.getFpsLimit();

        if (debugWindow.getFullScreen()) {
            auto mon = display.getPrimaryMonitor();
            auto mode = mon->getSupportedVideoModes().at(debugWindow.getSelectedVideoMode());
            wnd.setMonitor(*mon, Recti({}, {mode.width, mode.height}), mode.refreshRate);
        } else {
            wnd.setWindowed(); // Does not unset video-mode on i3wm, but destructing the window/display-manager will unset it.
        }

        wnd.swapBuffers();

        if (fpsLimit != 0) {
            auto delta = std::chrono::high_resolution_clock::now() - frameStart;
            auto time = std::chrono::nanoseconds(static_cast<long>(1000000000.0f / fpsLimit));
            std::this_thread::sleep_for(time - delta);
        }
    }

private:
    void onKeyDown(KeyboardKey key) override {
        if (key == KEY_F1) {
            showDebugWindow = !showDebugWindow;
        } else if (key == KEY_F2) {
            auto &cmgr = ecs.getEntityManager().getComponentManager();
            auto comp = cmgr.lookup<AudioSourceComponent>(cameraEntity);
            comp.play = !comp.play;
            cmgr.update(cameraEntity, comp);
        }
    }

    void onKeyUp(KeyboardKey key) override {}

    void drawLoadingScreen(float progress, const std::string &loadingText = "Loading...") {
        if (progress > 1)
            progress = 1;
        else if (progress < 0)
            progress = 0;

        window->update();

        auto &target = window->getRenderTarget(graphicsBackend);

        ren2d->renderBegin(target, true, {}, target.getSize(), bgColor);

        auto targetSize = target.getSize();
        auto targetHalfSize = targetSize / 2;

        auto barBgColor = ColorRGBA(71, 71, 71, 255);
        auto barFgColor = ColorRGBA(119, 255, 74, 255);
        auto textColor = ColorRGBA(255, 255, 255, 255);

        // Draw background bar
        Vec2f barSize(400, 50);
        Vec2f halfBarSize = barSize / 2;
        Vec2f pos = targetHalfSize.convert<float>() - halfBarSize + Vec2f(0, 100);
        ren2d->draw(Rectf(pos, barSize),
                    barBgColor,
                    true);

        // Draw foreground bar
        Vec2f fgBarSize(barSize.x * progress, barSize.y);
        ren2d->draw(Rectf(pos, fgBarSize),
                    barFgColor,
                    true);

        ren2d->renderPresent();

        //Draw text
        textRenderer->setFontSize(Vec2i(40, 0));
        auto text = textRenderer->render(loadingText, 50);
        auto textSize = text.getTexture().getAttributes().size;
        auto textHalfSize = textSize / 2;
        auto textPos = targetHalfSize - textHalfSize - Vec2i(0, 0);

        auto dstRect = Rectf(textPos.convert<float>(), textSize.convert<float>());

        ren2d->renderBegin(target, false);
        ren2d->draw(text, dstRect, textColor);
        ren2d->renderPresent();

        window->swapBuffers();
    }

    void drawDebugWindow() {
        auto &wnd = *window;
        auto &target = window->getRenderTarget(graphicsBackend);
        ImGuiCompat::NewFrame(wnd, graphicsBackend);
        ImGui::NewFrame();
        debugWindow.draw();
        ImGui::Render();
        ImGuiCompat::DrawData(wnd,
                              target,
                              RenderOptions({}, target.getSize(), false, false, 1, {}, 1, false, false, false),
                              graphicsBackend);
    }

private:
    ECS ecs;

    Entity cameraEntity;
    double fpsAverage = 1;
    unsigned long drawCalls = 0;// The number of draw calls in the last update

    RenderSystem *renderSystem{};
    std::unique_ptr<AudioDevice> audioDevice;

    int fullscreeenIndex = 0;

    bool showDebugWindow = false;
    DebugWindow debugWindow;

    std::unique_ptr<AssetManager> assetManager;
    std::unique_ptr<AssetRenderManager> assetRenderManager;

    std::unique_ptr<DeferredPipeline> pipeline;
    std::unique_ptr<Renderer2D> ren2d;

    ColorRGBA bgColor = {38, 38, 38, 255};

    ImPlotContext *imPlotContext = nullptr;

    std::unique_ptr<Font> font;
    std::unique_ptr<TextRenderer> textRenderer;

    std::unique_ptr<Archive> archive;
};

#endif //MANA_SAMPLEAPPLICATION_HPP
