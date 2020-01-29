#pragma once
/*
* Covariant Script Implementation
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2020 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/impl/compiler.hpp>
#include <covscript/impl/runtime.hpp>

namespace cs {
	class instance_type final : public runtime_type {
		friend class repl;

		// Statements
		std::deque<statement_base *> statements;
	public:
		// Status
		bool return_fcall = false;
		bool break_block = false;
		bool continue_block = false;
		// Context
		context_t context;

		// Constructor and destructor
		instance_type() = delete;

		explicit instance_type(context_t c) : context(std::move(c))
		{
			struct_builder::reset_counter();
		}

		instance_type(const instance_type &) = delete;

		~instance_type() = default;

		// Wrapped Method
		namespace_t import(const std::string &, const std::string &);

		namespace_t source_import(const std::string &);

		void compile(const std::string &);

		void interpret();

		void dump_ast(std::ostream &);

		// Var definition
		void check_declar_var(tree_type<token_base *>::iterator, bool= false);

		void check_define_var(tree_type<token_base *>::iterator, bool= false, bool= false);

		void parse_define_var(tree_type<token_base *>::iterator, bool= false);

		void check_define_structured_binding(tree_type<token_base *>::iterator, bool= false);

		void parse_define_structured_binding(tree_type<token_base *>::iterator, bool= false);
	};

// Repl
	class repl final {
		std::deque<std::deque<token_base *>> tmp;
		stack_type<method_base *> methods;
		charset encoding = charset::utf8;
		std::size_t line_num = 0;
		bool multi_line = false;
		string line_buff;
		string cmd_buff;

		void interpret(const string &, std::deque<token_base *> &);

		void run(const string &);

	public:
		context_t context;

		repl() = delete;

		explicit repl(context_t c) : context(std::move(c))
		{
			context->file_path = "<REPL_ENV>";
		}

		repl(const repl &) = delete;

		void exec(const string &);

		void reset_status()
		{
			context_t __context = context;
			charset __encoding = encoding;
			std::size_t __line_num = line_num;
			this->~repl();
			::new(this) repl(__context);
			encoding = __encoding;
			line_num = __line_num;
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
		}

		int get_level() const
		{
			return methods.size();
		}
	};

// Guarder
	class scope_guard final {
		context_t context;
	public:
		scope_guard() = delete;

		explicit scope_guard(context_t c) : context(std::move(c))
		{
			context->instance->storage.add_domain();
		}

		~scope_guard()
		{
			context->instance->storage.remove_domain();
		}

		const domain_type &get() const
		{
			return context->instance->storage.get_domain();
		}

		void clear() const
		{
			context->instance->storage.clear_domain();
		}
	};

	class fcall_guard final {
	public:
#ifdef CS_DEBUGGER
		fcall_guard() = delete;

		explicit fcall_guard(const std::string &decl)
		{
			current_process->stack.push(null_pointer);
			current_process->stack_backtrace.push(decl);
		}

		~fcall_guard()
		{
			current_process->stack.pop_no_return();
			current_process->stack_backtrace.pop_no_return();
		}
#else

		fcall_guard()
		{
			current_process->stack.push(null_pointer);
		}

		~fcall_guard()
		{
			current_process->stack.pop_no_return();
		}

#endif

		var get() const
		{
			return current_process->stack.top();
		}
	};
}
