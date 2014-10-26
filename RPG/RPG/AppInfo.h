#pragma once

#ifndef _APPINFO_H_
#define _APPINFO_H_

namespace Solidus
{
	class AppInfo
	{
	public:
		AppInfo();
		int			LatestUpdateId;
		std::string LatestUpdateName;
		std::string CurrentVersion;
		std::string ManifestFile;		
	};
}

extern Solidus::AppInfo g_AppInfo;

#endif