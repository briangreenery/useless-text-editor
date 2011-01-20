// RelevanceToken.h

#ifndef RelevanceToken_h
#define RelevanceToken_h

class RelevanceToken
{
public:
	enum Token
	{
		t_endOfInput,

		t_and,
		t_as,
		t_closeParen,
		t_comma,
		t_concat,
		t_containedBy,
		t_contains,
		t_else,
		t_endsWith,
		t_equal,
		t_exists,
		t_false,
		t_greater,
		t_greaterOrEqual,
		t_if,
		t_ignored,
		t_illegal,
		t_it,
		t_less,
		t_lessOrEqual,
		t_minus,
		t_mod,
		t_not,
		t_notContainedBy,
		t_notContains,
		t_notEndsWith,
		t_notEqual,
		t_notExists,
		t_notGreater,
		t_notGreaterOrEqual,
		t_notLess,
		t_notLessOrEqual,
		t_notStartsWith,
		t_number,
		t_of,
		t_openParen,
		t_or,
		t_phraseItem,
		t_phraseItems,
		t_plus,
		t_semiColon,
		t_slash,
		t_star,
		t_startsWith,
		t_string,
		t_then,
		t_true,
		t_whitespace,
		t_whose,
		t_word,
	};

	RelevanceToken() : m_token( t_endOfInput ) {}
	RelevanceToken( Token token ) : m_token( token ) {}

	operator Token() const { return m_token; }

private:
	Token m_token;
};

#endif
