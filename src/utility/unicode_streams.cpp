/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/unicode_streams.hpp>

#include <cstddef>
#include <cstdint>
#include <streambuf>
#include <string>
#ifdef _MSC_VER
    #include <fcntl.h>
    #include <io.h>
    #include <iostream>
    #include <stdexcept>
    #include <stdio.h>
    #include <string.h>
    #include <windows.h>
#endif

namespace libbitcoin {

constexpr int32_t utf8_code_page = 65001u;
constexpr size_t keyboard_buffer_size = 1024u;

// Initialize private static member.
bool unicode_streams::initialized_ = false;

// Set file mode to UTF8 no BOM (translated).
static void set_stdio_utf8(FILE* file)
{
#ifdef _MSC_VER
    if (!_setmode(_fileno(file), _O_U8TEXT))
        throw std::ios_base::failure("Failed to set stdio to utf8.");
#endif
}

// This class/mathod is a no-op on non-windows platforms.
// This is the factory method to privately instantiate a singleton class.
void unicode_streams::initialize()
{
    if (initialized_)
        return;

    initialized_ = true;

#ifdef _MSC_VER
    if (SetConsoleCP(utf8_code_page) == FALSE)
        throw std::ios_base::failure("Failed to set console to utf8.");

    DWORD console_mode;
    if (GetConsoleMode(get_input_handle(), &console_mode) != FALSE)
    {
        // Hack for faulty wcin translation of non-ASCII keyboard input.
        static unicode_streams buffer(*std::wcin.rdbuf());
        std::wcin.rdbuf(&buffer);
    }

    // Set all stdio to wide streaming.
    set_stdio_utf8(stdin);
    set_stdio_utf8(stdout);
    set_stdio_utf8(stderr);

    //// Set console font to Lucida Console 16 (to see non-ASCII better).
    //CONSOLE_FONT_INFOEX font;
    //font.cbSize = sizeof(font);
    //font.nFont = 0;
    //font.dwFontSize.X = 0;
    //font.dwFontSize.Y = 16;
    //font.FontFamily = FF_DONTCARE;
    //font.FontWeight = FW_NORMAL;
    //wcscpy(font.FaceName, L"Lucida Console");
    //auto console = GetStdHandle(STD_OUTPUT_HANDLE);
    //SetCurrentConsoleFontEx(console, FALSE, &font);
#endif
}

unicode_streams::unicode_streams(wide_streambuf const& other)
#ifdef _MSC_VER
    : wide_streambuf(other), buffer_(keyboard_buffer_size, L'\0')
#endif
{
}

void* unicode_streams::get_input_handle()
{
    void* handle = nullptr;

#ifdef _MSC_VER
    handle = GetStdHandle(STD_INPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE || handle == nullptr)
        throw std::ios_base::failure("Failed to get input handle.");
#endif

    return handle;
}

std::streamsize unicode_streams::xsgetn(wchar_t* buffer, std::streamsize size)
{
    std::streamsize read_size = 0;

#ifdef _MSC_VER
    DWORD read_bytes;
    const auto result = ReadConsoleW(get_input_handle(), buffer,
        static_cast<DWORD>(size), &read_bytes, nullptr);

    if (result == FALSE)
        throw std::ios_base::failure("Failed to read from console.");

    read_size = static_cast<std::streamsize>(read_bytes);
#endif

    return read_size;
}

unicode_streams::traits::int_type unicode_streams::underflow()
{
    if (gptr() == nullptr || gptr() >= egptr())
    {
        const auto start = &buffer_[0];
        const auto length = xsgetn(start, buffer_.size());
        if (length > 0)
            setg(start, start, start + length);
    }

    if (gptr() == nullptr || gptr() >= egptr())
        return traits::eof();

    return traits::to_int_type(*gptr());
}

} // namespace libbitcoin
