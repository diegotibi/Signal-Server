cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(json LANGUAGES CXX)

# ISO standard compliance (C++14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimization
set(CMAKE_CXX_FLAGS "-O3")

# Link time optimization (LTO)
# https://documentation.suse.com/sbp/server-linux/html/SBP-GCC-12/index.html#sec-gcc12-lto
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output LANGUAGES CXX)
if(result)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
	message(STATUS "LTO is supported:  ${output}")
else()
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
	message(WARNING "LTO is not supported:  ${output}")
endif()

# Compiler warnings
if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
	# Enable for security reasons several runtime protections and lots of warnings
	# https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++
    add_compile_options(-Wall -Wextra -Wformat=2 -Wconversion -Wsign-conversion 
						-Wtrampolines -Wimplicit-fallthrough 
						-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 
						-fstrict-flex-arrays=3 
						-fstack-clash-protection -fstack-protector-strong 
						-Wl,-z,nodlopen -Wl,-z,noexecstack 
						-Wl,-z,relro -Wl,-z,now 
						-fPIE -pie
						-D_GLIBCXX_ASSERTIONS
						-fcf-protection=full
						-march=x86-64-v3
						)
endif()

# Linker options
# Enable for security reasons several runtime protections
add_link_options(-Wl,-z,nodlopen -Wl,-z,noexecstack 
				 -Wl,-z,relro -Wl,-z,now 
				 -fPIE -pie)

# external libraries
find_library(jansson jansson)
find_library(curl curl)

add_executable(json json.cc)

target_link_libraries(json
			PUBLIC jansson
		    PUBLIC curl
		  )

# INSTALL target (make install)
install(TARGETS json)

