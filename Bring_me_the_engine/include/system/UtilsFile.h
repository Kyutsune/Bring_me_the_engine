#pragma once

#include <fstream>
#include <iostream>


namespace UtilsFile {
    void saveSettingsSensRot(const std::string & filepath);

    void loadSettingsSensRot(const std::string & filepath);
}