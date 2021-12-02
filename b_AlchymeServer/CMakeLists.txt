#AlchymeServer

#Include src
file(GLOB src "./src/*.cpp")
file(GLOB mf "./src/*.manifest")

#Make an executable
add_executable(AlchymeServer ${src} ${mf})

#AlchymeCore includes
include_directories(${CMAKE_SOURCE_DIR}/AlchymeCore/src)

#Static link
target_link_libraries(AlchymeServer 
	AlchymeCore
    Threads::Threads
    asio
    easyloggingpp
    #nlohmann_json::nlohmann_json
    OptickCore
    zlibstatic
    sol2
)

#Use C++20 features
target_compile_features(AlchymeServer PRIVATE cxx_std_20)

#set_target_properties(AlchymeServer OptickCore PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/AlchymeServer
#)

set_target_properties(AlchymeServer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/AlchymeServer/
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AlchymeServer)

if (MSVC)
    target_compile_options(AlchymeServer PRIVATE /bigobj)
endif()
if(MSVC AND CMAKE_BUILD_TYPE MATCHES Release)
    set_target_properties(AlchymeServer PROPERTIES WIN32_EXECUTABLE ON)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#install(TARGETS AlchymeServer RUNTIME DESTINATION bin)
