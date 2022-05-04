file(GLOB_RECURSE AssetExplorer.SRC apps/assetexplorer/src/*.cpp apps/assetexplorer/src/*.c)
add_executable(assetexplorer ${AssetExplorer.SRC})
target_include_directories(assetexplorer PRIVATE apps/assetexplorer/src/)
target_link_libraries(assetexplorer xengine implot)
