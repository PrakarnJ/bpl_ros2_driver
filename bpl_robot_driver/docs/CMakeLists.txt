if (BUILD_DOCS MATCHES 1)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		set(DOXYGEN_PROJECT_NAME ${CMAKE_PROJECT_NAME})
		set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cfg)
		set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
		set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
        
        message("░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░") 
        message("░█▀▀█ ░█▀▀▀█ ░█▀▀▀█ █▀█  ░█▀▀█ ░█─▄▀ ░█▀▀█  ▀▀█▀▀ ░█▀▀▀ ░█▀▄▀█ ░█▀▀█ ░█─── ─█▀▀█ ▀▀█▀▀ ░█▀▀▀") 
        message("░█▄▄▀ ░█──░█ ─▀▀▀▄▄ ─▄▀  ░█▄▄█ ░█▀▄─ ░█─▄▄  ─░█── ░█▀▀▀ ░█░█░█ ░█▄▄█ ░█─── ░█▄▄█ ─░█── ░█▀▀▀") 
        message("░█─░█ ░█▄▄▄█ ░█▄▄▄█ █▄▄  ░█─── ░█─░█ ░█▄▄█  ─░█── ░█▄▄▄ ░█──░█ ░█─── ░█▄▄█ ░█─░█ ─░█── ░█▄▄▄")
        message("░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░") 
        message("     | ")
        message("     --- Doxygen build started")
		file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
		add_custom_target(doxygen ALL
					COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
					MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
					COMMENT "Generating API documentation with Doxygen"
					VERBATIM)
	else (DOXYGEN_FOUND)
	message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)

	find_package(Sphinx)
	if(SPHINX_FOUND)
		set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
		set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/html)
		add_custom_target(Sphinx ALL
						COMMAND ${SPHINX_EXECUTABLE} -b html
						-Dbreathe_projects.${CMAKE_PROJECT_NAME}=${CMAKE_CURRENT_BINARY_DIR}/doxygen/xml
						${SPHINX_SOURCE} ${SPHINX_BUILD}
						WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
						COMMENT "Generating documentation with Sphinx")
		install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
		DESTINATION .)
	endif()
else()
    message("====== ROS2_PKG_TEMPLATE ======")
    message("     |--- Skip building docs...")
endif()