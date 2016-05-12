////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CommandLineParser.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <sstream>

namespace utilities
{
    // format of argv: Main.exe [options]
    // where options are of the form "-<std::string> <option>" where the <option> part is mandatory (defaulting to 'true')
    // options have two names, the short name is used with a single hyphen, and the long name with two
    // e.g., "-s true" and "--serial_mode true" can mean the same thing
    // options are queried by the long name
    // short name is optional
    // args are just strings at the end
    // example of valid commandlines:
    // myexe.exe file1.tsv
    // myexe.exe file1.tsv file2.tsv
    // myexe.exe -t 8 -x someString file1.tsv file2.tsv
    template <typename T, typename U>
    void CommandLineParser::AddOption(T& option, std::string name, std::string shortName, std::string description, const U& defaultValue)
    {
        auto callback = [&option, this](std::string optionVal)
        {
            bool didParse = ParseVal<T>(optionVal, option);
            return didParse;
        };

        OptionInfo info(name, shortName, description, ToString(defaultValue), callback);
        AddOption(info);
    }

    template <typename T>
    void CommandLineParser::AddOption(T& option, std::string name, std::string shortName, std::string description, std::initializer_list<std::pair<std::string, T>> enumValues, std::string defaultValue)
    {
		// transform initializer list into useful things that will stick around
		std::vector<std::string> valueNameStrings;
		std::vector<std::pair<std::string, T>> valueNamesTable;
		for (auto v : enumValues)
		{
			valueNameStrings.push_back(v.first);
			valueNamesTable.push_back(v);
		}

		auto callback = [&option, this, name, valueNamesTable](std::string optionVal)
		{
			std::string optionString;
			bool didParse = ParseVal<T>(optionVal, valueNamesTable, option, optionString);
			if (didParse)
			{
				_options[name].currentValueString = optionString;
				return true;
			}
			else
			{
				std::string errorMessage = "Could not parse value for option " + name;
				throw std::runtime_error(errorMessage);
				return false;
			}
		};

		OptionInfo info(name, shortName, description, defaultValue, callback);
		info.enumValues = valueNameStrings;
		AddOption(info);
	}
    
    template <typename T>
    bool CommandLineParser::ParseVal(std::string str, T& result)
    {
        std::stringstream ss(str);
        ss >> result;
        return true;
    }

    template <typename T>
    bool CommandLineParser::ParseVal(std::string str, std::vector<std::pair<std::string, T>> valNames, T& result, std::string& resultString)
    {
        bool didFindOne = false;
        for (const auto& valNamePair : valNames)
        {
            if (valNamePair.first.find(str) == 0)
            {
                if (didFindOne)
                {
                    return false;
                }

                resultString = valNamePair.first;
                result = valNamePair.second;
                didFindOne = true;
            }
        }

        return didFindOne;
    }

    template <typename T>
    std::string CommandLineParser::ToString(const T& val)
    {
        std::stringstream ss;
        ss << val;
        return ss.str();
    }

    // bool specialization
    template <>
    inline bool CommandLineParser::ParseVal<bool>(std::string val, bool& result)
    {
        result = (val[0] == 't');
        return true;
    }

    template<>
    inline std::string CommandLineParser::ToString<bool>(const bool& val)
    {
        return val ? "true" : "false";
    }
}
