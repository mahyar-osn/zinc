# OpenCMISS-Zinc Library
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

SET( CPACK_PACKAGE_NAME ${PROJECT_NAME} )
SET( ZINC_PACKAGE_VERSION "${ZINC_VERSION}" )
SET( CPACK_PACKAGE_VERSION "${ZINC_PACKAGE_VERSION}" )

SET(ZINC_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
IF(WIN32)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(ZINC_ARCHITECTURE x86)
    ELSE()
        SET(ZINC_ARCHITECTURE amd64)
    ENDIF()
ENDIF()

GET_SYSTEM_NAME( ZINC_SYSTEM )

IF( WIN32 )
    IF ( ${CMAKE_VERSION} VERSION_LESS "2.8.11" )
        MESSAGE( STATUS "The CMAKE_SYSTEM_PROCESSOR will not give the correct value for 64 bit systems." )
    ENDIF()
    SET( CPACK_GENERATOR "ZIP;NSIS" )
    SET( CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}" )
    SET( CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}" )
    IF( CMAKE_CL_64 )
        SET( CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64" )
        SET( CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} (${ZINC_ARCHITECTURE})" )
        SET( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} (${ZINC_ARCHITECTURE})" )
    ENDIF()
    SET( CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/distrib/windows\\\\nsis_banner.bmp" )
    SET( CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/distrib/icons\\\\cmiss_torso.ico" )
    SET( CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/distrib/icons\\\\cmiss_torso.ico" )
    SET( CPACK_NSIS_MODIFY_PATH "ON" )
ELSEIF( APPLE )
    # Preserve the CMAKE_INSTALL_PREFIX for the project and work with absolute install
    SET(CPACK_SET_DESTDIR ON)

    SET( CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/distrib/osx/welcome.txt" )
    LIST( LENGTH LENGTH_ARCHS CMAKE_OSX_ARCHITECTURES )

    SET( ZINC_ARCHITECTURE "universal" )
    SET( CPACK_GENERATOR  "TGZ" "PackageMaker" )
ELSEIF( UNIX )
    # Preserve the CMAKE_INSTALL_PREFIX for the project and work with absolute install
    SET(CPACK_SET_DESTDIR ON)

    SET( CPACK_GENERATOR "TGZ" )
    STRING( FIND ${ZINC_SYSTEM} "Ubuntu" INDEX )
    IF( INDEX EQUAL 0 )
        SET( CPACK_DEBIAN_PACKAGE_MAINTAINER "Hugh Sorby" ) #required
        LIST( APPEND CPACK_GENERATOR "DEB" )
    ENDIF()
ENDIF()

SET( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt" )
SET( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt" )
SET( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/distrib/description.txt" )
SET( CPACK_PACKAGE_VENDOR "physiomeproject" )

SET( CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${ZINC_PACKAGE_VERSION}-${ZINC_ARCHITECTURE}-${ZINC_SYSTEM}" )
SET( CPACK_OUTPUT_FILE_PREFIX "package" )

INCLUDE(CPack)

