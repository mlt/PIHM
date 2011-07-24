#ifndef IS_SM_ET_H
#define IS_SM_ET_H

#include <sundials/sundials_types.h>
#include <nvector/nvector_serial.h>
#include "pihm.h"

void is_sm_et(realtype, realtype, model_data_structure *DS, N_Vector);

#endif
