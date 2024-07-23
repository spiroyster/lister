// > g++ lister.cpp -o lister 


#include <iostream>
#include <filesystem>
#include <list>
#include <functional>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <optional>

#include "xc.hpp"
#include "md5.hpp"

namespace Conditional
{
	enum Conditional
	{
		And,
		Or,
		Not,
		Duplicate
	};
}

//typedef std::unordered_map<std::string, std::filesystem::path> FileList;
typedef std::unordered_multimap<std::string, std::filesystem::path> FileList;
typedef std::function<void(const std::string& checksum, const std::filesystem::path& filepath)> EntryCallback;
typedef std::pair<Conditional::Conditional, int> Operation;

static unsigned int statsFiles = 0;
static unsigned int statsDirectories = 0;

std::string Coloured(const Conditional::Conditional& conditional, const std::string& text)
{
	switch (conditional)
	{
	case Conditional::And:
		return xc::yellow(text);
	case Conditional::Not:
		return xc::green(text);
	case Conditional::Or:
		return xc::cyan(text);
	case Conditional::Duplicate:
		return xc::magenta(text);		// Same as and with a reference list...
	default:
		return text;
	}
}

void info_message()
{
	//std::cout << xc::red("l") << xc::magenta("i") << xc::yellow("s") << xc::green("t") << xc::cyan("e") << xc::blue("r") << "\n\n";
	std::cout << "Usage > lister [outputfile] {[conditional] [filelist/rootpath]} \n\n";
	std::cout << "Builds a list to output to {outputfile} of the files specified in either the {filelist} or a new list derived from {rootpath} using preceeded conditional.\n";
	std::cout << "Is detemined by checksum (not filename). Command line can contain one or more conditionals and associated filepaths, but must contain output file as first argument.\n";
	std::cout << "\"" << Coloured(Conditional::And, "&") << "\" conditional appends to the list if already in list.\n";
	std::cout << "\"" << Coloured(Conditional::Not, "!") << "\" conditional appends to the list if not already in list.\n";
	std::cout << "\"" << Coloured(Conditional::Or, "or") << "\" conditional appends to the list irrelevant if there or not (overwrites in list with same checksum).\n\n";
	std::cout << "\"" << Coloured(Conditional::Duplicate, "%") << "\" conditional appends to the list irrelevant if there or not (overwrites in list with same checksum).\n\n";
	std::cout << "Output is in the format... \n";
	std::cout << " ; this is comment, entire line ignored\n";
	std::cout << "[checksum] dir1/dir2/file2\n";
	std::cout << "[checksum] dir1/file1 ; this is comment\n";
	std::cout << "[checksum] dir1/dir2/file1\n";
	std::cout << "[checksum] dir1/dir2/file2 ; this is a comment\n";
	std::cout << "[checksum] dir1/dir3/file1\n";
	std::cout << "[checksum] dir1/dir3/file2\n";
}

std::string Checksum(const std::filesystem::path& filepath)
{
	MD5 md5;
	return std::string(md5.digestFile(filepath.string().c_str()));
}

void IterateDirectory(const std::filesystem::path& current, EntryCallback callback)
{
	// Iterate the current path filesystem...
	for (auto const& entry : std::filesystem::directory_iterator{current}) 
	{
		auto path = entry.path();

		// If current is directory, recurssivly iterate that first...
		if (std::filesystem::is_directory(path))
			IterateDirectory(path, callback);

		// If current is a file, calculate checksum and callback entry (checksum and path)...
		else
			callback(Checksum(path), path);
	}

	// Increment the directory stats...
	++statsDirectories;
}

void ReadFileList(const std::filesystem::path& fileList, EntryCallback callback)
{
	// Open the file...
	std::ifstream file(fileList.string().c_str());

	std::string line;
	while(std::getline(file, line))
	{
		// Parse and ignore comment...
		auto commentItr = line.find(';');
		if (commentItr != std::string::npos)
			line = line.substr(0, commentItr);

		// Parse the checksum part...
		auto checksumBegin = line.find('[');
		auto checksumEnd = line.find(']', checksumBegin);
		
		// If the checksum syntax is valid...
		if (checksumBegin != std::string::npos && checksumEnd != std::string::npos && checksumEnd > checksumBegin)
		{
			std::string checksum = line.substr(checksumBegin + 1, checksumEnd - checksumBegin);
			std::string filepath = line.substr(checksumEnd + 2);

			// Remove quotations...
			if (filepath.front() == '\"')
				filepath = filepath.substr(1);
			if (filepath.back() == '\"')
				filepath.pop_back();
			
			callback(checksum, filepath);
		}
	}
}

int Count(FileList& filelist, const std::string& checksum)
{
	int result = 0;

	auto itr = filelist.find(checksum);
	if (itr != filelist.end())
		while (itr != filelist.end() && itr->first == checksum)
		{
			++result;
			++itr;
		}

	return result;
}

void Append(FileList& reference, FileList& working, const Operation& operation, const std::string& checksum, const std::filesystem::path& filepath)
{
	switch (operation.first)
	{
	case Conditional::And:
		{
			if (working.find(checksum) != working.end())
			{
				working.insert(std::make_pair(checksum, filepath));
				std::cout << "[" << Coloured(operation.first, checksum) << "] " << filepath << '\n';
				++statsFiles;
			}
				
			break;
		}
	case Conditional::Not:
		{
			if (working.find(checksum) == working.end())
			{
				working.insert(std::make_pair(checksum, filepath));
				std::cout << "[" << Coloured(operation.first, checksum) << "] " << filepath << '\n';
				++statsFiles;
			}

			break;
		}
	case Conditional::Duplicate:
		{
			// Check reference...
			if (Count(reference, checksum) > 1)
			{
				working.insert(std::make_pair(checksum, filepath));
				std::cout << "[" << Coloured(operation.first, checksum) << "] " << filepath << '\n';
				++statsFiles;
			}

			break;
		}
	case Conditional::Or:
		{
			// If not already present...
			bool duplicate = working.find(checksum) != working.end();

			// Add it...
			working.insert(std::make_pair(checksum, filepath));

			std::cout << "[" << Coloured(duplicate ? Conditional::And : Conditional::Not, checksum) << "] " << filepath << '\n';
			++statsFiles;

			break;
		}
	default:
		{
			// Add it...
			working.insert(std::make_pair(checksum, filepath));

			std::cout << "[" << Coloured(Conditional::Not, checksum) << "] " << filepath << '\n';
			++statsFiles;
			break;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		info_message();
		return 1;
	}

	std::list<std::pair<Operation, std::filesystem::path>> inFilepaths; // Comparisons...
	std::string outFilepath;	// The output file...

	std::optional<std::string> pre, post, destination;

	try
	{
		Conditional::Conditional currentConditional = Conditional::Or;
		bool setPre = false, setPost = false, setDestination = false;

		for (int arg = 1; arg < argc; ++arg)
		{
			std::string argument(*(argv+arg));
			int operationArg = 0;

			if (setPre)
			{
				pre = argument;
				setPre = false;
			}
			else if (setPost)
			{
				post = argument;
				setPost = false;
			}
			else if (setDestination)
			{
				destination = argument;
				setDestination = false;
			}

			else if (arg == 1)
				outFilepath = argument;
			else if (argument == "and")
				currentConditional = Conditional::And;
			else if (argument == "or")
				currentConditional = Conditional::Or;
			else if (argument == "!")
				currentConditional = Conditional::Not;
			else if (argument == "%")
			 	currentConditional = Conditional::Duplicate;
			else if (argument == "=")
				setDestination = true;
			else if (argument == "{")
			 	setPre = true;
			else if (argument == "}")
			 	setPost = true;
			else if (std::filesystem::exists(argument))
				inFilepaths.emplace_back(std::make_pair(std::make_pair(currentConditional, operationArg), argument));
			else
				throw std::runtime_error("Unknown argument \"" + argument + "\"");
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << xc::magenta(e.what());
		info_message();
		return 1;
	}

	// start overall timer...
	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

	// build a working list or records...
	FileList working, reference;
	try
	{
		for (auto itr = inFilepaths.begin(); itr != inFilepaths.end(); ++itr)
		{
			if (pre || post)
			{
				if (!std::filesystem::is_directory(itr->second))
					ReadFileList(itr->second, [&itr, &working](const std::string& checksum, const std::filesystem::path& filepath)
					{
						working.insert(std::make_pair(checksum, filepath));
					});
				// Otherwise if 'root' path...
				else
					IterateDirectory(itr->second, [&itr, &working](const std::string& checksum, const std::filesystem::path& filepath)
					{
						working.insert(std::make_pair(checksum, filepath));
					});

				continue;
			}

			// IF needed, populate the reference flielist with current state...
			if (itr->first.first == Conditional::Duplicate)
			{
				if (!std::filesystem::is_directory(itr->second))
					ReadFileList(itr->second, [&itr, &reference](const std::string& checksum, const std::filesystem::path& filepath)
					{
						reference.insert(std::make_pair(checksum, filepath));
					});
				// Otherwise if 'root' path...
				else
					IterateDirectory(itr->second, [&itr, &reference](const std::string& checksum, const std::filesystem::path& filepath)
					{
						reference.insert(std::make_pair(checksum, filepath));
					});
			}
			
			if (!std::filesystem::is_directory(itr->second))
				ReadFileList(itr->second, [&itr, &reference, &working](const std::string& checksum, const std::filesystem::path& filepath)
				{
					Append(reference, working, itr->first, checksum, filepath);
				});
			// Otherwise if 'root' path...
			else
				IterateDirectory(itr->second, [&itr, &reference, &working](const std::string& checksum, const std::filesystem::path& filepath)
				{
					Append(reference, working, itr->first, checksum, filepath);
				});	
		}

		std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();

		// Output the summary...
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		std::cout << xc::green(std::to_string(statsFiles)) << " files\n";
		std::cout << xc::yellow(std::to_string(statsDirectories)) << " directories\n";
		std::cout << " took " << xc::cyan(std::to_string(duration)) << " ms.\n";

		// Write to output file...
		if (!outFilepath.empty())
		{
			std::cout << "Writing " << outFilepath << "...\n";
			std::ofstream file(outFilepath.c_str());

			if (pre || post)
			{
				for (auto itr = working.begin(); itr != working.end(); ++itr)
					file << (pre ? *pre : "") << itr->second << (post ? *post : "") << '\n';	
			}
			else if (destination)
			{
				for (auto itr = working.begin(); itr != working.end(); ++itr)
					file << "cp -r " << itr->second << " " << *destination << "/" << itr->second << '\n';	
			}
			else
			{
				for (auto itr = working.begin(); itr != working.end(); ++itr)
					file << "[" << itr->first << "] " << itr->second << '\n';	
				file << ";" << std::to_string(statsFiles) << " files\n;" << std::to_string(statsDirectories) << " directories\n";
			}
		}

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << xc::red(e.what());
	}
	catch (...)
	{
		std::cerr << xc::red("Unknown error.");
	}
	return 1;
}
