# Sample 0
include(cmake/sample0.cmake)

# Sample...

# Copy Assets dir to binary dir
set(Assets submodules/assets)
file(COPY ${Assets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})