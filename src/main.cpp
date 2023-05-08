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

    auto enum_syms = wil::com_ptr<IDiaEnumSymbolsByAddr>{};
    hr = session->getSymbolsByAddr(enum_syms.put());
    THROW_IF_FAILED(hr);

    auto symbol = wil::com_ptr<IDiaSymbol>{};
    hr = enum_syms->symbolByAddr(1, 0, symbol.put());
    THROW_IF_FAILED(hr);

    auto rva = DWORD{};
    hr = symbol->get_relativeVirtualAddress(&rva);
    THROW_IF_FAILED(hr);

    hr = enum_syms->symbolByRVA(rva, symbol.put());
    THROW_IF_FAILED(hr);

    auto nfetched = ULONG{};
    do {
        auto name = wil::unique_bstr{};
        hr = symbol->get_name(name.put());
        THROW_IF_FAILED(hr);

        rva = 0;
        hr = symbol->get_relativeVirtualAddress(&rva);

        std::printf("%S : %d\n", name.get(), rva);

        hr = enum_syms->Next(1, symbol.put(), &nfetched);
    } while (SUCCEEDED(hr) && nfetched == 1);

    return 0;
}
