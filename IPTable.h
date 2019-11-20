#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include <fstream>
#include <future>

#include <cpprest/http_client.h>

#include "BasicRefresher.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;

using str = std::wstring;

class TableDemoComponent : public Component,
	public TableListBoxModel
{
public:

	TableDemoComponent(json::value& data)
	{
		loadData(data);
		addAndMakeVisible(table);
		table.setModel(this);
		table.setColour(ListBox::outlineColourId, Colours::grey);
		table.setOutlineThickness(1);
		table.getHeader().addColumn(L"Hostname", 1, 300, 50, 400, TableHeaderComponent::defaultFlags);
		table.getHeader().setStretchToFitActive (true);
		table.setMultipleSelectionEnabled(false);
	}

	int getNumRows() override
	{
		return data.size();
	}

	void paintRowBackground(Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
	{
		auto alternateColour = getLookAndFeel().findColour(ListBox::backgroundColourId)
			.interpolatedWith(getLookAndFeel().findColour(ListBox::textColourId), 0.03f);
		if (rowIsSelected)
			g.fillAll(Colours::lightblue);
		else if (rowNumber % 2)
			g.fillAll(alternateColour);
	}

	void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/) override
	{
		g.setColour(getLookAndFeel().findColour(ListBox::textColourId));
		g.setFont(font);

		if (rowNumber < data.size())
		{
			std::wstring text = data[rowNumber];
			g.drawText(text.c_str(), 2, 0, width - 4, height, Justification::centredLeft, true);
		}

		g.setColour(getLookAndFeel().findColour(ListBox::backgroundColourId));
		g.fillRect(width - 1, 0, 1, height);
	}

	String getText(const int columnNumber, const int rowNumber) const
	{
		return data[rowNumber].to_wstring().c_str();
	}
	
	struct hostname
	{
		std::wstring name;
		std::wstring addres;
		hostname(const std::wstring& i_name, const std::wstring& i_addr)
			:name{ i_name }, addres{ i_addr } {}
		hostname(const hostname&) = default;
		hostname(hostname&&) = default;
		hostname() = default;
		hostname& operator=(const hostname&) = default;
		hostname& operator=(hostname&&) = default;

		std::wstring to_wstring() const
		{
			return name + L" [" + addres + L"]";
		}

		operator std::wstring()
		{
			return to_wstring();
		}
	};

	virtual void selectedRowsChanged(int last_row) override
	{
		on_selected_row_changed(last_row);
	}

	TableListBox table;     // the table component itself
	std::function<void(int)> on_selected_row_changed = [](int) {};

	hostname operator[](const int row) { if (row >= data.size() || row < 0) return hostname(L"", L""); else return data[row]; }
	std::vector<hostname> data;

	bool add(const hostname& src)
	{
		if (!online(api_protocol + src.addres + L":" + api_port)) return false;
		data.emplace_back(src);
		return true;
	}

private:

	Font font{ 14.0f };
	
	void loadData(json::value& dat)
	{
		json::array arr = dat[L"hostnames"].as_array();
		data.reserve(arr.size());
		for(auto& tmp : arr)
		{
			const auto add = tmp[L"address"].as_string();
			if (!online(api_protocol + add + L":" + api_port)) continue;
			else data.emplace_back(hostname(tmp[L"name"].as_string(), tmp[L"address"].as_string()));
		}
	}

#include <thread>

	bool online(const std::wstring& host)
	{
		std::shared_ptr<int> ret{ new int(0) };
		std::thread th{ [](auto ret, const std::wstring host) {
			try
			{
				*ret = (http_client(host).request(methods::GET, L"/api").get().status_code() == status_codes::OK ? 10 : -10);
			}
			catch (const http_exception & e)
			{
				*ret = -11;
			}

		}, ret, host};
		
		std::this_thread::sleep_for(std::chrono::time_point<std::chrono::system_clock>::duration(std::chrono::seconds(1)));

		if (*ret == 10)
		{
			th.join();
			return true;
		}
		else
		{
			th.detach();
			return false;
		}
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TableDemoComponent)
};
