add_library(hcs301_protocol
    hcs301_protocol.c
)

target_include_directories(hcs301_protocol
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hcs301_protocol
    PUBLIC
        err_core
        keeloq_sw
        remote_storage
)