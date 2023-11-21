#include <iostream>
#include <cctype>
#include <string>
#include <string_view>
#include <sstream>
#include <algorithm>
#include <functional>

#include "DebruijnGraph.h"
#include "CommandLineExecutor.h"

namespace command_executors
{
	void command_error(std::string_view cmd)
	{
		std::cout << cmd << " command error, use -help for usage." << std::endl;
	}

	void print_header()
	{
		std::cout << "Computes De Bruijn sequences for the given alphabet and order." << std::endl;
	}

	void print_about(Command* cmd = nullptr)
	{
		std::cout << "About: " << std::endl;
		std::cout << "\t";
		print_header();
		std::cout << "\tVersion 1.0.0" << std::endl;
		std::cout << "\tDeveloped by Murat Yirci, COPYRIGHT 2022" << std::endl;
	}

	void print_help(Command* cmd = nullptr)
	{
		std::cout << "Usage: " << std::endl;
		std::cout << "DebruijnSequenceGenerator -cmd1 arg1 -cmd2 -cmdi -cmdj argj " << std::endl;
		std::cout << "Available commands: " << std::endl;
		std::cout << "\t-help\t\t\t -> for printing out the available commands" << std::endl;
		std::cout << "\t-about\t\t\t -> for printing the copyright and version" << std::endl;
		std::cout << "\t-quit\t\t\t -> for exiting the program" << std::endl;
		std::cout << "\t-alphabet value\t\t -> for setting the alphabet of the debruijn sequence" << std::endl;
		std::cout << "\t-order value\t\t -> for setting the desired debruijn sequence order" << std::endl;
		std::cout << "\t-start value\t\t -> for setting the starting sub-sequence" << std::endl;
		std::cout << "\t-method value\t\t -> for setting the computation method (either hamiltonian (default) or euler" << std::endl;
		std::cout << "\t-generate-graph\t\t -> for initializing the debruijn graph" << std::endl;
		std::cout << "\t-print-graph\t\t -> for printing the debruijn graph" << std::endl;
		std::cout << "\t-print-state\t\t -> for printing the current state of the parameters" << std::endl;
		std::cout << "\t-limit\t\t\t -> for setting the limit for the number of debruijn sequences to be computed (optional)" << std::endl;
		std::cout << "\t-compute \t\t -> for computing the debruijn sequences" << std::endl;
	}

	void quit(Command* cmd = nullptr)
	{
		exit(0);
	}
}

class DebruijnSequenceGenerator
{
public:

	enum class ComputationMethod : std::uint8_t
	{
		Hamiltonian,
		Euler
	};

	int _limit;
	std::string _start;
	ComputationMethod _method;
	std::unique_ptr<DebruijnGraph> _dg;

	DebruijnSequenceGenerator(CommandLineExecutor& executor) :
		_limit{ -1 },
		_start{ "" },
		_method{ ComputationMethod::Hamiltonian },
		_dg{ std::make_unique<DebruijnGraph>() }
	{
		executor.RegisterCommand(std::make_unique<Command>("quit", &command_executors::quit));
		executor.RegisterCommand(std::make_unique<Command>("about", &command_executors::print_about));
		executor.RegisterCommand(std::make_unique<Command>("help", &command_executors::print_help));

		auto alphabet_execute = [this](Command* cmd)
		{
			if (!cmd->_values || cmd->_values.value().empty())
				return command_executors::command_error(cmd->_cmd);

			auto& a = cmd->_values.value()[0];
			if (std::any_of(a.begin(), a.end(), [](char c) { return !std::isalnum(c); }))
				return command_executors::command_error(cmd->_cmd);

			this->_dg->SetAlphabet(a);
		};
		executor.RegisterCommand(std::make_unique<Command>("alphabet", alphabet_execute));

		auto order_execute = [this](Command* cmd)
		{
			if (!cmd->_values || cmd->_values.value().empty())
				return command_executors::command_error(cmd->_cmd);

			this->_dg->SetOrder(std::stoi(cmd->_values.value()[0]));
		};
		executor.RegisterCommand(std::make_unique<Command>("order", order_execute));

		auto start_execute = [this](Command* cmd)
		{
			if (!cmd->_values || cmd->_values.value().empty())
				return command_executors::command_error(cmd->_cmd);

			auto alphabet = this->_dg->GetAlphabet();
			if (alphabet.empty())
			{
				std::cout << "Please set the alphabet before setting up the starting sequence." << std::endl;
				return;
			}

			if (this->_dg->GetOrder() < MIN_ORDER)
			{
				std::cout << "Please set the order before setting up the starting sequence." << std::endl;
				return;
			}

			std::string_view start = cmd->_values.value()[0];
			if (start.size() != this->_dg->GetOrder())
			{
				std::cout << "Number of symbols in the starting sequence is not correct." << std::endl;
				return;
			}

			for (char c : start)
			{
				auto pos = alphabet.find(c);
				if (pos == std::string::npos)
				{
					std::cout << "Invalid char in the given sequence -> " << c << std::endl;
					return;
				}
			}

			this->_start = cmd->_values.value()[0];
		};
		executor.RegisterCommand(std::make_unique<Command>("start", start_execute));

		auto method_execute = [this](Command* cmd)
		{
			if (!cmd->_values || cmd->_values.value().empty())
				return command_executors::command_error(cmd->_cmd);

			if (cmd->_values.value()[0] == "hamiltonian")
				this->_method = ComputationMethod::Hamiltonian;
			else if (cmd->_values.value()[0] == "euler")
				this->_method = ComputationMethod::Euler;
			else
				return command_executors::command_error(cmd->_cmd);
		};
		executor.RegisterCommand(std::make_unique<Command>("method", method_execute));

		auto generate_graph_execute = [this](Command* cmd)
		{
			if (this->_dg->GetOrder() < MIN_ORDER)
			{
				std::cout << "Please set the order not smaller than the minimum allowed order(" << MIN_ORDER << ") before generating the graph." << std::endl;
				return;
			}

			if (this->_dg->GetAlphabet().empty())
			{
				std::cout << "Please set the alphabet before generating the graph." << std::endl;
				return;
			}

			this->_dg->InitGraph();
		};
		executor.RegisterCommand(std::make_unique<Command>("generate-graph", generate_graph_execute));

		auto print_graph_execute = [this](Command* cmd)
		{
			std::cout << "De bruijn graph:" << std::endl;
			std::cout << *(this->_dg) << std::endl;
		};
		executor.RegisterCommand(std::make_unique<Command>("print-graph", print_graph_execute));

		executor.RegisterCommand(std::make_unique<Command>("print-state", [this](Command*) -> void { this->PrintState(); }));

		auto limit_execute = [this](Command* cmd)
		{
			if (!cmd->_values || cmd->_values.value().empty())
				return command_executors::command_error(cmd->_cmd);

			auto val = std::stoi(cmd->_values.value()[0]);
			if (val < 1)
			{
				std::cout << "Limit needs to greater than 0." << std::endl;
				return;
			}
			this->_limit = val;
		};
		executor.RegisterCommand(std::make_unique<Command>("limit", limit_execute));

		auto compute_execute = [this](Command* cmd)
		{
			if (this->_dg->GetAlphabet().empty())
			{
				std::cout << "Please set the alphabet before computing De bruijn sequences." << std::endl;
				return;
			}

			if (this->_dg->GetOrder() < MIN_ORDER)
			{
				std::cout << "Please set the order ( > " << MIN_ORDER << ") before generating the graph." << std::endl;
				return;
			}

			this->_dg->InitGraph();

			if (this->_method == ComputationMethod::Hamiltonian)
			{
				auto dbj_seq = this->_dg->HamiltonianPaths(this->_start, this->_limit);

				for (const auto& seq : dbj_seq)
					std::cout << "\t" << seq << std::endl;
			}
			else if (this->_method == ComputationMethod::Euler)
			{
				auto dbj_seq = this->_dg->EulerCycles(this->_start, this->_limit);
				for (const auto& seq : dbj_seq)
					std::cout << "\t" << seq << std::endl;
			}
		};
		executor.RegisterCommand(std::make_unique<Command>("compute", compute_execute));
	}

	void PrintState() const
	{
		std::cout << "Current state:" << std::endl;
		std::cout << "\tAlphabet\t\t: " << _dg->GetAlphabet() << std::endl;
		std::cout << "\tOrder\t\t\t: " << _dg->GetOrder() << std::endl;
		std::cout << "\tStarting subsequence\t: " << _start << std::endl;
		std::cout << "\tMethod\t\t\t: ";
		if (_method == ComputationMethod::Hamiltonian)
			std::cout << "Hamiltonian" << std::endl;
		else if (_method == ComputationMethod::Euler)
			std::cout << "Euler" << std::endl;
		std::cout << "\tLimit\t\t\t: " << _limit << std::endl;
	}
};

int main(void)
{
	command_executors::print_header();
	command_executors::print_help();

	CommandLineExecutor executor;
	DebruijnSequenceGenerator dsg(executor);

	try
	{
		while (true) { executor.Execute(); }
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


