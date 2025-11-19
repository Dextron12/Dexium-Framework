//
// Created by Dextron12 on 19/11/2025.
//

#ifndef DEXIUM_RESOURCE_HPP
#define DEXIUM_RESOURCE_HPP

#include <string>
#include <vector>
#include <filesystem>

#include <utils//ID.hpp>


namespace Dexium::Core {

    class FileEntry {
    public:
        std::string data; // Either raw input data for a system or a filePath to load data from
        std::filesystem::file_time_type lastModified;

        bool isPathValid = false; // True, if the data is a valid path/file

        FileEntry(const std::string& data);

        bool shouldReload() const; // Checks the last write time of the file and if it doesn tmatch with lastModified, returns true
    };

}


#endif //DEXIUM_RESOURCE_HPP