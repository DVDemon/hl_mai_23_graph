cmake_minimum_required(VERSION 3.2)

project(hl_mal_graphdb)
set(BINARY "graphdb")
SET (NEO4J_LIBS "neo4j-client")
LIST(APPEND NEO4J_LIBS "crypto")
LIST(APPEND NEO4J_LIBS "ssl")

find_package(Threads)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(${BINARY} main.cpp)

set_target_properties(${BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

link_libraries(${BINARY} PRIVATE 
                        ${NEO4J_LIBS}
                        ${CMAKE_THREAD_LIBS_INIT})