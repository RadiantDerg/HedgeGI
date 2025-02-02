﻿#include "Game.h"

#include "FileStream.h"
#include "Utilities.h"

const char* const GAME_NAMES[] =
{
    "Unknown",
    "Sonic Unleashed",
    "Sonic Generations",
    "Sonic Lost World",
    "Sonic Forces"
};

Game detectGameFromStageDirectory(const std::string& directoryPath)
{
    Game game = Game::Unknown;

    if (std::filesystem::exists(directoryPath + "/Stage.pfd"))
    {
        if (std::filesystem::exists(directoryPath + "/" + getFileName(directoryPath) + ".ar.00"))
            game = Game::Generations;
        else
            game = Game::Unleashed;
    }

    else
    {
        const FileStream fileStream((directoryPath + "/" + getFileName(directoryPath) + "_trr_cmn.pac").c_str(), "rb");
        if (fileStream.isOpen())
        {
            fileStream.seek(4, SEEK_SET);
            const char version = fileStream.read<char>();

            game = version == '3' ? Game::Forces : version == '2' ? Game::LostWorld : Game::Unknown;
        }
    }

    return game;
}
