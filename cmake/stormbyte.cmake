# URL of a StormByte Base release archive for the system being configured.
#
# stormbyte_component_url(<outvar> <tag>)
# Writes the download URL for that release tag into <outvar>.
# The archive name matches release.yml: StormByte-<tag>-<os>-<arch>[-stdlib].zip
# Windows has no runtime suffix. Linux is libstdc++ unless -stdlib=libc++
# is set. macOS is libc++ unless the compiler is GCC or -stdlib=libstdc++
# is set.

function(_stormbyte_cxx_blob _out)
	set(_blob "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_INIT} $ENV{CXXFLAGS}")
	if(CMAKE_BUILD_TYPE)
		string(TOUPPER "${CMAKE_BUILD_TYPE}" _cfg)
		string(APPEND _blob " ${CMAKE_CXX_FLAGS_${_cfg}}")
	endif()
	set(${_out} "${_blob}" PARENT_SCOPE)
endfunction()

function(_stormbyte_stdlib _out)
	_stormbyte_cxx_blob(_blob)
	if(_blob MATCHES "(^| )-stdlib=libc\\+\\+")
		set(${_out} "libc++" PARENT_SCOPE)
	elseif(_blob MATCHES "(^| )-stdlib=libstdc\\+\\+")
		set(${_out} "libstdc++" PARENT_SCOPE)
	elseif(APPLE AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(${_out} "libc++" PARENT_SCOPE)
	else()
		set(${_out} "libstdc++" PARENT_SCOPE)
	endif()
endfunction()

function(stormbyte_component_url out tag)
	if("${out}" STREQUAL "" OR "${tag}" STREQUAL "")
		message(FATAL_ERROR "stormbyte_component_url: <outvar> and <tag> are required")
	endif()

	if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
		set(_arch "arm64")
	elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64|x64)$")
		set(_arch "x64")
	else()
		message(FATAL_ERROR
			"stormbyte_component_url: unsupported architecture '${CMAKE_SYSTEM_PROCESSOR}'")
	endif()

	if(WIN32)
		set(_tag "windows-${_arch}")
	elseif(APPLE)
		_stormbyte_stdlib(_stdlib)
		set(_tag "macos-${_arch}-${_stdlib}")
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		_stormbyte_stdlib(_stdlib)
		set(_tag "linux-${_arch}-${_stdlib}")
	else()
		message(FATAL_ERROR
			"stormbyte_component_url: unsupported system '${CMAKE_SYSTEM_NAME}'")
	endif()

	set(_name "StormByte-${tag}-${_tag}.zip")
	set(${out}
		"https://github.com/StormByte-Suite/StormByte/releases/download/${tag}/${_name}"
		PARENT_SCOPE)
endfunction()
