#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

struct Command
{
	Command(
		std::string_view cmd,
		std::function<void(Command* cmd)> execute = nullptr);

	void ClearValues();

	std::string _cmd;
	std::optional<std::vector<std::string>> _values;

	std::function<void(Command*)> _execute;
};

enum class ExecutionResult : uint8_t
{
	SUCCESS,
	UNRECOGNIZED_COMMAND
};

class CommandLineExecutor
{
public:
	using CommandVector = std::vector<std::unique_ptr<Command>>;

	CommandLineExecutor();

	void RegisterCommand(std::unique_ptr<Command> cmd);
	void SetCursor(std::string_view cursor);
	ExecutionResult Execute();

	static std::vector<std::string> ParseCommandLine(std::string_view command_line);

private:
	CommandVector _commands;
	std::string _cursor;
};

