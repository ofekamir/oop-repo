cmake_minimum_required(VERSION 3.7)
project(repo)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        BattleshipGame/Game.cpp
        BattleshipGame/GameUtils.cpp
        BattleshipGame/GameUtils.h
        BattleshipGame/IBattleshipGameAlgo.h
        BattleshipGame/Player.cpp
        BattleshipGame/Player.h
        BattleshipGame/Ship.cpp
        BattleshipGame/Ship.h)

add_executable(repo ${SOURCE_FILES})