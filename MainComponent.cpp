/*
  ==============================================================================

	This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "SpecifyRefresher.h"
#include "GlobalRefresher.h"
#include "AddHostnameDialog.h"

#include <future>
#include <chrono>
#include <ctime>
#include <Windows.h>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>


//==============================================================================
MainComponent::MainComponent()
{
	setSize(window_width, window_height);

	//Global Labels
	add_std_label(L"Średnie Obciążenie Procesora", margin_left + 0, margin_top + 0);
	add_std_label(L"Średnie Użycie Pamięci", margin_left + 0, margin_top + 25);
	add_std_label(L"Średnie Użycie Dysków", margin_left + 0, margin_top + 50);

	//Global Progress Bars
	globalProcessor.setBounds(margin_left + width(labels[0]) + 20, margin_top + Y(labels[0]) - 5, window_width - width(labels[0]) - default_height - 10 - margin_left, default_height);
	globalRAM.setBounds(margin_left + width(labels[1]) + 20, margin_top + Y(labels[1]) - 5, window_width - width(labels[1]) - default_height - 10 - margin_left, default_height);
	globalDisk.setBounds(margin_left + width(labels[2]) + 20, margin_top + Y(labels[2]) - 5, window_width - width(labels[2]) - default_height - 10 - margin_left, default_height);
	addAndMakeVisible(globalProcessor);
	addAndMakeVisible(globalRAM);
	addAndMakeVisible(globalDisk);

	//Table config
	tbl.table.setBounds(
		margin_left,
		margin_top + Y(globalDisk) + height(globalDisk) + margin_top,
		0.2 * window_width,
		window_height - Y(globalDisk) - ( 4 * margin_top ) - ( 2 * default_height )
	);
	tbl.on_selected_row_changed = [&](int row) { if(row >=0 ) update_specific(tbl[row].addres); };
	addAndMakeVisible(tbl.table);

	//Setting up global refresher
	for (int i = 0; i < tbl.getNumRows(); i++)
		addresses.push_back(tbl[i].addres);
	if (tbl.data.size() > 0) global_refresher.reset(new std::thread(GlobalRefresher(this)));

	const int alt_left_margin = margin_left + X(tbl.table) + width(tbl.table);
	const int alt_top_margin = margin_top + Y(tbl.table);

	//Specify Labels
	const int s_model_proc = add_std_label(L"Model Procesora:", alt_left_margin, alt_top_margin);
	const int s_cores = add_std_label(L"Ilość Rdzenii Procesora:", alt_left_margin, alt_top_margin + 25);
	const int s_processor_usage = add_std_label(L"Obciążenie Procesora:", alt_left_margin, alt_top_margin + 50);
	const int s_ram_capacity = add_std_label(L"Całkowita Pojemność Pamięci Fizycznej:", alt_left_margin, alt_top_margin + 75);
	const int s_ram_usage = add_std_label(L"Użycie Pamięci Fizycznej:", alt_left_margin, alt_top_margin + 100);
	const int s_os = add_std_label(L"System Operacyjny:", alt_left_margin, alt_top_margin + 125);
	const int s_os_ver = add_std_label(L"Wersja Systemu Operacyjnego:", alt_left_margin, alt_top_margin + 150);
	const int s_sp_ver = add_std_label(L"Wersja Service Pack'a Systemu Operacyjnego", alt_left_margin, alt_top_margin + 175);
	const int s_ip = add_std_label(L"Dostępne Adresy IP:", alt_left_margin, alt_top_margin + 200);
	const int s_choose_disk = add_std_label(L"Wybierz Dysk:", alt_left_margin, alt_top_margin + 225);
	const int s_disk_capacity = add_std_label(L"Pojemność Dysku:", alt_left_margin, alt_top_margin + 250);
	const int s_disk_free = add_std_label(L"Zajęta Przestrzeń:", alt_left_margin, alt_top_margin + 275);

	//Specify TextEditors
	configure_txt_editor(specProcModel, s_model_proc, L"Intel Core i5");
	configure_txt_editor(specProcCores, s_cores, L"8");
	configure_txt_editor(specRamCapacity, s_ram_capacity, L"12 G");
	configure_txt_editor(specOS, s_os, L"Windows 10");
	configure_txt_editor(specOSVersion, s_os_ver, L"1900.12");
	configure_txt_editor(specOSSPVersion, s_sp_ver, L"2.1");
	configure_txt_editor(specIP, s_ip, L"127.0.0.1; 192.168.56.1");
	configure_txt_editor(specDiskCapacity, s_disk_capacity, L"1.0 T");

	specProcessorUsage.setBounds(X(labels[s_processor_usage]) + margin_left + width(labels[s_processor_usage]),
		Y(labels[s_processor_usage]),
		window_width - X(labels[s_processor_usage]) - width(labels[s_processor_usage]) - (2 * margin_left),
		default_height);
	addAndMakeVisible(specProcessorUsage);

	specRamUsage.setBounds(X(labels[s_ram_usage]) + margin_left + width(labels[s_ram_usage]),
		Y(labels[s_ram_usage]),
		window_width - X(labels[s_ram_usage]) - width(labels[s_ram_usage]) - (2 * margin_left),
		default_height);
	addAndMakeVisible(specRamUsage);

	specDiskFree.setBounds(X(labels[s_disk_free]) + margin_left + width(labels[s_disk_free]),
		Y(labels[s_disk_free]),
		window_width - X(labels[s_disk_free]) - width(labels[s_disk_free]) - (2 * margin_left),
		default_height);
	addAndMakeVisible(specDiskFree);
	
	specDisks.setBounds(
		X(labels[s_choose_disk]) + margin_left + width(labels[s_choose_disk]),
		Y(labels[s_choose_disk]),
		window_width - X(labels[s_choose_disk]) - width(labels[s_choose_disk]) - (2 * margin_left),
		default_height
	);
	addAndMakeVisible(specDisks);

	//Address Buttons
	rem_address.onClick = [&]()
	{
		int selected = tbl.table.getSelectedRow();
		if (selected == -1) return;
		auto it = tbl.data.begin();
		for (int i = 0; i < selected; i++)
			it++;
		tbl.data.erase(it);

		system(R"(ERASE C:\Users\raidg\Documents\hosts.json)");
		json::value serial = json::value::object();
		std::vector<json::value> vec;
		vec.reserve(tbl.data.size());
		for (const TableDemoComponent::hostname& var : tbl.data)
		{
			json::value tmp = json::value::object();
			tmp[L"name"] = json::value::string(var.name);
			tmp[L"address"] = json::value::string(var.addres);
			vec.push_back(tmp);
		}
		serial[L"hostnames"] = json::value::array(vec);

		std::wofstream out{ R"(C:\Users\raidg\Documents\hosts.json)" };
		out << serial.serialize();
		out.close();

		if (global_refresher.get() != nullptr)
		{
			while (!global_run.try_lock()) {}
			global_refresher->join();
			global_run.unlock();
			global_refresher.reset(nullptr);
		}
		if (specify_refresher.get() != nullptr)
		{
			while (!specify_run.try_lock()) {}
			specify_refresher->join();
			specify_run.unlock();
			specify_refresher.reset(nullptr);
		}

		if (tbl.data.size() > 0) global_refresher.reset(new std::thread(GlobalRefresher(this)));
		tbl.table.selectRow(-1);
		tbl.table.updateContent();
	};
	rem_address.setBounds(X(tbl.table), Y(tbl.table) + height(tbl.table) + margin_top, ( width(tbl.table) / 2 ) * 0.8, default_height);
	rem_address.setButtonText(L"Usuń");
	addAndMakeVisible(rem_address);

	add_address.onClick = [&]()
	{
		AddHostnameDialog dial;
		if (dial.add == L"" || dial.hn == L"") return;

		tbl.add({ dial.hn, dial.add });
		
		system(R"(ERASE C:\Users\raidg\Documents\hosts.json)");
		json::value serial = json::value::object();
		std::vector<json::value> vec;
		vec.reserve(tbl.data.size());
		for (const TableDemoComponent::hostname& var : tbl.data)
		{
			json::value tmp = json::value::object();
			tmp[L"name"] = json::value::string(var.name);
			tmp[L"address"] = json::value::string(var.addres);
			vec.push_back(tmp);
		}
		serial[L"hostnames"] = json::value::array(vec);

		std::wofstream out{ R"(C:\Users\raidg\Documents\hosts.json)" };
		out << serial.serialize();
		out.close();

		tbl.table.updateContent();

		if (global_refresher.get() != nullptr)
		{
			while (!global_run.try_lock()) {}
			global_refresher->join();
			global_run.unlock();
			global_refresher.reset(nullptr);
		}
		if (specify_refresher.get() != nullptr)
		{
			while (!specify_run.try_lock()) {}
			specify_refresher->join();
			specify_run.unlock();
			specify_refresher.reset(nullptr);
		}

		global_refresher.reset(new std::thread(GlobalRefresher(this)));
		tbl.table.selectRow(-1);
		tbl.table.updateContent();

	};
	add_address.setBounds(X(rem_address) + width(rem_address) + 5, Y(tbl.table) + height(tbl.table) + margin_top, width(rem_address), default_height);
	add_address.setButtonText(L"Dodaj");
	addAndMakeVisible(add_address);

	sync_files.onClick = [&]() {

		std::string s(255, '\0');
		std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::strftime(&s[0], s.size(), "%Y_%m_%d", std::localtime(&now));
		std::wstring w;
		w.reserve(s.size());
		for (const auto var : s)
			w += static_cast<wchar_t>(var);

		std::string x = R"(C:\Users\raidg\Downloads\Pobrane_Raporty\)";
		x += s;
		LPCSTR t = x.c_str();
		CreateDirectoryA(t, NULL);

		for (const auto& var : tbl.data)
			std::async(std::launch::async, [&](const TableDemoComponent::hostname& address, const std::wstring& dirname)
				{
					client::http_client client{ L"http://" + address.addres + L":9000/file/raport" };
					http_response res;
					try { res = client.request(methods::GET).get(); }
					catch (const http_exception& e)
					{
						throw;
					}
					Concurrency::streams::ostream fileStream;
					std::wstringstream path_to_create;
					path_to_create << dirname << L"\\" << address.name << L".xlsx";
					auto t = path_to_create.str();
					std::wstring path;
					for (const auto& var : t)
						if (var != L'\0') path += var;
					try { fileStream = Concurrency::streams::fstream::open_ostream(path).get(); }
					catch (const std::exception & e)
					{
						throw;
					}
					int ret = 1;
					while (ret > 0)
						try { ret = res.body().read_to_end(fileStream.streambuf()).get(); }
					catch (const std::exception & e)
					{
						throw;
					}
					fileStream.close();
			}, var, LR"(C:\Users\raidg\Downloads\Pobrane_Raporty\)" + w);
	};
	sync_files.setBounds(
		X(add_address) + width(add_address) + 5,
		Y(tbl.table) + height(tbl.table) + margin_top,
		width(tbl.table) - (width(add_address) + width(rem_address) + 10),
		default_height);
	sync_files.setButtonText(L"O");
	addAndMakeVisible(sync_files);
}

MainComponent::~MainComponent()
{
	if (global_refresher.get() != nullptr)
	{
		while (!global_run.try_lock()) {}
		global_refresher->join();
		global_run.unlock();
		global_refresher.reset(nullptr);
	}
	if(specify_refresher.get() != nullptr)
	{
		while (!specify_run.try_lock()) {}
		specify_refresher->join();
		specify_run.unlock();
		specify_refresher.reset(nullptr);
	}
}

int MainComponent::add_std_label(const std::wstring& caption, const int pos_x, const int pos_y, int width, int height)
{	
	assert(amount_of_labels < max_amount_of_labels);
	Label& tmp = labels[amount_of_labels];
	tmp.setText(String(caption.c_str()), NotificationType::dontSendNotification);
	tmp.setFont(std_font);
	tmp.setBounds(pos_x, pos_y, width == -1 ? caption.length() * 7.5 : width, height == -1 ? default_height : height);
	tmp.setJustificationType(Justification::centredLeft);
	addAndMakeVisible(tmp);
	amount_of_labels++;
	return amount_of_labels - 1;
}

void MainComponent::configure_txt_editor(TextEditor& src, const int idx, const std::wstring& txt)
{
	src.setBounds(
		X(labels[idx]) + margin_left + width(labels[idx]),
		Y(labels[idx]),
		window_width - X(labels[idx]) - width(labels[idx]) - (2 * margin_left),
		default_height
	);
	src.setReadOnly(true);
	//src.setText(txt.c_str());
	src.setFont(std_font);
	src.setJustification(Justification::centred);
	addAndMakeVisible(src);
}

void MainComponent::update_specific(const std::wstring& src)
{
	if (specify_refresher.get() != nullptr)
	{
		while (!specify_run.try_lock()) {}
		specify_refresher->join();
		specify_run.unlock();
		specify_refresher.reset(nullptr);
	}

	if (src == L"") return;

	specify_refresher.reset(new std::thread(SpecifyRefresher(this, src)));
}

//==============================================================================