#include "Utility.h"


std::ostream& print_time(const stdc::nanoseconds& time, std::ostream& out)
{
#ifdef HAS_CPP20
	if (time < 1us)
		out << time;
	else if (time < 1ms)
		out << to<stdc::microseconds>(time);
	else if (time < 1s)
		out << to<stdc::milliseconds>(time);
	else
		out << to<stdc::seconds>(time);
#else
	if (time < 1us)
		out << time.count() << "ns";
	else if (time < 1ms)
		out << to<stdc::microseconds>(time).count() << "us";
	else if (time < 1s)
		out << to<stdc::milliseconds>(time).count() << "ms";
	else
		out << to<stdc::seconds>(time).count() << 's';
#endif
	return out;
}


#ifdef HAS_CPP17
std::string ReadFile(const fs::path& file)
{
	std::ifstream in(file);
	std::string content = std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	in.close();
	return content;
}

void ShowInExplorer(const fs::path& path)
{
#if defined(_WIN32)
	if (std::filesystem::is_directory(path))
		std::system(("explorer \""s + path.string() + "\"").c_str());
	else {
		std::system(("explorer /select,\""s + path.string() + "\"").c_str());
	}
#elif defined(__APPLE__)
	std::system((std::string("open \"") + path.string() + "\"").c_str());
#elif defined(__linux__)
	std::system((std::string("xdg-open \"") + path.string() + "\"").c_str());
#else
	std::cerr << "Unsupported platform.\n";
#endif
}
#endif