#pragma once

#include <algorithm>
#include <vector>
#include <initializer_list>
#include <TFile.h>
#include "exception.h"

namespace hh_limits {

template<typename T>
std::vector<std::string> ToStrVector(const std::vector<T>& v)
{
    std::vector<std::string> result;
    std::transform(v.begin(), v.end(), std::back_inserter(result),
                   [](const T& x) { std::ostringstream ss; ss << x; return ss.str(); });
    return result;
}

inline std::shared_ptr<TFile> CreateRootFile(const std::string& file_name)
{
    std::shared_ptr<TFile> file(TFile::Open(file_name.c_str(), "RECREATE"));
    if(file->IsZombie())
        throw analysis::exception("File '%1%' not created.") % file_name;
    return file;
}

inline std::string FullPath(std::initializer_list<std::string> paths)
{
    if(!paths.size())
        return "";

    std::ostringstream full_path;
    auto iter = paths.begin();
    full_path << *iter++;
    for(; iter != paths.end(); ++iter)
        full_path << "/" << *iter;
    return full_path.str();
}

} // namespace hh_limits
