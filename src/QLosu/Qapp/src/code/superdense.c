#include "quest/include/quest.h"
#include <stdio.h>
int
main (int argc, char **argv)
{
  initQuESTEnv ();
  Qureg reg = createQureg (2);
  applyHadamard (reg, 0);
  applyControlledPauliX (reg, 0, 1);

  // 00
  // 01
  // applyPauliX (reg, 0);
  // 10
  applyPauliZ (reg, 0);
  // 11
  // applyPauliX (reg, 0);
  // applyPauliZ (reg, 0);

  applyControlledPauliX (reg, 0, 1);
  applyHadamard (reg, 0);

  printf ("%d,%d\n", applyQubitMeasurement (reg, 0),
          applyQubitMeasurement (reg, 1));
}