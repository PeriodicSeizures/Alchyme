#AlchymeClient

file(GLOB src "./src/*.cpp")
file(GLOB mf "./src/*.manifest")

#Make an executable
add_executable(AlchymeClient ${src} ${mf})

#AlchymeCore includes
include_directories(${CMAKE_SOURCE_DIR}/AlchymeCore/src)

#Static link
target_link_libraries(AlchymeClient 
	AlchymeCore
    Threads::Threads
    asio
    OpenGL::GL
    glm
    stb
    easyloggingpp
    libglew_static
    #nlohmann_json::nlohmann_json
    SDL2-static
    SDL2main
    OptickCore
    zlibstatic
    sol2
    RmlLua
    RmlDebugger
)

#Use C++20 features
target_compile_features(AlchymeClient PRIVATE cxx_std_20)

set_target_properties(AlchymeClient OptickCore PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(AlchymeClient PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/AlchymeClient/
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AlchymeClient)

if (MSVC)
    target_compile_options(AlchymeClient PRIVATE /bigobj)
endif()
if(MSVC AND CMAKE_BUILD_TYPE MATCHES Release)
    set_target_properties(AlchymeClient PROPERTIES WIN32_EXECUTABLE ON)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#add_custom_command(
#        TARGET AlchymeClient POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#                ${CMAKE_SOURCE_DIR}/AlchymeClient/cwd
#                ${CMAKE_CURRENT_BINARY_DIR}/cwd)
                
#file(DOWNLOAD https://github.com/google/fonts/raw/main/apache/opensans/OpenSans%5Bwdth%2Cwght%5D.ttf ${CMAKE_SOURCE_DIR}/AlchymeClient/cwd/assets/fonts/OpenSans-Regular.ttf)
file(DOWNLOAD https://github.com/google/fonts/raw/main/apache/opensans/OpenSans%5Bwdth%2Cwght%5D.ttf ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts/OpenSans-Regular.ttf)