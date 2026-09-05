add_library(keeloq_sw
    keeloq_sw.c
)

target_include_directories(keeloq_sw
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(keeloq_sw
    PUBLIC
        err_core
)