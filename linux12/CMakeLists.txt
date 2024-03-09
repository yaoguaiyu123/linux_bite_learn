cmake_minimum_required(VERSION 3.27)

project(linux12 LANGUAGES C)

add_executable(linux12
    main.c
)

target_compile_features(linux12 PRIVATE c_std_17)




































































# include(GNUInstallDirs)
# install(TARGETS linux12
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# target_sources(linux12 
#     PRIVATE
#         %{CppFileName}    
#         
#     PUBLIC
#         
# )
