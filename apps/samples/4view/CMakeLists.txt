add_executable(4view 4view.c++)
target_link_libraries(4view Inventor InventorXt)
install(TARGETS 4view DESTINATION bin/samples)
