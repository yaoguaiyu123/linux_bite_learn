cmake_minimum_required(VERSION 3.27)

project(linux02 LANGUAGES C)

add_executable(linux02
    main.c
)

target_compile_features(linux02 PRIVATE c_std_17)




































































# include(GNUInstallDirs)
# install(TARGETS linux02
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# target_sources(linux02 
#     PRIVATE
#         %{CppFileName}    
#         
#     PUBLIC
#         
# )
