#ifndef COMMON_SUBEXPRESSION_ELIMINATOR_HPP
#define COMMON_SUBEXPRESSION_ELIMINATOR_HPP

#include <libfirm/firm.h>
#include <unordered_map>
#include <tuple>
#include "../../globals.hpp"
#include "../GraphHandler.hpp"
#include "../Node.hpp"

namespace firm
{
	typedef std::tuple<unsigned, unsigned, unsigned, vec<unsigned>> nodeInfo;
}

namespace std
{
	template<>
	struct hash<firm::nodeInfo>
	{
		size_t operator ()(firm::nodeInfo const& t) const
		{
			hash<unsigned> h;
			size_t hash = h(std::get<0>(t)) ^ h(std::get<1>(t)) ^ h(std::get<2>(t));

			for (auto it : std::get<3>(t))
				hash = hash ^ h(it);

			return hash;
		}
	};
}

namespace firm
{
	class CommonSubexpressionEliminator : public GraphHandler
	{

		protected:
			std::unordered_map<long int, Node> const_Is_nodes;
			std::unordered_map<uint64_t, Node> const_Lu_nodes;
			std::unordered_map<nodeInfo, Node> comp_nodes;
			void handleConst(Node node);
			void handleArithmetic(Node node);
		public:
			CommonSubexpressionEliminator(ir_graph* irg);
			virtual void handle(Node);
			virtual void cleanUp(Node node);
	};
}
#endif
