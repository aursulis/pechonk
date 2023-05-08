#include <wil/com.h>
#include <wil/resource.h>
#include <wil/win32_helpers.h>
#include <wil/result_macros.h>

#include <dia2.h>

#include <format>
#include <iostream>

int wmain(int argc, wchar_t** argv)
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

    if (argc != 2) {
        std::cerr << "Specify pdb file\n";
        return 1;
    }

    auto pdb_file = std::wstring{ argv[1] };

    hr = data_source->loadDataFromPdb(pdb_file.c_str());
    THROW_IF_FAILED(hr);

    auto session = wil::com_ptr<IDiaSession>{};
    hr = data_source->openSession(session.put());
    THROW_IF_FAILED(hr);

    auto enum_exports = wil::com_ptr<IDiaEnumSymbols>{};
    hr = session->getExports(enum_exports.put());
    THROW_IF_FAILED(hr);

    auto symbol = wil::com_ptr<IDiaSymbol>{};
    auto nfetched = ULONG{};
    while (SUCCEEDED(hr = enum_exports->Next(1, symbol.put(), &nfetched)) && nfetched == 1) {
        auto name = wil::unique_bstr{};
        hr = symbol->get_name(name.put());
        THROW_IF_FAILED(hr);

        std::printf("%S\n", name.get());
    }

    return 0;
}
