// Copyright Benoit Blanchon 2014-2017
// MIT License
//
// Arduino JSON library
// https://bblanchon.github.io/ArduinoJson/
// If you like this project, please add a star!

#pragma once

#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t

#include "../Configuration.hpp"
#include "../Polyfills/attributes.hpp"
#include "../StringTraits/StringTraits.hpp"
#include "../TypeTraits/EnableIf.hpp"
#include "../TypeTraits/IsArray.hpp"
#include "NonCopyable.hpp"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

namespace ArduinoJson {
namespace Internals {
// Handles the memory management (done in derived classes) and calls the
// parser.
// This abstract class is implemented by StaticJsonBuffer which implements a
// fixed memory allocation.
class JsonBuffer : Internals::NonCopyable {
 public:
  // CAUTION: NO VIRTUAL DESTRUCTOR!
  // If we add a virtual constructor the Arduino compiler will add malloc() and
  // free() to the binary, adding 706 useless bytes.
  // virtual ~JsonBuffer() {}

  // Duplicates a string
  //
  // char* strdup(TValue);
  // TValue = const std::string&, const String&,
  template <typename TString>
  typename TypeTraits::EnableIf<!TypeTraits::IsArray<TString>::value,
                                char *>::type
  strdup(const TString &src) {
    return Internals::StringTraits<TString>::duplicate(src, this);
  }
  //
  // char* strdup(TValue);
  // TValue = const char*, const char[N], const FlashStringHelper*
  template <typename TString>
  char *strdup(const TString *src) {
    return Internals::StringTraits<const TString *>::duplicate(src, this);
  }

  // Allocates n bytes in the JsonBuffer.
  // Return a pointer to the allocated memory or NULL if allocation fails.
  virtual void *alloc(size_t size) = 0;

 protected:
  // Preserve aligment if necessary
  static FORCE_INLINE size_t round_size_up(size_t bytes) {
#if ARDUINOJSON_ENABLE_ALIGNMENT
    const size_t x = sizeof(void *) - 1;
    return (bytes + x) & ~x;
#else
    return bytes;
#endif
  }
};
}
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
#endif
