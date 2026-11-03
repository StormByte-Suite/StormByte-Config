option(ENABLE_TEST "Enable Unit Tests" OFF)
if(ENABLE_TEST)
	enable_testing()

	add_test(NAME OptimizationTests
		COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/optimizations_test.cmake")
	set_tests_properties(OptimizationTests PROPERTIES TIMEOUT 15)

	add_executable(BinaryTests binary_test.cxx)
	target_link_libraries(BinaryTests StormByte::Config)
	add_test(NAME BinaryTests COMMAND BinaryTests)

	add_executable(ConfigTests config_test.cxx)
	target_link_libraries(ConfigTests StormByte::Config)
	add_test(NAME ConfigTests COMMAND ConfigTests)

	add_executable(HookTests hooks_test.cxx)
	target_link_libraries(HookTests StormByte::Config)
	add_test(NAME HookTests COMMAND HookTests)

	add_executable(SerializationTests serialization_test.cxx)
	target_link_libraries(SerializationTests StormByte::Config)
	add_test(NAME SerializationTests COMMAND SerializationTests)

	add_executable(VersionBinReadTests version_bin_read.cxx)
	target_link_libraries(VersionBinReadTests StormByte::Config)
	add_test(NAME VersionBinReadTests COMMAND VersionBinReadTests)

	if(WITH_STORMBYTE STREQUAL "BUNDLED" AND NOT CMAKE_CROSSCOMPILING)
		set(_package_cxx_flags "${CMAKE_CXX_FLAGS}")
		set(_package_link_flags "${CMAKE_EXE_LINKER_FLAGS}")
		if(COMMAND _pgo_strip_profile_flags)
			_pgo_strip_profile_flags("${_package_cxx_flags}" _package_cxx_flags)
			_pgo_strip_profile_flags("${_package_link_flags}" _package_link_flags)
		endif()

		add_test(NAME PackageDiscoveryTests
			COMMAND "${CMAKE_COMMAND}"
				"-DCONFIG_BUILD_DIR=${PROJECT_BINARY_DIR}"
				"-DCONSUMER_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/package_consumer"
				"-DBASE_PREFIX=${BUILDMASTER_INSTALL_DIR}"
				"-DBUILD_CONFIG=$<CONFIG>"
				"-DGENERATOR=${CMAKE_GENERATOR}"
				"-DGENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
				"-DGENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
				"-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
				"-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
				"-DCXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}"
				"-DCXX_FLAGS=${_package_cxx_flags}"
				"-DEXE_LINKER_FLAGS=${_package_link_flags}"
				"-DOSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}"
				"-DOSX_SYSROOT=${CMAKE_OSX_SYSROOT}"
				"-DEXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}"
				-P "${CMAKE_CURRENT_SOURCE_DIR}/package_discovery_test.cmake")
		set_tests_properties(PackageDiscoveryTests PROPERTIES TIMEOUT 120)
	endif()
endif()
