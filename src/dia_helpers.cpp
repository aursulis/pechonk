#include "dia_helpers.h"

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/win32_helpers.h>
#include <wil/result_macros.h>

#include <dia2.h>

namespace pechonk {
DiaCtx LoadDia()
{
    // TODO: see if registration-free COM (some sort of sxs manifest) can help
    // to make CoCreateInstance work directly here.
    // auto source = wil::CoCreateInstance<IDiaDataSource>(CLSID_DiaSource, CLSCTX_INPROC_SERVER);

    auto dia_mod = wil::unique_hmodule{ ::LoadLibraryW(L"msdia140.dll") };
    THROW_LAST_ERROR_IF_NULL(dia_mod);

    auto dia_GetClassObject = GetProcAddressByFunctionDeclaration(dia_mod.get(), DllGetClassObject);
    THROW_LAST_ERROR_IF_NULL(dia_GetClassObject);

    auto class_factory = wil::com_ptr<IClassFactory>{};
    auto hr = dia_GetClassObject(CLSID_DiaSource, IID_IClassFactory, class_factory.put_void());
    THROW_IF_FAILED(hr);

    auto data_source = wil::com_ptr<IDiaDataSource>{};
    hr = class_factory->CreateInstance(nullptr, IID_IDiaDataSource, data_source.put_void());
    THROW_IF_FAILED(hr);

    return { std::move(dia_mod), std::move(data_source) };
}
} // namespace pechonk
