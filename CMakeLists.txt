cmake_minimum_required(VERSION 3.19)
project(Fifa26_Simulator LANGUAGES CXX)

# Fallbacki dla CLion/Windows, gdy profil nie przekazuje CMAKE_PREFIX_PATH.
if(NOT Qt6_DIR AND DEFINED ENV{Qt6_DIR})
    set(Qt6_DIR "$ENV{Qt6_DIR}" CACHE PATH "Path to Qt6 CMake config directory")
endif()

if(DEFINED ENV{QTDIR})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

if(WIN32)
    set(_qt_windows_roots
            "C:/Qt/6.10.2/mingw_64"
            "C:/Qt/6.10.2/msvc2022_64"
            "C:/Qt/6.9.0/mingw_64"
            "C:/Qt/6.8.0/mingw_64"
            "C:/Qt/6.5.0/mingw_64"
    )

    foreach(_qt_root IN LISTS _qt_windows_roots)
        if(EXISTS "${_qt_root}/lib/cmake/Qt6/Qt6Config.cmake")
            list(APPEND CMAKE_PREFIX_PATH "${_qt_root}")
            break()
        endif()
    endforeach()
endif()

find_package(Qt6 6.5 QUIET COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    message(FATAL_ERROR
            "Nie znaleziono Qt6. Ustaw CMAKE_PREFIX_PATH na katalog Qt (np. C:/Qt/6.10.2/mingw_64)\n"
            "albo ustaw Qt6_DIR na katalog z Qt6Config.cmake (np. C:/Qt/6.10.2/mingw_64/lib/cmake/Qt6)."
    )
endif()

qt_standard_project_setup()

set(CORE_HEADERS
        core/headers/Player.h
        core/headers/Striker.h
        core/headers/Defender.h
        core/headers/Goalkeeper.h
        core/headers/Midfielder.h
        core/headers/Statistic.h
        core/headers/Team.h
        core/headers/Match.h
        core/headers/LeagueEngine.h
)

set(CORE_SOURCES
        core/src/Player.cpp
        core/src/Striker.cpp
        core/src/Defender.cpp
        core/src/Goalkeeper.cpp
        core/src/Midfielder.cpp
        core/src/Team.cpp
        core/src/Match.cpp
        core/src/LeagueEngine.cpp
)

set(GUI_SOURCES
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        gui/LeagueTableWidget.h
        gui/LeagueTableWidget.cpp
        gui/MatchScheduleWidget.h
        gui/MatchScheduleWidget.cpp
        gui/TeamDetailsWidget.h
        gui/TeamDetailsWidget.cpp
)

qt_add_executable(Fifa26_Simulator
    WIN32 MACOSX_BUNDLE
    main.cpp
    ${CORE_HEADERS}
    ${CORE_SOURCES}
    ${GUI_SOURCES}
)

target_include_directories(Fifa26_Simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core/headers
)

target_link_libraries(Fifa26_Simulator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

if(WIN32)
    get_filename_component(_qt_bin_hint "${Qt6_DIR}/../../../bin" ABSOLUTE)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_hint}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET Fifa26_Simulator POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --no-opengl-sw --no-compiler-runtime "$<TARGET_FILE:Fifa26_Simulator>"
            COMMENT "Deploying Qt runtime with windeployqt"
            VERBATIM
        )
    else()
        message(WARNING "Nie znaleziono windeployqt - uruchom EXE z PATH zawierajacym katalog Qt/bin.")
    endif()
endif()

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