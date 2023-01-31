#ifndef _OMS_FILESYSTEM_H_
#define _OMS_FILESYSTEM_H_

#include <filesystem>
namespace filesystem = std::filesystem;
#define OMC_STD_FS 1

#define OMS_RECURSIVE_DIRECTORY_ITERATOR(path) (filesystem::recursive_directory_iterator{path})

filesystem::path naive_uncomplete(const filesystem::path& path, const filesystem::path& base);
filesystem::path oms_unique_path(const std::string& prefix);
void oms_copy_file(const filesystem::path& from, const filesystem::path& to);
filesystem::path oms_canonical(const filesystem::path& p);
filesystem::path oms_absolute(const filesystem::path& p);
filesystem::path oms_temp_directory_path(void);

#endif
