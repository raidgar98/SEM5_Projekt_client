#pragma once

#include <string>

using str = std::wstring;

struct settings abstract
{

	inline static str api_port;
	inline static str fport;
	inline static str protocol = L"http://";
	inline static str download_directory;

};

#define api_port settings::api_port
#define fport settings::fport
#define api_protocol settings::protocol
#define download_directory settings::download_directory
