#pragma once

#include "settings.hpp"
#include <random>
#include <cpprest/http_client.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class MainComponent;

class BasicRefresher
{
protected:

	MainComponent* src;
	std::vector<std::wstring> address_pool;
	
	explicit BasicRefresher(MainComponent* i_src);

	pplx::task<void> POST_request(const std::wstring& host, const std::wstring& uri, json::value& in_out) const;

	pplx::task<void> GET_request(const std::wstring& host, const std::wstring& uri, json::value& out) const;

public:

	virtual void operator()() = 0;

	BasicRefresher(const BasicRefresher&) = default;
	BasicRefresher(BasicRefresher&&) = default;
	BasicRefresher& operator=(BasicRefresher&&) = default;
	BasicRefresher& operator=(const BasicRefresher&) = default;

	~BasicRefresher() { src = nullptr; }
};