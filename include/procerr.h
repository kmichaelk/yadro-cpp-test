#ifndef PROCESSING_ERRORS_H
#define PROCESSING_ERRORS_H

enum class processing_error {
#define EXPAND_PROCERR(name) name,
#include "xprocerr.def"
#undef EXPAND_PROCERR
};

#endif // PROCESSING_ERRORS_H
