#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <dia2.h>

namespace pechonk {

struct DiaCtx
{
	wil::unique_hmodule dia_mod;
	wil::com_ptr<IDiaDataSource> data_source;
};

DiaCtx LoadDia();
}
