cmake_minimum_required(VERSION 3.14)
project(PICoBoot_Utility)

set(CMAKE_CXX_STANDARD 17)

set(CPM_DOWNLOAD_VERSION 0.27.5)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
        NAME cxxopts
        GITHUB_REPOSITORY SudoMaker/cxxopts
        VERSION 3.0.0
        GIT_TAG 5eca8a30012b69b76316b71fa391a89fe09256cb
        GIT_SHALLOW ON
)

CPMAddPackage(
        NAME IODash
        GITHUB_REPOSITORY YukiWorkshop/IODash
        VERSION 0.1.7
        GIT_SHALLOW ON
)

CPMAddPackage(
        NAME iHexPP
        GITHUB_REPOSITORY SudoMaker/iHexPP
        VERSION 0.0.2
        GIT_SHALLOW ON
)

add_executable(picoboot PICoBoot_Utility.cpp PICoBoot_Utility.hpp ScanDevice.cpp SerialOps.cpp Protocol.cpp Cmd_Flash.cpp Cmd_Info.cpp Cmd_Reboot.cpp Cmd_Devices.cpp Cmd_Env.cpp Cmd_Erase.cpp)
target_link_libraries(picoboot cxxopts IODash iHexPP)

if(UNIX AND NOT APPLE)
    target_link_libraries(picoboot stdc++fs)
endif()