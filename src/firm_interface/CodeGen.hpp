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

			std::map<ir_node*, unsigned> partial;  //number of unseen children
			std::map<ir_node*, std::pair<size_t, std::vector<size_t>>> usage;  //node data: {register written, registers read}

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

			struct Register
			{
				Constraint constraint;
				std::vector<ir_node*> writes;
				std::vector<ir_node*> reads;
			};

			std::vector<Register> registers;
			std::map<ir_node*, std::vector<std::string>> code;  //genereated code per block, bottom to top

			CodeGen(FILE* out);

			void assemble(ir_graph* irg);
			void assemble(ir_node* irn);

		public:
			static void assemble(FILE* out);
	};
}

#endif // CODEGEN_HPP
