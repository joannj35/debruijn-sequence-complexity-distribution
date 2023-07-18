#include "CommandLineExecutor.h"

#include <iostream>
#include <algorithm>
#include <sstream>

Command::Command(
	std::string_view cmd,
	std::function<void(Command* cmd)> execute) :
	_cmd{ "-" },
	_execute{ execute },
	_values{ std::nullopt }
{
	_cmd.append(cmd.data());
}

void Command::ClearValues()
{
	if (_values)
		_values.value().clear();
}

CommandLineExecutor::CommandLineExecutor() : _cursor{ ">>" } { }

void CommandLineExecutor::RegisterCommand(std::unique_ptr<Command> cmd)
{
	_commands.emplace_back(std::move(cmd));
}

void CommandLineExecutor::SetCursor(std::string_view cursor)
{
	_cursor = cursor.data();
}

ExecutionResult CommandLineExecutor::Execute()
{
	std::cout << _cursor;

	std::string command_line{ "" };
	std::getline(std::cin, command_line);

	auto parsed_commands = ParseCommandLine(command_line);

	for (auto it = parsed_commands.begin(); it != parsed_commands.end(); it++)
	{
		auto res = std::find_if(_commands.begin(), _commands.end(), [&it](const auto& cmd) { return cmd->_cmd == *it; });
		if (res == _commands.end())
		{
			std::cout << "Command " + *it + " cannot be recognized." << std::endl;
			return ExecutionResult::UNRECOGNIZED_COMMAND;
		}

		// Command is found
		Command* cmd = res->get();

		// Clear the previous values from the command
		cmd->ClearValues();

		// Check if there are arguments attached to the command
		while (std::next(it) != parsed_commands.end() && !std::next(it)->starts_with("-"))
		{
			if (!cmd->_values)
				cmd->_values = std::make_optional<std::vector<std::string>>({});
			it++;
			cmd->_values.value().push_back(*it);
		}

		// Execute the command
		if (cmd->_execute)
			cmd->_execute(cmd);
	}

	return ExecutionResult::SUCCESS;
}

std::vector<std::string> CommandLineExecutor::ParseCommandLine(std::string_view command_line)
{
	std::vector<std::string> parsed_commands;
	std::stringstream ss{ command_line.data() };
	std::string cmd{ "" };
	while (ss >> cmd) { parsed_commands.push_back(cmd); }
	return parsed_commands;
}
