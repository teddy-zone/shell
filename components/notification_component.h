#pragma once

#include <deque>

#include "component.h"

struct CompNotification : public Component
{
	std::deque<std::pair<std::string, float>> notifications;

	void add_notification(const std::string& in_string, float in_time)
	{
		notifications.push_back(std::make_pair(in_string, in_time));
	}
};
