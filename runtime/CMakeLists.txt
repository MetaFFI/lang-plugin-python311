find_package(Boost REQUIRED COMPONENTS filesystem) # Boost library
find_package(Python3 REQUIRED COMPONENTS Development) # Python library

set(target_name "xllr.python3")

# build xllr.python3
file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.h ${METAFFI_SDK}/utils/*.cpp ${METAFFI_SDK}/runtime/*.cpp ${METAFFI_SDK}/runtime/*.c ${METAFFI_SDK}/utils/*.hpp)
add_library(${target_name}
			SHARED
			${SRC_FILES}
)
target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${Boost_INCLUDE_DIRS} ${Python3_INCLUDE_DIRS} ${METAFFI_SDK}/runtime/*.h ${METAFFI_SDK}/utils/*.h)

set_property(TARGET ${target_name} PROPERTY POSITION_INDEPENDENT_CODE ON)

# remove the "lib" prefix in linux to get the expected filename "xllr.python3.[ext]"
SET_TARGET_PROPERTIES(${target_name} PROPERTIES PREFIX "")

target_link_libraries(${target_name} Boost::filesystem Python3::Python)
target_link_directories(${target_name} PUBLIC ${Python3_LIBRARY_DIRS})

if(WIN32)
	target_link_directories(${target_name} PUBLIC ${BOOST_LIBRARYDIR} $ENV{METAFFI_HOME})
else()
	target_link_directories(${target_name} PUBLIC ${Boost_LIBRARIES} $ENV{METAFFI_HOME})
endif()

# Copy to metaffi directory
add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy
		"${CMAKE_CURRENT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		"$ENV{METAFFI_HOME}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}"
		COMMENT "Copy ${CMAKE_CURRENT_BINARY_DIR}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX} -> $ENV{METAFFI_HOME}/${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")

# install
install(TARGETS ${target_name} DESTINATION .)

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
	ADD_DEB_DEPENDS(${CMAKE_BINARY_DIR} ${target_name} "python|boost|expat|jvm" INSTALL_FILES INSTALL_RESOLVED_FILES INSTALL_FILES_ERROR)

	if (${INSTALL_FILES_ERROR})
		message( FATAL_ERROR "Dependencies files resolution failed: " ${INSTALL_FILES_ERROR} )
	endif (${INSTALL_FILES_ERROR})

	if(INSTALL_FILES)
		install(FILES ${INSTALL_FILES} # Copy all libraries and symlinks pointing to real paths
				DESTINATION .)
	endif()

	if(INSTALL_RESOLVED_FILES)
		install(FILES ${INSTALL_RESOLVED_FILES} # Copy all libraries real paths
				DESTINATION .)
	endif()
endif()