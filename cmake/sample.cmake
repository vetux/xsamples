file(GLOB_RECURSE Sample.File.SRC source/sample/src/*.cpp source/sample/src/*.c)

add_executable(mana-sample ${Sample.File.SRC})

target_include_directories(mana-sample PRIVATE source/sample/src/)
target_link_libraries(mana-sample mana-engine)

# Copy mana dll and assets folder to binary dir
file(COPY assets/engine/csharp/mana.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY assets/engine/csharp/lib/Newtonsoft.Json/Newtonsoft.Json.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE Sample.AssetPack assets/sample/assets*.pak)
file(COPY ${Sample.AssetPack} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})