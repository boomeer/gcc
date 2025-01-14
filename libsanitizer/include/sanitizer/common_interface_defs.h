//===-- sanitizer/common_interface_defs.h -----------------------*- C++ -*-===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Common part of the public sanitizer interface.
//===----------------------------------------------------------------------===//

#ifndef SANITIZER_COMMON_INTERFACE_DEFS_H
#define SANITIZER_COMMON_INTERFACE_DEFS_H

#include <stddef.h>
#include <stdint.h>

// GCC does not understand __has_feature.
#if !defined(__has_feature)
# define __has_feature(x) 0
#endif

#ifdef __cplusplus
extern "C" {
#endif
  // Arguments for __sanitizer_sandbox_on_notify() below.
  typedef struct {
    // Enable sandbox support in sanitizer coverage.
    int coverage_sandboxed;
    // File descriptor to write coverage data to. If -1 is passed, a file will
    // be pre-opened by __sanitizer_sandobx_on_notify(). This field has no
    // effect if coverage_sandboxed == 0.
    intptr_t coverage_fd;
    // If non-zero, split the coverage data into well-formed blocks. This is
    // useful when coverage_fd is a socket descriptor. Each block will contain
    // a header, allowing data from multiple processes to be sent over the same
    // socket.
    unsigned int coverage_max_block_size;
  } __sanitizer_sandbox_arguments;

  // Tell the tools to write their reports to "path.<pid>" instead of stderr.
  void __sanitizer_set_report_path(const char *path);

  // Notify the tools that the sandbox is going to be turned on. The reserved
  // parameter will be used in the future to hold a structure with functions
  // that the tools may call to bypass the sandbox.
  void __sanitizer_sandbox_on_notify(__sanitizer_sandbox_arguments *args);

  // This function is called by the tool when it has just finished reporting
  // an error. 'error_summary' is a one-line string that summarizes
  // the error message. This function can be overridden by the client.
  void __sanitizer_report_error_summary(const char *error_summary);

  // Some of the sanitizers (e.g. asan/tsan) may miss bugs that happen
  // in unaligned loads/stores. In order to find such bugs reliably one needs
  // to replace plain unaligned loads/stores with these calls.
  uint16_t __sanitizer_unaligned_load16(const void *p);
  uint32_t __sanitizer_unaligned_load32(const void *p);
  uint64_t __sanitizer_unaligned_load64(const void *p);
  void __sanitizer_unaligned_store16(void *p, uint16_t x);
  void __sanitizer_unaligned_store32(void *p, uint32_t x);
  void __sanitizer_unaligned_store64(void *p, uint64_t x);

  // Annotate the current state of a contiguous container, such as
  // std::vector, std::string or similar.
  // A contiguous container is a container that keeps all of its elements
  // in a contiguous region of memory. The container owns the region of memory
  // [beg, end); the memory [beg, mid) is used to store the current elements
  // and the memory [mid, end) is reserved for future elements;
  // beg <= mid <= end. For example, in "std::vector<> v"
  //   beg = &v[0];
  //   end = beg + v.capacity() * sizeof(v[0]);
  //   mid = beg + v.size()     * sizeof(v[0]);
  //
  // This annotation tells the Sanitizer tool about the current state of the
  // container so that the tool can report errors when memory from [mid, end)
  // is accessed. Insert this annotation into methods like push_back/pop_back.
  // Supply the old and the new values of mid (old_mid/new_mid).
  // In the initial state mid == end and so should be the final
  // state when the container is destroyed or when it reallocates the storage.
  //
  // Use with caution and don't use for anything other than vector-like classes.
  //
  // For AddressSanitizer, 'beg' should be 8-aligned and 'end' should
  // be either 8-aligned or it should point to the end of a separate heap-,
  // stack-, or global- allocated buffer. I.e. the following will not work:
  //   int64_t x[2];  // 16 bytes, 8-aligned.
  //   char *beg = (char *)&x[0];
  //   char *end = beg + 12;  // Not 8 aligned, not the end of the buffer.
  // This however will work fine:
  //   int32_t x[3];  // 12 bytes, but 8-aligned under AddressSanitizer.
  //   char *beg = (char*)&x[0];
  //   char *end = beg + 12;  // Not 8-aligned, but is the end of the buffer.
  void __sanitizer_annotate_contiguous_container(const void *beg,
                                                 const void *end,
                                                 const void *old_mid,
                                                 const void *new_mid);
  // Returns true if the contiguous container [beg, end) is properly poisoned
  // (e.g. with __sanitizer_annotate_contiguous_container), i.e. if
  //  - [beg, mid) is addressable,
  //  - [mid, end) is unaddressable.
  // Full verification requires O(end-beg) time; this function tries to avoid
  // such complexity by touching only parts of the container around beg/mid/end.
  int __sanitizer_verify_contiguous_container(const void *beg, const void *mid,
                                              const void *end);

  // Print the stack trace leading to this call. Useful for debugging user code.
  void __sanitizer_print_stack_trace();

  // Sets the callback to be called right before death on error.
  // Passing 0 will unset the callback.
  void __sanitizer_set_death_callback(void (*callback)(void));

  // Interceptor hooks.
  // Whenever a libc function interceptor is called it checks if the
  // corresponding weak hook is defined, and it so -- calls it.
  // The primary use case is data-flow-guided fuzzing, where the fuzzer needs
  // to know what is being passed to libc functions, e.g. memcmp.
  // FIXME: implement more hooks.
  void __sanitizer_weak_hook_memcmp(void *called_pc, const void *s1,
                                    const void *s2, size_t n);
  void __sanitizer_weak_hook_strncmp(void *called_pc, const char *s1,
                                    const char *s2, size_t n);
#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // SANITIZER_COMMON_INTERFACE_DEFS_H
