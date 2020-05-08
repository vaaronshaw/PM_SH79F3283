#include "error.h"

static TErrorDef ERR_tErrorCode = 0;



TErrorDef ERR_tGetErrorCode(void)
{
	return ERR_tErrorCode;
}