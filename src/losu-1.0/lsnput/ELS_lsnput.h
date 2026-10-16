#ifndef ELS_LSINPUT_H_
#define ELS_LSINPUT_H_

#include "els.h"

int ELSAPI_lsnput_GetMousePosition(els_VmObj* vm);
int ELSAPI_lsnput_SetMousePosition(els_VmObj* vm);
int ELSAPI_lsnput_MouseAct(els_VmObj* vm);
int ELSAPI_lsnput_click(els_VmObj *vm);

#endif
