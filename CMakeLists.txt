CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 ) 

SET ( CMAKE_CXX_FLAGS "-std=c++14 -w")

INCLUDE_DIRECTORIES( 
    ${CMAKE_SOURCE_DIR}
    )
SET ( SRC_FILES
    main.cpp
    )

ADD_EXECUTABLE( main.exe ${SRC_FILES} )


target_link_libraries( main.exe SDL2 SDL2_image pthread glfw)

