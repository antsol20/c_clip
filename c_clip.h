#pragma once
#include <string>
#include <vector>

std::string imap_check(std::string const& username, std::string const& password);

void tokenize(std::string const& str, const char delim,
	std::vector<std::string>& out);

void clipCheckFile(std::string const& in_file, std::string const& out_file);
