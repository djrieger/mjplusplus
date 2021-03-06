#ifndef FIRM_INTERFACE_HPP
#define FIRM_INTERFACE_HPP

#include <unordered_map>
#include <queue>

#include <libfirm/firm.h>
#include <libfirm/irdump.h>
#include <libfirm/irverify.h>

#include "../ast/MethodDeclaration.hpp"
#include "../ast/TypeIdent.hpp"
#include "../ast/Statement.hpp"
#include "../ast/Block.hpp"
#include "../ast/BinaryExpression.hpp"
#include "../ast/PrimaryExpression.hpp"
#include "../ast/MethodInvocation.hpp"

namespace std
{
	/**
	 * We need a hash for std::pair<ir_type*, std::string> for maps
	 * such as firm::FirmInterface::classMethodEntities and
	 * firm::FirmInterface::classFieldEntities
	 */
	template<>
	struct hash<pair<ir_type*, string>>
	{
		size_t operator ()(pair<ir_type*, string> const& t) const
		{
			hash<ir_type*> hi;
			hash<string> hs;
			return hi(t.first) ^ hs(t.second);
		}
	};
}

namespace firm
{
	class FirmInterface
	{

		private:
			FirmInterface();
			FirmInterface(FirmInterface const&) = delete;
			void operator=(FirmInterface const&) = delete;
			static shptr<FirmInterface> instance;
			/**
			 * Name of mj source file
			 */
			std::string in_name;
			/**
			 * Name of assembler file to generate
			 */
			std::string out_name;

			ir_op* lea;

			/**
			 * Flag used for the optimization 0=OFF,1=ON
			 */
			int optimizationFlag;

			/**
			 * Flag for graph output
			 */
			bool dumpFirmGraph;

			shptr<std::map<std::string, int>> var2pos;

			std::unordered_map<shptr<ast::Type>, ir_type*> types;
			std::unordered_map<std::pair<ir_type*, std::string>, ir_entity*> classMethodEntities;
			std::unordered_map<std::pair<ir_type*, std::string>, ir_entity*> classFieldEntities;

			std::tuple<ir_node*, ir_type*> createNodeForMethodCall(ir_node* caller,
			        shptr<ast::Arguments const> arguments,
			        shptr<ast::MethodDeclaration const> methodDeclaration);

		public:
			static FirmInterface& getInstance()
			{
				static FirmInterface instance;
				return instance;
			}
			std::tuple<ir_node*, ir_type*> createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr);
			std::tuple<ir_node*, ir_type*> createNodeForMethodCall(ir_node* caller, shptr<ast::po::MethodInvocation const> expr);
			ir_node* createNodeForCallocCall(ir_node* count, unsigned int size);
			ir_node* createNodeForIntegerConstant(int64_t);
			ir_node* createNodeForBooleanConstant(bool);
			ir_node* createModOperation(ir_node* left, ir_node* right);
			ir_node* createDivOperation(ir_node* left, ir_node* right);
			ir_node* createNullPointerNode();

			~FirmInterface();
			/**
			 * Convert program to Firm representation and call build() if successful
			 */
			void convert(shptr<ast::Program> program);
			/**
			 * Convert the generated Firm graphs to assembler and save in file out_name
			 */
			void build();

			/**
			 * Outputs the given firm graph with the given string as suffix as vcg file, if the
			 * graph output option is enabled.
			 * @param irg    the given firm graph
			 * @param suffix the given suffix
			 */
			void outputFirmGraph(ir_graph* irg, std::string suffix);

			/**
			 * Optimize all firm graphs using the optimization flags.
			 * Calls optimize(ir_graph*)
			 */
			void optimizeGraphs();

			/**
			 * Replaces chains of Conv nodes with a single node.
			 * @param irg the given firm graph
			 */
			void handleConvNodes(ir_graph* irg);

			/**
			 * Convert the generated Firm graphs to assembler using FIRM itself and save in file out_name
			 */
			void buildWithFirm();
			/**
			 * Replace all dollar signs in name with _C
			 */
			static std::string replaceDollarAndUnderscores(std::string name);
			// Get Firm type modes
			ir_mode* getIntegerMode();
			ir_mode* getBooleanMode();
			ir_mode* getReferenceMode();
			ir_op* getLeaOp() const;
			ir_mode* getMode(shptr<ast::Type> ast_type);
			ir_type* getType(shptr<ast::Type> ast_type);
			void addClassType(shptr<ast::Ident> class_ident, ir_type* class_type);
			void addMethod(ir_type* class_type, std::string method_name, ir_entity* ent);
			void addField(ir_type* class_type, std::string method_name, ir_entity* ent);
			ir_entity* getMethodEntity(ir_type* class_type, std::string method_name);
			ir_entity* getFieldEntity(ir_type* class_type, std::string method_name);
			ir_entity* createMethodEntity(ir_type* caller, shptr<ast::MethodDeclaration const> methodDeclaration);

			void setInput(std::string const& in);
			void setOutput(std::string const& out);
			enum OptimizationFlags
			{
				NONE = 0,
				DEFAULT = 1 << 0,
				FIRM_COMPATIBLE = 1 << 1,
				CUSTOM_PRINT = 1 << 2 //or SYSTEM_PRINT...depends on what should be default behaviour.
			};
			void setOptimizationFlag(int flag);
			int getOptimizationFlag() const;
			void setFirmGraphOutput(bool dumpFirmGraph);

			shptr<std::map<std::string, int>> getVarMap();
			void setVarMap(shptr<std::map<std::string, int>> newVar2pos);

			std::queue<ir_node*> getWorklist(ir_graph* irg);
			std::vector<std::pair<ir_node*, unsigned int>> getOuts(ir_node const* n);
	};
}

ir_node* new_r_Lea(ir_node* block, ir_node* offset, ir_node* base, ir_node* x, ir_node* scale, ir_mode* mode);
ir_node* new_Lea(ir_node* offset, ir_node* base, ir_node* x, ir_node* scale, ir_mode* mode);
int is_Lea(ir_node const* node);

#endif
