
add_sources(libopenage
	container_iterator.cpp
	event.cpp
	eventlist.cpp
	filter_iterator.cpp
	tube_continuous.cpp
	tube_discrete.cpp
	tube_type.cpp
	tube_internal.cpp
	types.cpp
	unordered_map.cpp
)

add_subdirectory(test)
add_subdirectory(demo)
