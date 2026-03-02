cmake_minimum_required(VERSION 3.19)
project(Fifa26_Simulator LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CORE_SOURCES
)

set(GUI_SOURCES
)

qt_add_executable(Fifa26_Simulator
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${CORE_SOURCES}
    ${GUI_SOURCES}
)

target_include_directories(Fifa26_Simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

target_link_libraries(Fifa26_Simulator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS Fifa26_Simulator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Fifa26_Simulator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})