if( NOT GEANT4_INSTALL_DATA )
    set( GEANT4_INSTALL_DATA "ON" )
endif()

if( NOT Geant4_TAG )
    set( Geant4_TAG "v10.3.1" )
endif()
string( REPLACE "v" "" Geant4_VERSION_NUMBER ${Geant4_TAG} )

externalproject_add(
    Geant4

    GIT_REPOSITORY "https://github.com/Geant4/geant4"
    GIT_TAG ${Geant4_TAG}
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    SOURCE_DIR "${CMAKE_BINARY_DIR}/geant4"
    
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}/geant4 -DGEANT4_INSTALL_DATA=${GEANT4_INSTALL_DATA} -DGEANT4_USE_OPENGL_X11=${WITH_GEANT4_UIVIS} -DGEANT4_USE_QT=${WITH_GEANT4_UIVIS}
    
    BUILD_COMMAND make -j4
    BUILD_ALWAYS ON
)

if( NOT "${Geant4_DIR}" )
    set( Geant4_DIR ${DEPENDENCY_INSTALL_DIR}/geant4/lib/Geant4-${Geant4_VERSION_NUMBER} CACHE PATH "Geant4 install dir" FORCE )
    message( "Geant4_DIR=${Geant4_DIR}" )
    list(APPEND CMAKE_MODULE_PATH "${Geant4_DIR}") 
endif()
