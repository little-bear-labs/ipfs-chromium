#ifndef IPFS_EXPORT_H_
#define IPFS_EXPORT_H_

#if __has_include(<base/component_export.h>)
#include <base/component_export.h>
#else

#ifndef IS_IPFS_IMPL
#if !defined(COMPONENT_EXPORT)
#define COMPONENT_EXPORT(IPFS)
#endif
#endif

#endif

#endif  // IPFS_EXPORT_H_
