#pragma once
#include <unordered_set>
#include "Rpc.h"

class BanList {
	std::unordered_set<std::string> m_bannedNames;
	std::unordered_set<std::string> m_bannedIps;

public:
	BanList() {}

	void BanIP(std::string ip) {
		
		m_bannedIps.insert(ip);
	}

	void BanName(std::string name) {
		m_bannedNames.insert(name);
	}

	void UnBanIP(std::string ip) {
		m_bannedIps.erase(ip);
	}

	void UnBanName(std::string name) {
		m_bannedNames.erase(name);
	}

	bool IsIPBanned(std::string ip) {
		return m_bannedIps.contains(ip);
	}

	bool IsNameBanned(std::string name) {
		return m_bannedNames.contains(name);
	}

	bool checkBan() {

	}
};

