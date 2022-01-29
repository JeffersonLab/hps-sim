EXTERNALPROJECT_ADD(
    XercesC 
    URL "https://downloads.apache.org/xerces/c/3/sources/xerces-c-3.2.3.tar.gz"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/xerces/configure" --enable-shared --disable-static --prefix=${DEPENDENCY_INSTALL_DIR}/xerces
)

SET( XercesC_DIR ${DEPENDENCY_INSTALL_DIR}/xerces CACHE PATH "XercesC install dir" FORCE )
