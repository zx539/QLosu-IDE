#include "lsffi.h"

extern uint64_t lsffi_cdel_intargsx__(uint64_t args[6], uint64_t *eargs, void *fun, uint64_t len);

uint64_t lsffi_cdel_intargsx(uint64_t args[6], uint64_t *eargs, void *fun, uint64_t len)
{
    return lsffi_cdel_intargsx__(args, eargs, fun, len);
}
