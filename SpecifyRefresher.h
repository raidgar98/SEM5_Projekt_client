#pragma once

#include "BasicRefresher.h"
#include <random>
#include <cpprest/http_client.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

#define set_progress_bar(pb, val) if(!src->specify_run.test_and_set(std::memory_order_acquire))\
{\
	src->specify_run.clear(std::memory_order_release);\
	\
	while(!src->GLUER(pb,Lc,k).test_and_set(std::memory_order_acquire)) {}\
	src->GLUER(pb,Valu,e) = val;\
	src->GLUER(pb, Lc,k).clear(std::memory_order_release);\
}else return;

#define chck_close(x) if(!src->specify_run.test_and_set(std::memory_order_acquire)){ src->specify_run.clear(std::memory_order_release); x; } else return;

class SpecifyRefresher : public BasicRefresher
{
private:

	const str addr;

	int get_api_int(const str& url, const str& key);

	double get_api_real(const str& url, const str& key);

	str get_api_str(const str& url, const str& key);

	str get_api_SI(const str& url, const str& key);

	str get_api_version(const str& url, const str& key, const str& additional_wrapper = str());

	str get_api_array(const str& url, const str& key);

	std::vector<str> get_api_array(const str& url, const str& key, bool);

	std::vector<int> get_api_array_num(const str& url, const str& key);

public:
	
	explicit SpecifyRefresher(MainComponent* src, const str& i_addr) : BasicRefresher{ src }, addr{ i_addr } { src = nullptr; }
	SpecifyRefresher(SpecifyRefresher&&) = default;
	SpecifyRefresher(const SpecifyRefresher&) = default;
	SpecifyRefresher& operator=(const SpecifyRefresher&) = default;
	SpecifyRefresher& operator=(SpecifyRefresher&&) = default;

	virtual void operator()() override;	

};