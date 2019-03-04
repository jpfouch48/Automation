#ifndef KLEANER_DEFINES_H
#define KLEANER_DEFINES_H

enum class InputSource
{
  Water     = 0,
  Cleaner   = 1,
  Sanitizer = 2,

  None      = 3
};

enum class RecircDest
{
  Waste     = 0,
  Cleaner   = 1,
  Sanitizer = 2,

  None      = 3
};

#endif