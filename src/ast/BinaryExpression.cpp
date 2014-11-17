#include "../globals.hpp"
#include "BinaryExpression.hpp"

namespace ast
{
	namespace be
	{

		BinaryExpression::BinaryExpression(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: leftChild(std::move(leftChild)), rightChild(std::move(rightChild))
		{
			;
		}

		void BinaryExpression::toString(std::ostream& out, unsigned int indent, std::string op, bool special) const
		{
			if (!special)
				out << '(';

			leftChild->toString(out, indent);
			out << ' ' << op << ' ';
			rightChild->toString(out, indent);

			if (!special)
				out << ')';
		}

		uptr<BinaryExpression> BinaryExpression::createBinaryExpr(uptr<Expression>& leftChild, uptr<Expression>& rightChild, Token::Token_type operator_type)
		{
			uptr<BinaryExpression> binaryExpr;

			switch (operator_type)
			{
				case Token::Token_type::OPERATOR_NOTEQ:
					binaryExpr = std::make_unique<NotEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_NOT:
					binaryExpr = std::make_unique<Not>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LPAREN:
					binaryExpr = std::make_unique<LeftParen>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_RPAREN:
					binaryExpr = std::make_unique<RightParen>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MULTEQ:
					binaryExpr = std::make_unique<MultEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MULT:
					binaryExpr = std::make_unique<Mult>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_PLUPLUS:
					binaryExpr = std::make_unique<PlusPlus>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_PLUSEQ:
					binaryExpr = std::make_unique<PlusEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_PLUS:
					binaryExpr = std::make_unique<Plus>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_COMMA:
					binaryExpr = std::make_unique<Comma>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MINUSEQ:
					binaryExpr = std::make_unique<MinusEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MINUSMINUS:
					binaryExpr = std::make_unique<MinusMinus>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MINUS:
					binaryExpr = std::make_unique<Minus>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_DOT:
					binaryExpr = std::make_unique<Dot>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_SLASHEQ:
					binaryExpr = std::make_unique<SlashEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_SLASH:
					binaryExpr = std::make_unique<Slash>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_COLON:
					binaryExpr = std::make_unique<Colon>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_SEMICOLON:
					binaryExpr = std::make_unique<SemiColon>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LTLTEQ:
					binaryExpr = std::make_unique<LessThanLessThanEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LTLT:
					binaryExpr = std::make_unique<LessThanLessThan>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LTEQ:
					binaryExpr = std::make_unique<LessThanEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LT:
					binaryExpr = std::make_unique<LessThan>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_EQEQ:
					binaryExpr = std::make_unique<EqEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_EQ:
					binaryExpr = std::make_unique<Eq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_GTEQ:
					binaryExpr = std::make_unique<GreaterThanEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_GTGTEQ:
					binaryExpr = std::make_unique<GreaterThanGreaterThanEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_GTGTGTEQ:
					binaryExpr = std::make_unique<GreaterThanGreaterThanGreaterThanEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_GTGTGT:
					binaryExpr = std::make_unique<GreaterThanGreaterThanGreaterThan>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_GTGT:
					binaryExpr = std::make_unique<GreaterThanGreaterThan>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_GT:
					binaryExpr = std::make_unique<GreaterThan>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_QUESTION:
					binaryExpr = std::make_unique<Question>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MODEQ:
					binaryExpr = std::make_unique<ModEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_MOD:
					binaryExpr = std::make_unique<Mod>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_ANDEQ:
					binaryExpr = std::make_unique<AndEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_ANDAND:
					binaryExpr = std::make_unique<AndAnd>(leftChild, rightChild);;
					break;

				case Token::Token_type::OPERATOR_AND:
					binaryExpr = std::make_unique<And>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LBRACKET:
					binaryExpr = std::make_unique<LeftBracket>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_RBRACKET:
					binaryExpr = std::make_unique<RightBracket>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_XOREQ:
					binaryExpr = std::make_unique<XorEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_XOR:
					binaryExpr = std::make_unique<Xor>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_LBRACE:
					binaryExpr = std::make_unique<LeftBrace>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_RBRACE:
					binaryExpr = std::make_unique<RightBrace>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_NEG:
					binaryExpr = std::make_unique<Neg>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_OREQ:
					binaryExpr = std::make_unique<OrEq>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_OROR:
					binaryExpr = std::make_unique<OrOr>(leftChild, rightChild);
					break;

				case Token::Token_type::OPERATOR_OR:
					binaryExpr = std::make_unique<Or>(leftChild, rightChild);
					break;

				default:
					binaryExpr = std::make_unique<Invalid>(leftChild, rightChild);
					break;
			}

			return binaryExpr;
		}

		NotEq::NotEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void NotEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "!=", special);
		}


		Not::Not(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Not::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "!", special);
		}

		LeftParen::LeftParen(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LeftParen::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "(", special);
		}

		RightParen::RightParen(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void RightParen::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ")", special);
		}

		MultEq::MultEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void MultEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "*=", special);
		}

		Mult::Mult(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Mult::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "*", special);
		}

		PlusPlus::PlusPlus(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void PlusPlus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "++", special);
		}

		PlusEq::PlusEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void PlusEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "+=", special);
		}

		Plus::Plus(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Plus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "+", special);
		}

		Comma::Comma(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Comma::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ",", special);
		}

		MinusMinus::MinusMinus(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void MinusMinus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "--", special);
		}

		MinusEq::MinusEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void MinusEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "-=", special);
		}

		Minus::Minus(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Minus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "-", special);
		}

		Dot::Dot(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Dot::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ".", special);
		}

		SlashEq::SlashEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void SlashEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "/=", special);
		}

		Slash::Slash(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Slash::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "/", special);
		}

		Colon::Colon(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Colon::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ":", special);
		}

		SemiColon::SemiColon(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void SemiColon::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ";", special);
		}

		LessThanLessThanEq::LessThanLessThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LessThanLessThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<<=", special);
		}

		LessThanLessThan::LessThanLessThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LessThanLessThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<<", special);
		}

		LessThanEq::LessThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LessThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<=", special);
		}

		LessThan::LessThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LessThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<", special);
		}

		EqEq::EqEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void EqEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "==", special);
		}

		Eq::Eq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Eq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "=", special);
		}

		GreaterThanEq::GreaterThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">=", special);
		}

		GreaterThanGreaterThanEq::GreaterThanGreaterThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThanGreaterThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">>=", special);
		}

		GreaterThanGreaterThanGreaterThanEq::GreaterThanGreaterThanGreaterThanEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThanGreaterThanGreaterThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">>>=", special);
		}

		GreaterThanGreaterThanGreaterThan::GreaterThanGreaterThanGreaterThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThanGreaterThanGreaterThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">>>", special);
		}

		GreaterThanGreaterThan::GreaterThanGreaterThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThanGreaterThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">>", special);
		}

		GreaterThan::GreaterThan(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">", special);
		}

		Question::Question(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Question::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "?", special);
		}

		ModEq::ModEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void ModEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "%=", special);
		}

		Mod::Mod(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Mod::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "%", special);
		}

		AndEq::AndEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void AndEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "&=", special);
		}

		AndAnd::AndAnd(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void AndAnd::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "&&", special);
		}

		And::And(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void And::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "&", special);
		}

		LeftBracket::LeftBracket(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LeftBracket::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "[", special);
		}

		RightBracket::RightBracket(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void RightBracket::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "]", special);
		}

		XorEq::XorEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void XorEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "^=", special);
		}

		Xor::Xor(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Xor::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "^", special);
		}

		LeftBrace::LeftBrace(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LeftBrace::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "{", special);
		}

		RightBrace::RightBrace(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void RightBrace::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "}", special);
		}

		Neg::Neg(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Neg::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "-", special);
		}

		OrEq::OrEq(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void OrEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "|=", special);
		}

		OrOr::OrOr(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void OrOr::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "||", special);
		}

		Or::Or(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Or::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "|", special);
		}

		Invalid::Invalid(uptr<Expression>& leftChild, uptr<Expression>& rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Invalid::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "NO_VALID_OPERATOR", special);
		}

	}

}

