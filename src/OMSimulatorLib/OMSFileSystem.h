#ifndef _OMS_FILESYSTEM_H_

#ifdef __has_include
#if __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#define OMC_STD_FS 1
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem::v1;
#define OMC_STD_FS 1
#endif
#endif

#if OMC_STD_FS == 1
static inline filesystem::path oms_temp_directory_path(void) {
  return filesystem::temp_directory_path();
}
static inline filesystem::path oms_canonical(filesystem::path p) {
  return filesystem::canonical(p);
}

#else
#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;

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
#include <ctpl.h>
#else // use the standard queue
#include <ctpl_stl.h>
#endif


#if (BOOST_VERSION >= 104600) // no temp_directory_path in boost < 1.46
static inline filesystem::path oms_temp_directory_path(void) {
  return filesystem::temp_directory_path();
}
static inline filesystem::path oms_canonical(filesystem::path p) {
  return filesystem::canonical(p);
}
#else
filesystem::path oms_temp_directory_path(void);
filesystem::path oms_canonical(filesystem::path p);
#endif

namespace boost::filesystem::copy_options {
const filesystem::copy_option overwrite_existing = filesystem::copy_option::overwrite_if_exists;
}
#endif

filesystem::path oms_unique_path(const std::string& prefix);

#endif
