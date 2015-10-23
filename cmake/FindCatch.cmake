include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        catch
        PREFIX ${CMAKE_BINARY_DIR}
)