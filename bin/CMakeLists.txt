file(GLOB targets_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(filename ${targets_list})
    get_filename_component(target ${filename} NAME_WE)

    add_executable(${target} ${filename})
    target_link_libraries(${target} ${PROJ_LIBRARY})
    set_target_properties(${target} PROPERTIES
            OUTPUT_NAME "${target}"
            PROJECT_LABEL "${target}"
            RUNTIME_OUTPUT_DIRECTORY "../")
endforeach()