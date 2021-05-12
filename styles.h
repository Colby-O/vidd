struct style styles[] = {
	{ { 0 },
	{
		{ "", 0, "", 0 },
	}},
	{ { ".c", ".h", 0 },
	{
		{ "//", 2, CYELLOW, false },

		{ ">=", 2, CYELLOW, false },
		{ "<=", 2, CYELLOW, false },
		{ "+=", 2, CYELLOW, false },
		{ "-=", 2, CYELLOW, false },
		{ "*=", 2, CYELLOW, false },
		{ "/=", 2, CYELLOW, false },
		{ "==", 2, CYELLOW, false },
		{ ">>", 2, CYELLOW, false },
		{ "<<", 2, CYELLOW, false },
		{ "&&", 2, CYELLOW, false },
		{ "||", 2, CYELLOW, false },
		{ "++", 2, CYELLOW, false },
		{ "--", 2, CYELLOW, false },
		{ "=", 1, CYELLOW, false },
		{ "+", 1, CYELLOW, false },
		{ "-", 1, CYELLOW, false },
		{ "*", 1, CYELLOW, false },
		{ "/", 1, CYELLOW, false },
		{ "!", 1, CYELLOW, false },
		{ "~", 1, CYELLOW, false },
		{ "|", 1, CYELLOW, false },
		{ "&", 1, CYELLOW, false },
		{ "<", 1, CYELLOW, false },
		{ ">", 1, CYELLOW, false },
		{ "?", 1, CYELLOW, false },
		{ "'", 1, CYELLOW, false },
		{ "\"", 1, CYELLOW, false },

		{ "void", 4, CBLUE, true },
		{ "char", 4, CBLUE, true },
		{ "short", 5, CBLUE, true },
		{ "int", 3, CBLUE, true },
		{ "long", 4, CBLUE, true },
		{ "bool", 4, CBLUE, true },
		{ "float", 5, CBLUE, true },
		{ "double", 6, CBLUE, true },
		{ "const", 5, CBLUE, true },
		{ "unsigned", 8, CBLUE, true },

		{ "for", 3, CRED, true },
		{ "if", 3, CRED, true },
		{ "else", 4, CRED, true },
		{ "while", 5, CRED, true },
		{ "struct", 6, CRED, true },
		{ "static", 6, CRED, true },
		{ "extern", 6, CRED, true },
		{ "enum", 4, CRED, true },
		{ "return", 6, CRED, true },
		{ "typedef", 7, CRED, true },
		{ "switch", 6, CRED, true },
		{ "case", 4, CRED, true },
		{ "default", 7, CRED, true },
		{ "break", 5, CRED, true },
		{ "continue", 8, CRED, true },

		{ "true", 4, CTEAL, true },
		{ "false", 5, CTEAL, true },

		{ "#include", 8, CGREEN, true },
		{ "#define", 7, CGREEN, true },
		{ "#ifdef", 6, CGREEN, true },
		{ "#ifndef", 7, CGREEN, true },
		{ "#endif", 6, CGREEN, true },

		{ "", 0, "", 0 },
	}},
	{ { ".java", 0 },
	{
		{ "//", 2, CYELLOW, false },

		{ ">=", 2, CYELLOW, false },
		{ "<=", 2, CYELLOW, false },
		{ "+=", 2, CYELLOW, false },
		{ "-=", 2, CYELLOW, false },
		{ "*=", 2, CYELLOW, false },
		{ "/=", 2, CYELLOW, false },
		{ "==", 2, CYELLOW, false },
		{ ">>", 2, CYELLOW, false },
		{ "<<", 2, CYELLOW, false },
		{ "&&", 2, CYELLOW, false },
		{ "||", 2, CYELLOW, false },
		{ "++", 2, CYELLOW, false },
		{ "--", 2, CYELLOW, false },
		{ "=", 1, CYELLOW, false },
		{ "+", 1, CYELLOW, false },
		{ "-", 1, CYELLOW, false },
		{ "*", 1, CYELLOW, false },
		{ "/", 1, CYELLOW, false },
		{ "!", 1, CYELLOW, false },
		{ "~", 1, CYELLOW, false },
		{ "|", 1, CYELLOW, false },
		{ "&", 1, CYELLOW, false },
		{ "<", 1, CYELLOW, false },
		{ ">", 1, CYELLOW, false },
		{ "?", 1, CYELLOW, false },
		{ "'", 1, CYELLOW, false },
		{ "\"", 1, CYELLOW, false },

		{ "void", 4, CBLUE, true },
		{ "char", 4, CBLUE, true },
		{ "int", 3, CBLUE, true },
		{ "boolean", 7, CBLUE, true },
		{ "double", 6, CBLUE, true },
		{ "String", 6, CBLUE, true },
		{ "const", 5, CBLUE, true },
		{ "unsigned", 8, CBLUE, true },

		{ "for", 3, CRED, true },
		{ "if", 3, CRED, true },
		{ "else", 4, CRED, true },
		{ "while", 5, CRED, true },
		{ "static", 6, CRED, true },
		{ "public", 6, CRED, true },
		{ "private", 7, CRED, true },
		{ "class", 7, CRED, true },
		{ "extern", 6, CRED, true },
		{ "return", 6, CRED, true },
		{ "switch", 6, CRED, true },
		{ "case", 4, CRED, true },
		{ "default", 7, CRED, true },
		{ "break", 5, CRED, true },
		{ "continue", 8, CRED, true },

		{ "true", 4, CTEAL, true },
		{ "false", 5, CTEAL, true },

		{ "import", 6, CGREEN, true },

		{ "", 0, "", 0 },
	}},
	{ { ".s", 0 },
	{
		{ ";", 1, CYELLOW, false },
		{ "#", 1, CYELLOW, false },
		{ "$", 1, CYELLOW, false },
		{ "%", 1, CYELLOW, false },
		{ "@", 1, CYELLOW, false },

		{ ".movq", 5, CRED, true },
		{ ".addq", 5, CRED, true },
		{ ".subq", 5, CRED, true },
		{ ".pushq", 6, CRED, true },
		{ ".popq", 5, CRED, true },
		{ ".cmp", 4, CRED, true },
		{ ".jmp", 4, CRED, true },
		{ ".je", 3, CRED, true },
		{ ".ret", 4, CRED, true },
		{ ".int", 4, CRED, true },
		{ ".byte", 5, CRED, true },
		{ ".word", 5, CRED, true },
		{ ".dword", 6, CRED, true },
		{ ".qword", 6, CRED, true },

		{ "", 0, "", 0 },
	}},
	{ { ".jl", 0 },
	{
		{ "#", 1, CYELLOW, false },

		{ ">=", 2, CYELLOW, false },
		{ "::", 2, CYELLOW, false },
		{ "<=", 2, CYELLOW, false },
		{ "+=", 2, CYELLOW, false },
		{ "-=", 2, CYELLOW, false },
		{ "*=", 2, CYELLOW, false },
		{ "/=", 2, CYELLOW, false },
		{ "==", 2, CYELLOW, false },
		{ ">>", 2, CYELLOW, false },
		{ "<<", 2, CYELLOW, false },
		{ "&&", 2, CYELLOW, false },
		{ "||", 2, CYELLOW, false },
		{ "++", 2, CYELLOW, false },
		{ "--", 2, CYELLOW, false },
		{ ":", 1, CYELLOW, false },
		{ "=", 1, CYELLOW, false },
		{ "+", 1, CYELLOW, false },
		{ "-", 1, CYELLOW, false },
		{ "*", 1, CYELLOW, false },
		{ "/", 1, CYELLOW, false },
		{ "!", 1, CYELLOW, false },
		{ "@", 1, CYELLOW, false },
		{ "~", 1, CYELLOW, false },
		{ "|", 1, CYELLOW, false },
		{ "&", 1, CYELLOW, false },
		{ "<", 1, CYELLOW, false },
		{ ">", 1, CYELLOW, false },
		{ "?", 1, CYELLOW, false },
		{ "'", 1, CYELLOW, false },
		{ ",", 1, CYELLOW, false },
		{ "\"", 1, CYELLOW, false },

		{ "String", 6, CBLUE, true },
		{ "Bool", 4, CBLUE, true },
		{ "Float16", 7, CBLUE, true },
		{ "Float32", 7, CBLUE, true },
		{ "Float64", 7, CBLUE, true },
		{ "Int", 3, CBLUE, true },
		{ "Int8", 4, CBLUE, true },
		{ "Int16", 5, CBLUE, true },
		{ "Int32", 5, CBLUE, true },
		{ "Int64", 5, CBLUE, true },
		{ "UInt8", 5, CBLUE, true },
		{ "UInt16", 6, CBLUE, true },
		{ "UInt32", 6, CBLUE, true },
		{ "UInt64", 6, CBLUE, true },

		{ "function", 8, CRED, true },
		{ "end", 3, CRED, true },
		{ "return", 6, CRED, true },
		{ "for", 3, CRED, true },
		{ "in", 2, CRED, true },
		{ "if", 3, CRED, true },
		{ "else", 4, CRED, true },
		{ "while", 5, CRED, true },
		{ "continue", 8, CRED, true },
		{ "struct", 6, CRED, true },

		{ "true", 4, CTEAL, true },
		{ "false", 5, CTEAL, true },

		{ "using", 5, CGREEN, true },

		{ "", 0, "", 0 },
	}},
	{ { ".py", 0 },
	{
		{ "#", 1, CYELLOW, false },

		{ ">=", 2, CYELLOW, false },
		{ "::", 2, CYELLOW, false },
		{ "<=", 2, CYELLOW, false },
		{ "+=", 2, CYELLOW, false },
		{ "-=", 2, CYELLOW, false },
		{ "*=", 2, CYELLOW, false },
		{ "/=", 2, CYELLOW, false },
		{ "==", 2, CYELLOW, false },
		{ ">>", 2, CYELLOW, false },
		{ "<<", 2, CYELLOW, false },
		{ "&&", 2, CYELLOW, false },
		{ "||", 2, CYELLOW, false },
		{ "++", 2, CYELLOW, false },
		{ "--", 2, CYELLOW, false },
		{ ":", 1, CYELLOW, false },
		{ "=", 1, CYELLOW, false },
		{ "+", 1, CYELLOW, false },
		{ "-", 1, CYELLOW, false },
		{ "*", 1, CYELLOW, false },
		{ "/", 1, CYELLOW, false },
		{ "!", 1, CYELLOW, false },
		{ "~", 1, CYELLOW, false },
		{ "|", 1, CYELLOW, false },
		{ "&", 1, CYELLOW, false },
		{ "<", 1, CYELLOW, false },
		{ ">", 1, CYELLOW, false },
		{ "?", 1, CYELLOW, false },
		{ "'", 1, CYELLOW, false },
		{ ",", 1, CYELLOW, false },
		{ "\"", 1, CYELLOW, false },

		{ "int", 3, CBLUE, true },
		{ "str", 3, CBLUE, true },
		{ "len", 3, CBLUE, true },
		{ "tuple", 5, CBLUE, true },

		{ "def", 3, CRED, true },
		{ "return", 6, CRED, true },
		{ "for", 3, CRED, true },
		{ "in", 2, CRED, true },
		{ "if", 3, CRED, true },
		{ "else", 4, CRED, true },
		{ "elif", 4, CRED, true },
		{ "while", 5, CRED, true },
		{ "continue", 8, CRED, true },
		{ "as", 2, CRED, true },
		{ "try", 3, CRED, true },
		{ "except", 6, CRED, true },

		{ "True", 4, CTEAL, true },
		{ "False", 5, CTEAL, true },
		{ "None", 5, CTEAL, true },

		{ "import", 6, CGREEN, true },
		{ "from", 4, CGREEN, true },

		{ "", 0, "", 0 },
	}},
};
