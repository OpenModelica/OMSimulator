#ifndef _OMS_FILESYSTEM_H_
#define _OMS_FILESYSTEM_H_

#if !defined(WITHOUT_FS) && defined(__has_include)

#if __has_include(<filesystem>)
#include <filesystem>
#if __cpp_lib_filesystem >= 201703
namespace filesystem = std::filesystem;
#define OMC_STD_FS 1
#endif
#endif

#if !defined(OMC_STD_FS) && __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem::v1;
#define OMC_STD_FS 1
#endif

#endif

#if OMC_STD_FS == 1

#define OMS_RECURSIVE_DIRECTORY_ITERATOR(path) (filesystem::recursive_directory_iterator{path})

#else // boost part

#include <string>
#include <boost/version.hpp>
// boost version < 1.57 has issues linking boost::filesystem::copy_file
// https://svn.boost.org/trac10/ticket/6124
#if (BOOST_VERSION < 105700)
#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif // #if (BOOST_VERSION < 105700)
#include <boost/filesystem.hpp>
#if (BOOST_VERSION < 105700)
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#endif // #if (BOOST_VERSION < 105700)

#if (BOOST_VERSION >= 105300)
#include <boost/lockfree/queue.hpp>
#endif

#if (BOOST_VERSION < 105500)
#include <boost/range.hpp>
#define OMS_RECURSIVE_DIRECTORY_ITERATOR(path) (boost::make_iterator_range(filesystem::recursive_directory_iterator{path}, {}))
#else // older boost
#define OMS_RECURSIVE_DIRECTORY_ITERATOR(path) (filesystem::recursive_directory_iterator{path})
#endif

#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;
#endif

filesystem::path oms_temp_directory_path(void);
filesystem::path oms_canonical(filesystem::path p);
void oms_copy_file(const filesystem::path& from, const filesystem::path& to);
filesystem::path oms_unique_path(const std::string& prefix);
filesystem::path naive_uncomplete(const filesystem::path& path, const filesystem::path& base);
filesystem::path oms_absolute(const filesystem::path& p);

#endif
