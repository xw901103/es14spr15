#include "OS.h"

void main(void);

void panic(void)
{
  for(;;);
}

void bootstrap(void)
{
  TOSEnvironment OSEnvironment =
  {
    16000000,
    24000000,
    &main,
    &panic
  };

  if (OS_SetEnvironment(&OSEnvironment) == OSERROR_NO_ERROR)
  {    
    (void)OS_Execute();
  }
  panic();
}
