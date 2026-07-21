#include "Utility.h"


void print_time(const stdc::nanoseconds& time, std::ostream& out)
{
	double value = to<double>(time.count());
	const char* unit = "ns";

	if (time >= 1s) {
		value /= 1'000'000'000.0;
		unit = "s";
	} else if (time >= 1ms) {
		value /= 1'000'000.0;
		unit = "ms";
	} else if (time >= 1us) {
		value /= 1'000.0;
		unit = "us";
	}

#ifdef HAS_CPP20
	out << std::format("{:.2f}{}", value, unit);
#else
	std::ios oldState(nullptr);
	oldState.copyfmt(out);

	out << std::fixed << std::setprecision(2) << value << unit;

	out.copyfmt(oldState);
#endif
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
	else
		std::system(("explorer /select,\""s + path.string() + "\"").c_str());
#elif defined(__APPLE__)
	std::system((std::string("open \"") + path.string() + "\"").c_str());
#elif defined(__linux__)
	std::system((std::string("xdg-open \"") + path.string() + "\"").c_str());
#else
	std::cerr << "Unsupported platform.\n";
#endif
}
#endif


#ifdef HAS_CPP20
namespace Parser
{
	const std::function<bool(const ParsedMap&)> KeySpec::OPTIONAL_VALUE  = [](const ParsedMap&) { return false; };
	const std::function<bool(const ParsedMap&)> KeySpec::MANDATORY_VALUE = [](const ParsedMap&) { return true ; };

	static std::string Trim(const std::string& s)
	{
		const auto first = s.find_first_not_of(" \t\n\r\f\v");
		if (first == std::string::npos)
			return "";

		const auto last = s.find_last_not_of(" \t\n\r\f\v");
		return s.substr(first, last - first + 1);
	}
	static bool LooksLikeInt(std::string_view s)
	{
		if (s.empty()) return false;
		if (s.front() == '+') 
			s.remove_prefix(1);
		if (s.empty()) return false;

		int value;
		auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

		return ec == std::errc{} && ptr == s.data() + s.size();
	}
	static bool LooksLikeDouble(std::string_view s)
	{
		if (s.empty()) return false;
		if (s.front() == '+')
			s.remove_prefix(1);
		if (s.empty()) return false;

		errno = 0;

		char* end;
		std::string str(s); // strtod requires a null-terminated string
		std::strtod(str.c_str(), &end);

		return errno != ERANGE && *end == '\0';
	}
	static bool LooksLikeArray(const std::string& s)
	{
		return s.starts_with('[') && s.ends_with(']');
	}

	static ParsedValue ParseValue(std::string& s);
	static std::vector<ParsedValue> ParseArray(std::string& s)
	{
		std::vector<ParsedValue> result;
		s = s.substr(1, s.size() - 2);

		size_t pos;
		size_t prev = 0;
		while ((pos = s.find(',', prev)) != std::string::npos)
		{
			std::string el = Trim(s.substr(prev, pos - prev));
			result.push_back(ParseValue(el));
			prev = pos + 1;
		}
		std::string el = Trim(s.substr(prev));
		result.push_back(ParseValue(el));

		return result;
	}
	static ParsedValue ParseValue(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

		if (s == "true" || s == "false")
			return s == "true";

		if (LooksLikeInt(s))
			return std::stoi(s);
		if (LooksLikeDouble(s))
			return std::stod(s);
		if (LooksLikeArray(s))
			return ParseArray(s);

		// string
		if (s.starts_with('"') && s.ends_with('"'))
			return s.substr(1, s.size() - 2);
		return s;
	}
	static ParsedMap Read(std::ifstream& file)
	{
		std::string line;
		size_t lineNo = 0;
		ParsedMap mapping = {};

		while (std::getline(file, line)) {
			lineNo++;
			// trim, skip comments, parse
			if (line.starts_with('#'))
				continue;

			std::string key, value;

			auto pos = line.find(':');
			if (pos == std::string::npos)
			{
				key = line;
				value = "true";
			}
			else
			{
				key = Trim(line.substr(0, pos));
				value = Trim(line.substr(pos + 1));
			}

			ParsedValue parsed = ParseValue(value);
			mapping[key] = parsed;
		}

		return mapping;
	}
	
	static std::optional<std::string> Validate(const ParsedMap& config, const Schema& schema)
	{
		for (const auto& [key, spec] : schema) {
			if (spec.required(config) && !config.contains(key)) {
				return "Missing required key: " + key;
			}

			if (config.contains(key) && spec.validator) {
				std::string reason = "";
				if (!spec.validator(config.at(key), config, reason)) {
					std::string error = "Invalid value for key: " + key;
					if (reason != "")
						error += "; reason: " + reason;
					return error;
				}
			}
   		}
		return std::nullopt;
	}
	ParseResult Parse(const fs::path& path, const Schema& schema)
	{
		std::ifstream file(path);
		
		ParseResult res;
		res.data = Read(file);
		res.error = Validate(res.data, schema);
		return res;
	}
}
#endif



#ifdef HAS_CPP20
std::unique_ptr<Loader> Loader::s_Instance = nullptr;
#endif