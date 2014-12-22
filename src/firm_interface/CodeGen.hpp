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
				RDI,//first arg
				RSI,//second arg
				R8,//fifth arg
				R9//sixth arg
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
			std::map<ir_node*, std::vector<ir_node*>> code;   //genereated code per block, bottom to top

			CodeGen(FILE* out);

			char const* constraintToRegister(Constraint c);

			void assemble(ir_graph* irg);
			void assemble(ir_node* irn);

			void output(ir_graph* irg);
			void output(ir_node* irn);

		public:
			static void assemble(FILE* out);
	};
}

#endif // CODEGEN_HPP
