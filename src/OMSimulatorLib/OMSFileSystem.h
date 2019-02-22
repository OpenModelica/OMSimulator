#ifndef _OMS_FILESYSTEM_H_

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
static inline filesystem::path oms_temp_directory_path(void) {
  return filesystem::temp_directory_path();
}
static inline filesystem::path oms_canonical(filesystem::path p) {
  return filesystem::canonical(p);
}

#else
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

#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;

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

#endif

void oms_copy_file(const filesystem::path &from, const filesystem::path &to);
filesystem::path oms_unique_path(const std::string& prefix);

#endif
