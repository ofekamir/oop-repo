set(user1 ofekamir)
set(user2 olshanezky)
set(user3 orelbenishai)
set(id1 "305637688")
set(id2 "203954276")
set(id3 "301798617")
set (SOURCE_FILES
	GameUtils.h
	IBattleshipGameAlgo.h
	Player.h
	Ship.h
	Game.cpp
	GameUtils.cpp
	Player.cpp
	Ship.cpp
)

### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
	${SOURCE_FILES}
)
#BUILD_BINARY_OUTPUT_DIRECTORY
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")