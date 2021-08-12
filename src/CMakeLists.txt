include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}.out
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/data_crunch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sort.cpp
)

target_link_libraries(${PROJECT_NAME}.out)