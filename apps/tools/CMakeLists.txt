add_subdirectory(ivAddVP)
add_subdirectory(ivcat)
add_subdirectory(ivfix)
add_subdirectory(ivinfo)
add_subdirectory(ivnorm)
if (X11_FOUND)
    add_subdirectory(ivperf)
    add_subdirectory(ivview)
endif()

