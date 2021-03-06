file(GLOB_RECURSE XSample0.SRC apps/sample0/src/*.cpp apps/sample0/src/*.c)
add_executable(xsample0 ${XSample0.SRC})
target_include_directories(xsample0 PRIVATE apps/sample0/src/)
target_link_libraries(xsample0 xengine implot)
set(SceneFile apps/sample0/scene.json)
file(COPY ${SceneFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)