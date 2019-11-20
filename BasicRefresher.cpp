#include "BasicRefresher.h"

#include "MainComponent.h"

BasicRefresher::BasicRefresher(MainComponent* i_src)
	:src{i_src}
{
	while (!src->mtx_addresses.try_lock()) {}
	address_pool = src->addresses;
	src->mtx_addresses.unlock();
	i_src = nullptr;
}

pplx::task<void> BasicRefresher::POST_request(const std::wstring& host, const std::wstring& uri, json::value& in_out) const
{
	return http_client(host).request(methods::POST, uri, in_out.serialize(), L"application/json").then([&](http_response resp)
		{
			if (resp.status_code() != status_codes::OK) in_out = json::value::null();
			in_out = json::value::parse(resp.extract_string().get());
		});
}

pplx::task<void> BasicRefresher::GET_request(const std::wstring& host, const std::wstring& uri, json::value& out) const
{
	return http_client(host).request(methods::GET, uri).then([&](http_response resp)
		{
			if (resp.status_code() != status_codes::OK) out = json::value::null();
			out = json::value::parse(resp.extract_string().get());
		});
}
