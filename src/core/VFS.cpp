//
// Created by Dextron12 on 6/10/2025.
//

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <core/VFS.hpp>

#include "core/AssetManager.hpp"
#include "core/Error.hpp"
#include "Dexium.hpp"

#include <filesystem>

std::filesystem::path Dexium::VFS::_execPath;

void Dexium::VFS::init() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    _execPath = std::filesystem::path(buffer).parent_path(); // Gets the working directory
#endif

    // If in DEBUG move two folders up to be in project Workspace
#ifdef DEXIUM_DEBUG
    _execPath = _execPath.parent_path(); // Moves another folder up
#endif

    //auto& ctx = EngineState::get();
    //ctx.VFS_INIT = true;
}

std::unique_ptr<std::filesystem::path> Dexium::VFS::resolve(const std::string &relPath) {
    if (_execPath.empty()) {
        TraceLog(Dexium::LOG_WARNING, "VFS execPath is invalid\nDid you forget to call VFS::Init()?");
        return nullptr;
    }

    std::filesystem::path rel(relPath);
    std::filesystem::path abs = _execPath / rel;

    std::filesystem::path finalPath;
    try {
        finalPath = std::filesystem::canonical(abs);
    }
    catch (const std::filesystem::filesystem_error& e) {
        finalPath = abs.lexically_relative(abs);
    }

    // Validate path
    if (std::filesystem::exists(finalPath)) {
        return std::make_unique<std::filesystem::path>(finalPath);
    } else {
        TraceLog(LOG_WARNING, "[VFS::Resolve]: Failed to resolve path {}", relPath);
        return nullptr;
    }
}

bool Dexium::VFS::exists(const std::filesystem::path &path) {
    return std::filesystem::exists(path);
}

bool Dexium::VFS::exists(const std::string &path) {
    return std::filesystem::exists(path);
}

std::filesystem::path Dexium::VFS::getExecutablePath() {
    return _execPath;
}



