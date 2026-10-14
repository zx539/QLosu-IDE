#include "quest/include/quest.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h> // 用于 malloc/free

#define M_PI 3.14159265358979323846

void
transform (Qureg reg, int a, int b, int c)
{
  applyHadamard (reg, b);
  applyControlledPauliX (reg, b, c);
  applyControlledPauliX (reg, a, b);
  applyHadamard (reg, a);

  if (applyQubitMeasurement (reg, a))
    {
      applyPauliZ (reg, c);
    }
  if (applyQubitMeasurement (reg, b))
    {
      applyPauliX (reg, c);
    }
}

int
main ()
{
  initQuESTEnv ();
  Qureg reg = createQureg (3);
  applyRotateX (reg, 0, M_PI / 3);
  transform (reg, 0, 1, 2);
  printf ("%g,%g\n", calcProbOfQubitOutcome (reg, 2, 0),
          calcProbOfQubitOutcome (reg, 2, 1));
}