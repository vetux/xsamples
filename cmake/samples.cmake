# Sample 0
file(GLOB_RECURSE XSample0.SRC source/sample/src/*.cpp source/sample/src/*.c)
add_executable(xsample0 ${XSample0.SRC})
target_include_directories(xsample0 PRIVATE sample0/src/)
target_link_libraries(xsample0 xengine)

file(GLOB_RECURSE Assets submodules/assets)
file(COPY ${Assets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})