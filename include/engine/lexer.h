#pragma once 
#include <string>
#include <unordered_map>
#include <vector>


namespace tay {
    enum TokenType{
        KEYWORD,
        IDENTIFIER,
        // literals
        INTEGER,
        FLOAR,
        STRING,

        //operators
        PLUS,
        MINUS,
        MULTIPLY,
        DIV,
        MOD,

        EQ,
        NEQ,
        LT,
        LE,
        GT,
        GE,

        //
        L_BRACKET,
        R_BRACKET,
        L_PAREN,
        R_PAREN,
        COMMA,
        SEMICOLON,
        COLON,
        DOT,
    };
    inline const std::unordered_map<std::string, TokenType> KeyWords = {
        {"SELECT", TokenType::KEYWORD},
        {"FROM", TokenType::KEYWORD},
        {"INSERT", TokenType::KEYWORD},
        {"INTO", TokenType::KEYWORD},
        {"VALUES", TokenType::KEYWORD},
        {"TABLE", TokenType::KEYWORD},
        {"CREATE", TokenType::KEYWORD},
        {"ALTER", TokenType::KEYWORD},
        {"DROP", TokenType::KEYWORD},
        {"INTEGER", TokenType::KEYWORD},
        {"TEXT", TokenType::KEYWORD},
        {"CHAR", TokenType::KEYWORD},
        {"DATABASE", TokenType::KEYWORD},
        {"TRUNCATE", TokenType::KEYWORD},
        {"NULL", TokenType::KEYWORD},
        {"NOT", TokenType::KEYWORD},
        {"AND",TokenType::KEYWORD},
        {"OR",TokenType::KEYWORD},
        {"EXISTS", TokenType::KEYWORD},
        {"CHECK", TokenType::KEYWORD},
        {"CONSTRAINT", TokenType::KEYWORD},
        {"PRIMARY", TokenType::KEYWORD},
        {"KEY", TokenType::KEYWORD},
        {"FOREIGN", TokenType::KEYWORD},
        {"REFERENCES", TokenType::KEYWORD},
        {"UNIQUE", TokenType::KEYWORD},
        {"DEFAULT", TokenType::KEYWORD},
        {"BETWEEN", TokenType::KEYWORD},
        {"IN", TokenType::KEYWORD},
        {"LIKE", TokenType::KEYWORD},
        {"IS", TokenType::KEYWORD},
        {"AS", TokenType::KEYWORD},
        {"ORDER", TokenType::KEYWORD},
        {"BY", TokenType::KEYWORD},
        {"ASC", TokenType::KEYWORD},
        {"DESC", TokenType::KEYWORD},
        {"LIMIT", TokenType::KEYWORD},
        {"OFFSET", TokenType::KEYWORD},
        {"INTERSECT", TokenType::KEYWORD},
        {"UNION", TokenType::KEYWORD},
        {"EXCEPT", TokenType::KEYWORD},
        {"BETWEEN", TokenType::KEYWORD},     
    };

    class Token{
        public:
        Token();
        Token(const std::string& value, TokenType tok_type, int start, int end);
        Token(const std::string& value, TokenType tok_type);
        ~Token();
        private:
        std::string value;
        TokenType tok_type;
        int line = 0;
        int col = 0;
    };

    class Lexer {
        public:
        Lexer(const std::string& path);
        ~Lexer();
        std::vector<Token> tokenize();
        void skip_white_space();
        void skip_comment();

        private:
        const std::string path;
        std::vector<Token> tokens;
        int current;
    };
}