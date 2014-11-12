#include "lexer.hpp"
#include "token.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>

Lexer::Lexer(const char* file_name, Stateomat const& stateomat, bool debug)
	: position {1, 1}, stateomat(stateomat), debug(debug), line_start(0)
{

	fd = open(file_name, O_RDONLY);

	if (fd == -1)
		throw 42;

	if (posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL) != 0)
		throw 42;

	buf_off = 0;
	buf_len = 0;

	c = getc();
}

void Lexer::advancePosition(int nextCharacter)
{
	if (nextCharacter == '\n')
	{
		line_start += position.second;
		position.first++;
		position.second = 1;
	}
	else
		position.second++;
}

char Lexer::getc()
{

	char foo;

	if (buf_off < buf_len)
	{
		foo = buf[buf_off];
		buf_off++;
	}
	else
	{
		ssize_t bytes_read = read(fd, buf, BUF_SIZE);

		if (bytes_read == -1)
			throw 42;
		else if (bytes_read == 0)
			foo = EOF;
		else
		{
			buf_off = 0;
			buf_len = bytes_read;
			return getc();
		}
	}

	return foo;
}

std::string Lexer::getLine()
{

	off_t initial_offset = lseek(fd, 0, SEEK_CUR);

	lseek(fd, line_start, SEEK_SET);

	FILE* f = fdopen(fd, "r");

	char* line = NULL;
	size_t alloc = 0;
	ssize_t len = getline(&line, &alloc, f);
	line[len - 1] = '\0';

	std::string s(line);

	free(line);

	lseek(fd, initial_offset, SEEK_SET);

	return s;
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

				if (t.token_type == Token::Token_type::TOKEN_OPERATOR)
					t.token_type = stateomat.operators[state][t.string_value];
				else if (t.token_type == Token::Token_type::TOKEN_IDENT_OR_KEYWORD)
				{
					auto keyword = stateomat.keywords.find(t.string_value);

					if (keyword != stateomat.keywords.end())
						t.token_type = keyword->second;
					else
						t.token_type = Token::Token_type::TOKEN_IDENT;
				}

				if (debug)
					t.print();

				return t;
			}
			else
			{
				t.token_type = Token::Token_type::TOKEN_ERROR;
				t.position = position;
				return t;
			}
		}
		else if (!stateomat.state_is_accepting(new_state))
			t.string_value.clear();
		else
		{
			if (state == STATE_START)
				t.position = position;

			t.string_value.push_back(c);
			t.token_type = stateomat.state_type[new_state];
		}

		advancePosition(c);
		c = getc();

		state = new_state;
	}

	t.token_type = Token::Token_type::TOKEN_ERROR;
	t.position = position;
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
	return true;
}

void Lexer::unget_token(Token const& t)
{
	token_stack.push_back(t);
}
