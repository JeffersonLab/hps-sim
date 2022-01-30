##########################################################
# CMake configuration for HPS sim application.           #
#                                                        #
# @author Jeremy McCormick <jeremym@slac.stanford.edu>   #
#                                                        #
##########################################################

project(HPSSIM)
set(HPSSIM_VERSION 0.0.1)
cmake_minimum_required(VERSION 3.0)

# Check compiler version
if(CMAKE_CXX_COMPILER_VERSION LESS 4.8)
    message(FATAL_ERROR "The gcc compiler version '${CMAKE_CXX_COMPILER_VERSION}' is too old (at least 4.8 required).")
endif()

# Option to enable installation of external dependencies
OPTION( INSTALL_DEPENDENCIES "Install external dependencies automatically" ON )

# Option to enable Geant4 visualization and UI components
option( WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON )

# Add project cmake scripts
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Dependency list
set(DEPENDENCIES XercesC Geant4 GDML LCDD LCIO HEPPDT)

# Install dependencies script
include(InstallDeps)

if( DEPENDENCIES_NOTFOUND )
    # In this case, the user needs to execute 'make' and then 'cmake' again for all the installed dependencies to be found.
    message("")
    message("Some dependencies were not found.  Run 'make' to build them, or rerun CMake and provide their location (e.g. Geant4_DIR etc.).")
    message("Set '-DDEPENDENCY_INSTALL_DIR=/my/install/dir' to set the directory where external projects will be installed.")
    message("After building any missing dependencies you will need to rerun CMake to resolve them.")
    message("")
else()
 
    # All dependencies were found or installed. The build can proceed.

    message("All dependencies were found!")
  
    # Define main binary executable  
    file(GLOB_RECURSE library_sources ${PROJECT_SOURCE_DIR}/src/*.cxx)
    add_executable(hps-sim ${library_sources} src/hps-sim.cxx)
    
    # Use Geant4 build flags
    include(${Geant4_USE_FILE})

    # Configure include directories
    include_directories(include/)
    include_directories(${XercesC_INCLUDE_DIR} ${LCIO_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS} ${GDML_INCLUDE_DIR} ${LCDD_INCLUDE_DIR}) 

    # Build the user plugin library
    file(GLOB_RECURSE plugin_sources plugins/*.cxx)
    add_library(SimPlugins SHARED ${plugin_sources})
    
    # This is needed for linking on Mac to work.
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        target_link_libraries(SimPlugins "-undefined dynamic_lookup" ${Geant4_LIBRARIES})
    endif()

    # Configure SimPlugins module    
    install(TARGETS SimPlugins DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    add_dependencies(hps-sim SimPlugins)
    
    # Add link libraries and directories
    target_link_libraries(hps-sim ${XercesC_LIBRARY} ${Geant4_LIBRARIES} ${GDML_LIBRARY} ${LCDD_LIBRARY} ${LCIO_LIBRARIES} dl)
    link_directories(${GDML_LIBRARY_DIR} ${LCDD_LIBRARY_DIR} ${LCIO_LIBRARY_DIRS})

    # Install the binary
    install(TARGETS hps-sim hps-sim DESTINATION bin)

    # Configure and install the env script
    configure_file(scripts/hps-sim-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/hps-sim-env.sh)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hps-sim-env.sh DESTINATION bin
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    
endif()
