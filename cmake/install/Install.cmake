include(InstallRequiredSystemLibraries)

install(
    TARGETS ${CMAKE_PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    BUNDLE  DESTINATION .
)

set(deploy_tool_options_arg "")
if(MACOS OR WIN32)
    set(deploy_tool_options_arg "-qmldir=${CMAKE_SOURCE_DIR}")
    # if(MACOS_SIGNING_IDENTITY)
    #     message(STATUS "Signing MacOS Bundle")
    #     set(deploy_tool_options_arg "${deploy_tool_options_arg} -sign-for-notarization=${MACOS_SIGNING_IDENTITY}")
    # endif()
endif()

# Set extra deploy QML app script options for Qt 6.7.0 and above
set(EXTRA_DEPLOY_QML_APP_SCRIPT_OPTIONS)
if(Qt6_VERSION VERSION_GREATER_EQUAL 6.7.0)
    list(APPEND EXTRA_DEPLOY_QML_APP_SCRIPT_OPTIONS DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg})
endif()

qt_generate_deploy_qml_app_script(
    TARGET ${CMAKE_PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    ${EXTRA_DEPLOY_QML_APP_SCRIPT_OPTIONS}
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})

if(ANDROID)
    # get_target_property(QGC_ANDROID_DEPLOY_FILE ${CMAKE_PROJECT_NAME} QT_ANDROID_DEPLOYMENT_SETTINGS_FILE)
    # cmake_print_variables(QGC_ANDROID_DEPLOY_FILE)
elseif(LINUX)
    configure_file(
        ${CMAKE_SOURCE_DIR}/deploy/linux/org.mavlink.qgroundcontrol.desktop.in
        ${CMAKE_BINARY_DIR}/org.mavlink.qgroundcontrol.desktop
        @ONLY
    )
    install(
        FILES ${CMAKE_BINARY_DIR}/org.mavlink.qgroundcontrol.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
    install(
        FILES ${QGC_APPIMAGE_ICON_PATH}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps/
        RENAME ${CMAKE_PROJECT_NAME}.png
    )
    configure_file(
        ${CMAKE_SOURCE_DIR}/deploy/linux/org.mavlink.qgroundcontrol.metainfo.xml.in
        ${CMAKE_BINARY_DIR}/metainfo/org.mavlink.qgroundcontrol.metainfo.xml
        @ONLY
    )
    install(
        FILES ${CMAKE_BINARY_DIR}/metainfo/org.mavlink.qgroundcontrol.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo/
    )
    install(
        FILES ${CMAKE_SOURCE_DIR}/deploy/linux/AppRun
        DESTINATION ${CMAKE_BINARY_DIR}
    )
    install(CODE "set(CMAKE_SYSTEM_PROCESSOR \"${CMAKE_SYSTEM_PROCESSOR}\")")
    install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/install/CreateAppImage.cmake")
elseif(WIN32)
    install(CODE "set(CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME})")
    install(CODE "set(QGC_ORG_NAME ${QGC_ORG_NAME})")
    install(CODE "set(QGC_WINDOWS_ICON_PATH ${QGC_WINDOWS_ICON_PATH})")
    install(CODE "set(QGC_WINDOWS_INSTALL_HEADER_PATH ${QGC_WINDOWS_INSTALL_HEADER_PATH})")
    install(CODE "set(QGC_WINDOWS_DRIVER_MSI ${CMAKE_SOURCE_DIR}/deploy/windows/driver.msi)")
    install(CODE "set(QGC_WINDOWS_OUT ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-installer.exe)")
    install(CODE "set(QGC_WINDOWS_INSTALLER_SCRIPT ${CMAKE_SOURCE_DIR}/deploy/windows/nullsoft_installer.nsi)")
    install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/install/CreateWinInstaller.cmake")
elseif(MACOS)
    install(CODE "set(TARGET_APP_NAME ${QGC_APP_NAME})")
    install(CODE "set(MACDEPLOYQT ${Qt6_DIR}/../../../bin/macdeployqt)")
    install(SCRIPT "${CMAKE_SOURCE_DIR}/cmake/install/CreateMacDMG.cmake")
endif()
