cmake_minimum_required(VERSION 3.13)

project(SEALExamples VERSION 4.1.2 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# If not called from root CMakeLists.txt
if(NOT DEFINED FOXVIEW_SECRETKEY_VIEW)
    set(FOXVIEW_SECRETKEY_VIEW ON)

    # Import Microsoft SEAL
    find_package(SEAL 4.1.2 EXACT REQUIRED)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if(FOXVIEW_SECRETKEY_VIEW)
    add_executable(secretkey_view)

    target_sources(secretkey_view
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/secretkey_view.cpp
    )
    include_directories(${GMP_INCLUDE_DIR})
    target_link_libraries(secretkey_view PRIVATE ${GMP_LIB})

    if(TARGET SEAL::seal)
        target_link_libraries(secretkey_view PRIVATE SEAL::seal)
    elseif(TARGET SEAL::seal_shared)
        target_link_libraries(secretkey_view PRIVATE SEAL::seal_shared)
    else()
        message(FATAL_ERROR "Cannot find target SEAL::seal or SEAL::seal_shared")
    endif()
endif()
