externalproject_add(    
    HEPPDT
    GIT_REPOSITORY    "https://github.com/slaclab/heppdt"
    GIT_TAG           "master"
    GIT_SHALLOW       ON
    SOURCE_DIR        "${CMAKE_BINARY_DIR}/HepPDT"
    UPDATE_COMMAND    ""
    PATCH_COMMAND     ""
    CONFIGURE_COMMAND CC=${CMAKE_CC_COMPILER} && CXX=${CMAKE_CXX_COMPILER} && ./configure --prefix=${DEPENDENCY_INSTALL_DIR}/heppdt --disable-shared
    BUILD_IN_SOURCE   ON
    BUILD_COMMAND     ${CMAKE_MAKE_PROGRAM} VERBOSE=1
)

if( NOT "${HEPPDT_DIR}" )
    set( HEPPDT_DIR ${DEPENDENCY_INSTALL_DIR}/heppdt CACHE PATH "HEPPDT install dir" FORCE )
    #message( "HEPPDT_DIR=${HEPPDT_DIR}" )
endif()
