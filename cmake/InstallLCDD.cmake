IF( NOT LCDD_TAG )
    SET( LCDD_TAG "master" )
ENDIF()

externalproject_add(
    LCDD

    DEPENDS XercesC Geant4 GDML

    GIT_REPOSITORY "https://github.com/slaclab/lcdd"
    GIT_TAG ${LCDD_TAG}
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    SOURCE_DIR "${CMAKE_BINARY_DIR}/lcdd"

    CMAKE_ARGS -DINSTALL_DOC=OFF -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}/lcdd -DGDML_DIR=${GDML_DIR} -DXercesC_INCLUDE_DIR=${XercesC_INCLUDE_DIR} -DXercesC_LIBRARY=${XercesC_LIBRARY} -DGeant4_DIR=${Geant4_DIR}
    
    BUILD_COMMAND make -j4
)

set( LCDD_DIR "${DEPENDENCY_INSTALL_DIR}/lcdd" CACHE PATH "LCDD install dir" FORCE )
