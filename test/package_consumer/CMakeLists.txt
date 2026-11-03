cmake_minimum_required(VERSION 3.28)
project(StormByteConfigPackageConsumer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(PREPEND CMAKE_MODULE_PATH "${BASE_PREFIX}/share/cmake/Modules")
list(PREPEND CMAKE_PREFIX_PATH "${CONFIG_PREFIX}" "${BASE_PREFIX}")
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)
set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)

find_package(StormByte REQUIRED COMPONENTS Base Config)

foreach(path IN ITEMS "${STORMBYTE_Config_INCLUDE_DIR}" "${STORMBYTE_Config_LIBRARY}")
	cmake_path(IS_PREFIX CONFIG_PREFIX "${path}" NORMALIZE _in_config_prefix)
	if(NOT _in_config_prefix)
		message(FATAL_ERROR "Config was not found in ${CONFIG_PREFIX}: ${path}")
	endif()
endforeach()
cmake_path(IS_PREFIX BASE_PREFIX "${STORMBYTE_LIBRARY}" NORMALIZE _in_base_prefix)
if(NOT _in_base_prefix)
	message(FATAL_ERROR "Base was not found in ${BASE_PREFIX}: ${STORMBYTE_LIBRARY}")
endif()

add_executable(config_package_consumer consumer.cxx)
target_link_libraries(config_package_consumer PRIVATE StormByte::Config)
set_target_properties(config_package_consumer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/bin>"
)
