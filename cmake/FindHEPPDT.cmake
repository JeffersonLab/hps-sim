find_path( HEPPDT_INCLUDE_DIR HepPDT/Version.hh ${HEPPDT_DIR}/include )

find_library( HEPPDT_LIBRARY HepPDT ${HEPPDT_DIR}/lib/ )
find_library( HEPPID_LIBRARY HepPID ${HEPPDT_DIR}/lib/ )

set( HEPPDT_LIBRARIES ${HEPPDT_LIBRARY} ${HEPPID_LIBRARY} )

# Report the found libraries, quit with fatal error if any required library has not been found.
include( FindPackageHandleStandardArgs )

find_package_handle_standard_args( HEPPDT DEFAULT_MSG HEPPDT_LIBRARIES HEPPDT_INCLUDE_DIR )
