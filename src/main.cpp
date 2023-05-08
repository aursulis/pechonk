#include "dia_helpers.h"

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include <dia2.h>

#include <format>
#include <iostream>

int wmain(int argc, wchar_t** argv)
{
    auto dia_ctx = pechonk::LoadDia();
    auto& data_source = dia_ctx.data_source;

    if (argc != 2) {
        std::cerr << "Specify pdb file\n";
        return 1;
    }

    auto pdb_file = std::wstring{ argv[1] };

    auto hr = data_source->loadDataFromPdb(pdb_file.c_str());
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
