cmake_minimum_required(VERSION 2.7)
project(Zavlaha)
set(CMAKE_CXX_STANDARD 14)

set(Boost_NO_BOOST_CMAKE TRUE CACHE BOOL "" FORCE)
set(BOOST_MIN_VERSION "1.70.0")
find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(RestApiServer 1.0.0 REQUIRED)

add_executable(Zavlaha 
	db/sqlite3.c
	db/sqlitedb.cpp
	db/db.cpp
	log.cpp
	main.cpp 
	interval.cpp
	gpio.cpp
	gpio_config.cpp
	restapi.cpp
	restapi_impl.cpp 
	storage.cpp 
	water_manager.cpp)

set(AdditionalLibs ${RESTAPISERVER_LIBRARIES} ${Boost_LIBRARIES} -lpthread -ldl)
if (RASPBERRY_PI)
	set(AdditionalLibs "${AdditionalLibs}" "${wiringPi}")
	add_definitions(-DRASPBERRY_PI)
endif()

target_link_libraries(Zavlaha ${AdditionalLibs})
