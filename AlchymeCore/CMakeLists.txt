# AlchymeCore

#set_target_properties(AlchymeCore PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB src "./src/*.manifest")
file(GLOB mf "./src/*.cpp")

add_library(AlchymeCore ${src} ${mf})

target_link_libraries(AlchymeCore
    Threads::Threads
    asio
)

target_compile_features(AlchymeCore PRIVATE cxx_std_20)

install(TARGETS AlchymeCore
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)