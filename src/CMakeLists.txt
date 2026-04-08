cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(SignalServer LANGUAGES CXX)

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


# Compiler flags and warnings
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

# Get GIT commit branch & hash
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Linker options
# Enable for security reasons several runtime protections
add_link_options(-Wl,-z,nodlopen -Wl,-z,noexecstack 
				 -Wl,-z,relro -Wl,-z,now 
				 -fPIE -pie)

# Prepare internal libraries search
include_directories(models)

# internal libraries
add_library(image STATIC image.cc)
add_library(image-ppm STATIC image-ppm.cc)
add_library(image-png STATIC image-png.cc)
add_library(inputs STATIC inputs.cc)
add_library(outputs STATIC outputs.cc)
add_library(tiles STATIC tiles.cc)
add_library(geo STATIC geo.cc)

add_library(cost STATIC models/cost.cc)
add_library(ecc33 STATIC models/ecc33.cc)
add_library(egli STATIC models/egli.cc)
add_library(ericsson STATIC models/ericsson.cc)
add_library(fspl STATIC models/fspl.cc)
add_library(hata STATIC models/hata.cc)
add_library(itwom3.0 STATIC models/itwom3.0.cc)
add_library(los STATIC models/los.cc models/egli.cc)
add_library(pel STATIC models/pel.cc)
add_library(soil STATIC models/soil.cc)
add_library(sui STATIC models/sui.cc)

# external libraries
find_library(bz2 bz2)
find_library(dl dl)
find_library(pthread pthread)
find_library(z z)
find_package(PNG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNGPKG REQUIRED libpng)

target_include_directories(image-png PRIVATE ${PNGPKG_INCLUDE_DIRS})
target_link_directories(image-png PRIVATE ${PNGPKG_LIBRARY_DIRS})
target_link_libraries(image-png PUBLIC ${PNGPKG_LIBRARIES})

# Spdlog
find_package(spdlog REQUIRED)

add_executable(signalserver main.cc)
add_executable(signalserverHD main.cc)
add_executable(signalserverLIDAR main.cc)

target_compile_definitions(
    signalserver PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\""
    "-DGIT_BRANCH=\"${GIT_BRANCH}\""
)

target_compile_definitions(
    signalserverHD PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\""
    "-DGIT_BRANCH=\"${GIT_BRANCH}\""
)

target_compile_definitions(
    signalserverLIDAR PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\""
    "-DGIT_BRANCH=\"${GIT_BRANCH}\""
)

target_link_libraries(signalserver 
			PUBLIC image
			PUBLIC image-ppm
			PUBLIC image-png
			PUBLIC inputs
			PUBLIC outputs
			PUBLIC tiles
			PUBLIC cost
			PUBLIC ecc33
			PUBLIC egli
			PUBLIC ericsson
			PUBLIC fspl
			PUBLIC hata
			PUBLIC itwom3.0
			PUBLIC los
			PUBLIC pel
			PUBLIC soil
			PUBLIC sui
			PUBLIC bz2
			PUBLIC dl
			PUBLIC pthread
			PUBLIC z
			PUBLIC PNG::PNG
            PUBLIC spdlog::spdlog
            PUBLIC geo
		  )

target_link_libraries(signalserverHD 
			PUBLIC image
			PUBLIC image-ppm
			PUBLIC image-png
			PUBLIC inputs
			PUBLIC outputs
			PUBLIC tiles
			PUBLIC cost
			PUBLIC ecc33
			PUBLIC egli
			PUBLIC ericsson
			PUBLIC fspl
			PUBLIC hata
			PUBLIC itwom3.0
			PUBLIC los
			PUBLIC pel
			PUBLIC soil
			PUBLIC sui
			PUBLIC bz2
			PUBLIC dl
			PUBLIC pthread
			PUBLIC z
			PUBLIC PNG::PNG
            PUBLIC spdlog::spdlog
            PUBLIC geo
		  )

target_link_libraries(signalserverLIDAR 
			PUBLIC image
			PUBLIC image-ppm
			PUBLIC image-png
			PUBLIC inputs
			PUBLIC outputs
			PUBLIC tiles
			PUBLIC cost
			PUBLIC ecc33
			PUBLIC egli
			PUBLIC ericsson
			PUBLIC fspl
			PUBLIC hata
			PUBLIC itwom3.0
			PUBLIC los
			PUBLIC pel
			PUBLIC soil
			PUBLIC sui
			PUBLIC bz2
			PUBLIC dl
			PUBLIC pthread
			PUBLIC z
			PUBLIC PNG::PNG
            PUBLIC spdlog::spdlog
            PUBLIC geo
		  )

# INSTALL target (make install)
install(TARGETS signalserver)
install(TARGETS signalserverHD)
install(TARGETS signalserverLIDAR)
