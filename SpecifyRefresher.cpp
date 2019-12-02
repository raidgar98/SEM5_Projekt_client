#include "SpecifyRefresher.h"

#include "MainComponent.h"

int SpecifyRefresher::get_api_int(const str& url, const str& key)
{
	json::value data;
	int ret = 0;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return -1; }
	ret = data[key].as_integer();
	return ret;
}

double SpecifyRefresher::get_api_real(const str& url, const str& key)
{
	json::value data;
	double ret = 0.0;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return -1; }
	ret = data[key].as_double();
	return ret;
}

str SpecifyRefresher::get_api_str(const str& url, const str& key)
{
	json::value data;
	str ret{ L"" };
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return L"\0"; }
	ret = data[key].as_string();
	return ret + L'\0';
}

str SpecifyRefresher::get_api_SI(const str& url, const str& key)
{
	json::value data;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return L"\0"; }
	return std::to_wstring(data[key].as_double()) + L" " + data[L"SI"].as_string();
}

str SpecifyRefresher::get_api_version(const str& url, const str& key, const str& additional_wrapper)
{
	json::value data;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return L"\0"; }
	if (additional_wrapper != str())
		data = data[additional_wrapper];
	return std::to_wstring(data[key][L"major"].as_number().to_int32()) + L"." + std::to_wstring(data[key][L"minor"].as_number().to_int32());
}

str SpecifyRefresher::get_api_array(const str& url, const str& key)
{
	json::value data;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return L"\0"; }
	const auto ret = data[key].as_array();
	str ret_str{ L"" };
	for (const auto& var : ret)
		ret_str += var.as_string() + L"; ";
	return ret_str;
}

std::vector<str> SpecifyRefresher::get_api_array(const str& url, const str& key, bool)
{
	json::value data;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return std::vector<str>(); }
	const auto ret = data[key].as_array();
	std::vector<str> ret_vec;
	for (const auto& var : ret)
		ret_vec.emplace_back(var.as_string());
	return ret_vec;
}

std::vector<int> SpecifyRefresher::get_api_array_num(const str& url, const str& key)
{
	json::value data;
	try { GET_request(api_protocol + addr + L":" + api_port, url, data).wait(); }
	catch (...) { return std::vector<int>(); }
	const auto ret = data[key].as_array();
	std::vector<int> ret_vec;
	for (const auto& var : ret)
		ret_vec.emplace_back(var.as_number().to_int32());
	return ret_vec;
}

void SpecifyRefresher::operator()()
{
	//Once

	//Processor
	chck_close(src->specProcModel.set_text(
		get_api_str(L"/api/cpu/model", L"model").c_str())
	);
	chck_close(src->specProcCores.set_text(
		std::to_wstring(get_api_int(L"/api/cpu/cores", L"cores")).c_str())
	);

	//Physical Memory
	chck_close(src->specRamCapacity.set_text(
		get_api_SI(L"/api/mem/size", L"space_total").c_str())
	);

	//System
	chck_close(src->specOS.set_text(
		get_api_str(L"/api/system/os_info", L"os_info").c_str()));

	chck_close(src->specOSVersion.set_text(
		get_api_version(L"/api/system/os_version", L"os").c_str())
	);
	chck_close(src->specOSSPVersion.set_text(
		get_api_version(L"/api/system/os_version", L"sp").c_str())
	);

	//Disks
	//Little dangerous, can make program closing little bit longer if user close app instantly after machine change
	const std::vector<int> disks{ get_api_array_num(L"/api/disk/disks", L"disks") };
	int disk_idx = 0;
	if (!src->specify_run.test_and_set(std::memory_order_acquire))
	{
		src->specify_run.clear(std::memory_order_release);
		src->specDisks.clear();
		for (int i = 0; i < disks.size(); i++)
		{
			str w{ L"" };
			w += wchar_t(disks[i]);
			src->specDisks.addItem(w.c_str(), i + 1);
		}
	}
	else return;

	//Real-time
	while (!src->specify_run.test_and_set(std::memory_order_acquire))
	{
		src->specify_run.clear(std::memory_order_release);

		//Disks
		if (disk_idx != src->specDisks.getSelectedId())
		{
			disk_idx = src->specDisks.getSelectedId();
			if (disk_idx == 0 || disk_idx < 0 || disk_idx >= disks.size())
			{
				while (!src->specDiskFreeLck.test_and_set(std::memory_order_acquire)) {}
				src->specDiskFreeValue = get_api_real(L"/api/disk/space_total_free", L"space_total_free");
				src->specDiskFreeLck.clear(std::memory_order_release);
				chck_close(src->specDiskCapacity.set_text(
					get_api_SI(L"/api/disk/space_total", L"space_total").c_str()
				));
			}
			else
			{
				//Get total space
				if (!src->specify_run.test_and_set(std::memory_order_acquire))
				{
					src->specify_run.clear(std::memory_order_release);
					json::value data = json::value::object();
					str w{ L"" };
					w += wchar_t(disks[disk_idx - 1]);
					data[L"letter"] = json::value::string(w);
					try { POST_request(api_protocol + addr + L":" + api_port, L"/api/disk/space", data).wait(); }
					catch (...) { data = json::value::object(); data[L"space_total"] = json::value::number(0.0); data[L"SI"] = json::value::string(L"B"); }
					chck_close(src->specDiskCapacity.set_text(std::wstring(std::to_wstring(data[L"space_total"].as_double()) + L" " + data[L"SI"].as_string()).c_str()));
				}
				else return;

				//Get free space
				if (!src->specify_run.test_and_set(std::memory_order_acquire))
				{
					src->specify_run.clear(std::memory_order_release);
					json::value data = json::value::object();
					str w{ L"" };
					w += wchar_t(disks[disk_idx - 1]);
					data[L"letter"] = json::value::string(w);
					try { POST_request(api_protocol + addr + L":" + api_port, L"/api/disk/space_free", data).wait(); }
					catch (...) { data = json::value::object(); data[L"space_free"] = json::value::number(0.0); }
					while (!src->specDiskFreeLck.test_and_set(std::memory_order_acquire)) {}
					src->specDiskFreeValue = 1.0 - data[L"space_free"].as_double();
					src->specDiskFreeLck.clear(std::memory_order_release);
				}
				else return;

			}
		}

		//Processor
		if (!src->specify_run.test_and_set(std::memory_order_acquire))
		{
			src->specify_run.clear(std::memory_order_release);
			double ret = get_api_real(L"/api/cpu/usage", L"usage") / 100.0;
			while (!src->specProcessorUsageLck.test_and_set(std::memory_order_acquire)) {}
			src->specProcessorUsageValue = ret;
			src->specProcessorUsageLck.clear(std::memory_order_release);
		}
		else return;

		//RAM
		if (!src->specify_run.test_and_set(std::memory_order_acquire))
		{
			src->specify_run.clear(std::memory_order_release);
			double ret = get_api_real(L"/api/mem/used", L"used");
			while (!src->specRamUsageLck.test_and_set(std::memory_order_acquire)) {}
			src->specRamUsageValue = ret;
			src->specRamUsageLck.clear(std::memory_order_release);
		}
		else return;

		//Avaiable IP's
		chck_close(	src->specIP.set_text(
			get_api_array(L"/api/net", L"avaiable_ips").c_str())
		);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
