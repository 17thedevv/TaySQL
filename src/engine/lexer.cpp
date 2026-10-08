#include <engine/lexer.h>

namespace tay {

    Token::Token() {
    }

    Token::Token(const std::string& value, TokenType tok_type, int start, int end) : 
        value(value), tok_type(tok_type), start(start), end(end) {}

    Token::Token(const std::string& value, TokenType tok_type) : 
        value(value), tok_type(tok_type), start(0), end(0) {}

    Token::~Token() {}

    Lexer::Lexer(const std::string& file) : path(file) {}

    Lexer::~Lexer() {}

    std::vector<Token> Lexer::tokenize() {
        return this->tokens;
    }

    void Lexer::skip_white_space() {
        
    }
    void Lexer::skip_comment(){

    }

}
