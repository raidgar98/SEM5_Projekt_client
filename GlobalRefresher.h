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

	double disk_usage() const
	{
		json::value data;
		double ret = 0.0;
		for (const auto &var : address_pool)
		{
			try { GET_request(protocol + var + L":" + port, L"/api/disk/space_total_free", data).wait(); }
			catch (...) { continue; }
			if (data.is_null()) continue;
			ret = (ret + data[L"space_total_free"].as_double()) / (ret == 0 ? 1.0 : 2.0);
		}
		return 1.0 - ret;
	}

	double processor_usage() const
	{
		json::value data;
		double ret = 0.0;
		for (const auto& var : address_pool)
		{
			try { GET_request(protocol + var + L":" + port, L"/api/cpu/usage", data).wait(); }
			catch (...) { continue; }
			if (data.is_null()) continue;
			ret = (ret +( data[L"usage"].as_double() / 100.0)) / ( ret == 0 ? 1.0 : 2.0 );
		}
		return ret;
	}

	double memory_usage() const
	{
		json::value data;
		double ret = 0.0;
		for (const auto& var : address_pool)
		{
			try { GET_request(L"http://" + var + L":9001", L"/api/mem/used", data).wait(); }
			catch (...) { continue; }
			if (data.is_null()) continue;
			ret = (ret + data[L"used"].as_double()) / (ret == 0 ? 1.0 : 2.0);
		}
		return ret;
	}

public:

	GlobalRefresher(MainComponent* src) : BasicRefresher{ src } { src = nullptr; }

	void operator()()
	{
		while (src->global_run.try_lock())
		{
			src->global_run.unlock();

			if (src->global_run.try_lock())
			{
				src->global_run.unlock();

				while (!src->globalDiskLck.try_lock()) {}
				src->globalDiskValue = disk_usage();
				src->globalDiskLck.unlock();
			}
			else return;

			if (src->global_run.try_lock())
			{
				src->global_run.unlock();

				while (!src->globalProcessorLck.try_lock()) {}
				src->globalProcessorValue = processor_usage();
				src->globalProcessorLck.unlock();
			}
			else return;

			if (src->global_run.try_lock())
			{
				src->global_run.unlock();

				while (!src->globalRAMLck.try_lock()) {}
				src->globalRAMValue = memory_usage();
				src->globalRAMLck.unlock();
			}
			else return;

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		
	}
};