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
endif()
