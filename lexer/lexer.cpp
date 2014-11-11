#include "lexer.hpp"
#include "token.hpp"

std::istream& Lexer::getInput()
{
	return input;
}

Lexer::Lexer(std::istream& input, Stateomat const& stateomat, bool debug)
	: position {1, 1}, input(input), stateomat(stateomat), debug(debug)
{
	c = input.get();
	advancePosition(c);
}

void Lexer::advancePosition(int nextCharacter)
{
	if (nextCharacter == 10)
	{
		position.first++;
		position.second = 1;
	}
	else
		position.second++;
}

Token Lexer::get_next_token()
{
	if (!token_stack.empty())
	{
		Token t(token_stack[token_stack.size() - 1]);
		token_stack.pop_back();
		return t;
	}

	Token t;
	t.position = position;
	int state = STATE_START;

	while (1)
	{
		int new_state = stateomat.transitions[state][c == EOF ? 128 : c];

		if (new_state == STATE_STOP)
		{
			if (stateomat.state_is_accepting(state))
			{

				if (t.token_type == Token::Token_type::TOKEN_IDENT)
				{
					auto keyword = stateomat.keywords.find(t.string_value);

					if (keyword != stateomat.keywords.end())
						t.token_type = keyword->second;
				}
				else if (t.token_type == Token::Token_type::TOKEN_OPERATOR)
					t.token_type = stateomat.operators[t.string_value];

				if (debug)
					t.print();

				return t;
			}
			else
			{
				t.token_type = Token::Token_type::TOKEN_ERROR;
				return t;
			}
		}
		else if (!stateomat.state_is_accepting(new_state))
			t.string_value.clear();
		else
		{
			if (state == STATE_START)
			{
				t.position = position;
				t.position.second--;
			}

			t.string_value.push_back(c);
			t.token_type = stateomat.state_type[new_state];
		}

		c = input.get();
		advancePosition(c);

		state = new_state;
	}

	t.token_type = Token::Token_type::TOKEN_ERROR;
	return t;
}

std::string Lexer::describe(Token::Token_type const& t) const
{
	auto it = stateomat.reverse.find(t);

	if (it != stateomat.reverse.end())
		return it->second;
	else
		return "";
}

bool Lexer::good() const
{
	return input.good();
}

void Lexer::unget_token(Token const& t)
{
	token_stack.push_back(t);
}
