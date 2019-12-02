#include "GlobalRefresher.h"
#include "MainComponent.h"

 double GlobalRefresher::disk_usage() const
{
	json::value data;
	double ret = 0.0;
	for (const auto& var : address_pool)
	{
		try { GET_request(api_protocol + var + L":" + api_port, L"/api/disk/space_total_free", data).wait(); }
		catch (...) { continue; }
		if (data.is_null()) continue;
		ret = (ret + data[L"space_total_free"].as_double()) / (ret == 0 ? 1.0 : 2.0);
	}
	return 1.0 - ret;
}

 double GlobalRefresher::processor_usage() const
{
	json::value data;
	double ret = 0.0;
	for (const auto& var : address_pool)
	{
		try { GET_request(api_protocol + var + L":" + api_port, L"/api/cpu/usage", data).wait(); }
		catch (...) { continue; }
		if (data.is_null()) continue;
		ret = (ret + (data[L"usage"].as_double() / 100.0)) / (ret == 0 ? 1.0 : 2.0);
	}
	return ret;
}

 double GlobalRefresher::memory_usage() const
{
	json::value data;
	double ret = 0.0;
	for (const auto& var : address_pool)
	{
		try { GET_request(api_protocol + var + L":" + api_port, L"/api/mem/used", data).wait(); }
		catch (...) { continue; }
		if (data.is_null()) continue;
		ret = (ret + data[L"used"].as_double()) / (ret == 0 ? 1.0 : 2.0);
	}
	return ret;
}

void GlobalRefresher::operator()()
{
	while (!src->global_run.test_and_set(std::memory_order_acquire))
	{
		src->global_run.clear(std::memory_order_release);

		if (!src->global_run.test_and_set(std::memory_order_acquire))
		{
			src->global_run.clear(std::memory_order_release);

			while (!src->globalDiskLck.test_and_set(std::memory_order_acquire)) {}
			src->globalDiskValue = disk_usage();
			src->globalDiskLck.clear(std::memory_order_release);
		}
		else return;

		if (!src->global_run.test_and_set(std::memory_order_acquire))
		{
			src->global_run.clear(std::memory_order_release);

			while (!src->globalProcessorLck.test_and_set(std::memory_order_acquire)) {}
			src->globalProcessorValue = processor_usage();
			src->globalProcessorLck.clear(std::memory_order_release);
		}
		else return;

		if (!src->global_run.test_and_set(std::memory_order_acquire))
		{
			src->global_run.clear(std::memory_order_release);

			while (!src->globalRAMLck.test_and_set(std::memory_order_acquire)) {}
			src->globalRAMValue = memory_usage();
			src->globalRAMLck.clear(std::memory_order_release);
		}
		else return;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}
