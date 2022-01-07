# Sample 0
file(GLOB_RECURSE XSample0.SRC sample0/src/*.cpp sample0/src/*.c)
add_executable(xsample0 ${XSample0.SRC})
target_include_directories(xsample0 PRIVATE sample0/src/)
target_link_libraries(xsample0 xengine implot)

# Sample...

# Copy Assets to binary dir
file(GLOB_RECURSE Assets submodules/assets)
file(COPY ${Assets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})