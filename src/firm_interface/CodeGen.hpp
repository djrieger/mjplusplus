#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <stack>
#include <set>
#include <map>
#include <vector>

#include <stdio.h>

#include <libfirm/firm.h>

namespace firm
{
	class CodeGen
	{
		private:
			FILE* out;

			enum Constraint
			{
				NONE = 0,
				RAX,//return, call result
				RCX,//fourth arg
				RDX,//third arg
				RBX, RSP, RBP,
				RSI,//second arg
				RDI,//first arg
				R8,//fifth arg
				R9,//sixth arg
				R10, R11, R12, R13, R14, R15
			};

			struct Access
			{
				Constraint constraint;
				size_t reg;
			};

			struct Register
			{
				std::vector<ir_node*> writes;
				std::vector<ir_node*> reads;
			};

			static const Constraint arg_order[];

			std::map<ir_node*, size_t> partial;  //number of unseen children
			std::map<ir_node*, std::pair<std::vector<Access>, std::vector<Access>>> usage;  //node data: {registers written, registers read}
			std::vector<Register> registers;
			std::set<size_t> free_registers;
			std::map<ir_node*, std::vector<ir_node*>> code;   //genereated code per block, bottom to top

			CodeGen(FILE* out);

			size_t new_register();
			void merge_register(size_t a, size_t b, bool add_to_free = true);

			char const* constraintToRegister(Constraint c, ir_mode* mode);
			char const* operationSuffix(ir_mode* mode);

			void assemble(ir_graph* irg);
			void assemble(ir_node* irn);

			void load_or_imm(ir_node* node, size_t reg);
			void gen_bin_op(ir_node* irn, ir_mode* mode, char const* op);

			void output(ir_graph* irg);
			void output(ir_node* irn, ir_node* block);

		public:
			static void assemble(FILE* out);
	};
}

#endif // CODEGEN_HPP
