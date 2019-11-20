#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class AddHostnameDialog : public DialogWindow
{

	std::unique_ptr<DialogWindow> p{ nullptr };

	std::unique_ptr<Label> l1;
	std::unique_ptr<Label> l2;
	std::unique_ptr<TextEditor> hostname;
	std::unique_ptr<TextEditor> address;
	
public:
	
	std::wstring hn{ L"" }, add{ L"" };

	AddHostnameDialog()
		:DialogWindow(L"Podaj parametry nowej masyny", juce::Colour::fromRGBA(100, 100, 100, 255), true)
	{
		l1.reset(new Label());
		l2.reset(new Label());
		hostname.reset(new TextEditor());
		address.reset(new TextEditor());

		l1->setText(L"Nazwa Hosta:", NotificationType::dontSendNotification);
		l1->setBounds(10, 10, 100, 25);
		l1->setFont(Font("Consolas", 15.0, Font::FontStyleFlags::plain));
		l1->setColour(Label::ColourIds::textColourId, Colour::fromRGBA(0, 0, 0, 255));
		hostname->setBounds(125, 10, 110, 25);

		l2->setText(L"Adres IP:", NotificationType::dontSendNotification);
		l2->setBounds(10, 50, 100, 25);
		l2->setFont(Font("Consolas", 15.0, Font::FontStyleFlags::plain));
		l2->setColour(Label::ColourIds::textColourId, Colour::fromRGBA(0, 0, 0, 255));
		address->setBounds(125, 50, 110, 25);

		GroupComponent grid;
		grid.setColour(GroupComponent::ColourIds::outlineColourId, juce::Colour::fromRGBA(255, 255, 255, 0));

		setContentComponent(&grid);
		setSize(250, 50);
		setVisible(true);

		grid.addAndMakeVisible(l1.get());
		grid.addAndMakeVisible(l2.get());
		grid.addAndMakeVisible(hostname.get());
		grid.addAndMakeVisible(address.get());

		runModalLoop();
	}

	void closeButtonPressed() override
	{

		const auto s1 = hostname->getText().toStdString();
		const auto s2 = address->getText().toStdString();

		for (const char var : s1)
			hn += var;

		for (const char var : s2)
			add += var;

		exitModalState(0);
	}

};