include(cmake/sample0.cmake)
include(cmake/assetexplorer.cmake)

# Copy Assets dir to binary dir
set(Assets submodules/assets)
file(COPY ${Assets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})