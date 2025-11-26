//
// Created by Dextron12 on 6/10/2025.
//

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#ifdef __linux__
#include <unistd.h>
#include <limits>
#endif

#include <core/VFS.hpp>

#include "core/AssetManager.hpp"
#include "core/Error.hpp"
#include "core/Signal.hpp"

#include <filesystem>

std::filesystem::path Dexium::VFS::_execPath;

void Dexium::VFS::init() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    _execPath = std::filesystem::path(buffer).parent_path(); // Gets the working directory
#endif
#ifdef __linux__
    char buf[PATH_MAX] = {0};
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len == -1) {
        TraceLog(LOG_FATAL, "[ExecPath]: LINUX (FATAL ERROR): Failed to resolve execPath symlink");
    }
    buf[len] = '\0'; //null-termination
    _execPath = std::filesystem::path(buf).parent_path(); // Gets working directory
#endif

    // If in DEBUG move two folders up to be in project Workspace
#ifdef DEXIUM_DEBUG
    _execPath = _execPath.parent_path(); // Moves another folder up
#endif

    if (!_execPath.empty()) {
        SignalManager::get().emit("VFS_Init", true);
    } else {
        SignalManager::get().emit("VFS_Init", false);
    }
}

std::unique_ptr<std::filesystem::path> Dexium::VFS::resolve(const std::string &relPath) {
    if (_execPath.empty()) {
        TraceLog(Dexium::LOG_WARNING, "VFS execPath is invalid\nDid you forget to call VFS::Init()?");
        return nullptr;
    }

    std::filesystem::path rel(relPath);
    std::filesystem::path abs = (_execPath / rel).lexically_normal();

    //Validate path:
    if (exists(abs)) {
        return std::make_unique<std::filesystem::path>(abs);
    }

    //Otherwsie, warn of failed path and return nullptr
    TraceLog(LOG_WARNING, "[VFS::Resolve]: resolved path '{}' does not exist", abs.string());
    return nullptr;

}

bool Dexium::VFS::exists(const std::filesystem::path &path) {
    return std::filesystem::exists(path);
}

bool Dexium::VFS::exists(const std::string &path) {
    return std::filesystem::exists(path);
}

void Dexium::VFS::overwriteExecutablePath(std::string_view newPath) {
    std::filesystem::path npath = newPath;

    //Validate it
    if (std::filesystem::exists(npath) && std::filesystem::is_directory(npath)) {
        _execPath = npath;
        TraceLog(LOG_INFO, "Updated the working directory to: '{}'", npath.string());
    } else {
        TraceLog(LOG_ERROR, "[VFS]: Cannot change working dir to '{}', it does not meet the expectations of a working directory", npath.string());
    }
}


std::filesystem::path Dexium::VFS::getExecutablePath() {
    return _execPath;
}



