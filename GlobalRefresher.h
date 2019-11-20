#pragma once

#include "BasicRefresher.h"
#include <random>
#include <cpprest/http_client.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class GlobalRefresher : private BasicRefresher
{
private:

	double disk_usage() const;

	double processor_usage() const;

	double memory_usage() const;

public:

	explicit GlobalRefresher(MainComponent* src) : BasicRefresher{ src } { src = nullptr; }
	GlobalRefresher(GlobalRefresher&&) = default;
	GlobalRefresher(const GlobalRefresher&) = default;
	GlobalRefresher& operator=(const GlobalRefresher&) = default;
	GlobalRefresher& operator=(GlobalRefresher&&) = default;

	void operator()() override;	

};