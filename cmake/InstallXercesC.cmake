externalproject_add(
    XercesC 
    URL "https://downloads.apache.org/xerces/c/3/sources/xerces-c-3.2.3.tar.gz"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/xerces"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/xerces/configure" --enable-shared --disable-static --prefix=${DEPENDENCY_INSTALL_DIR}/xerces
)

set(XercesC_INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}/xerces)

set(XercesC_DIR ${XercesC_INSTALL_DIR} CACHE PATH "XercesC dir" FORCE )
set(XercesC_LIBRARY ${XercesC_INSTALL_DIR}/lib/libxerces-c${CMAKE_SHARED_LIBRARY_SUFFIX} CACHE FILEPATH "XercesC library" FORCE)
set(XercesC_INCLUDE_DIR ${XercesC_INSTALL_DIR}/include CACHE PATH "XercesC include dir" FORCE)
