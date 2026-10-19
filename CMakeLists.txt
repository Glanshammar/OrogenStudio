cmake_minimum_required(VERSION 3.19)
project(OrogenStudio LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(OrogenStudio
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    startupdialog.h startupdialog.cpp startupdialog.ui
    highlighter.h highlighter.cpp
    cpphighlighter.h
    project.h
)

qt_add_resources(OrogenStudio "resources"
    PREFIX ""
    FILES "mountain.ico"
)

target_link_libraries(OrogenStudio
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS OrogenStudio
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET OrogenStudio
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
