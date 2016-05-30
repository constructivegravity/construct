#pragma once

#include <sstream>

namespace Construction {
    namespace Language {

        /**
            \class Token

            Tokens for lexical analysis of the input. In our simple language
            this means literals, strings, brackets, comma and % for the previous
            output.
         */
        class Token {
        public:
            enum Type {
                LITERAL,
                PREVIOUS,
                LBRACKET,
                RBRACKET,
                ASSIGNMENT,
                COMMA,
                STRING,
                NUMERIC,
                EOL
            };
        public:
            Token() = default;

            /**
                Constructor of a token
             */
            Token(Type type, unsigned pos, std::string content) : pos(pos), type(type), content(content) { }
        public:
            unsigned GetPosition() const { return pos; }
            std::string GetContent() const { return content; }
        public:
            bool IsLiteral() const { return type == LITERAL; }
            bool IsPrevious() const { return type == PREVIOUS; }
            bool IsLeftBracket() const { return type == LBRACKET; }
            bool IsRightBracket() const { return type == RBRACKET; }
            bool IsComma() const { return type == COMMA; }
            bool IsAssignment() const { return type == ASSIGNMENT; }
            bool IsString() const { return type == STRING; }
            bool IsNumeric() const { return type == NUMERIC; }
            bool IsEndOfLine() const { return type == EOL; }

            std::string TypeToString() const {
                switch (type) {
                    case LITERAL: return "Literal";
                    case PREVIOUS: return "Previous";
                    case LBRACKET: return "LBracket";
                    case RBRACKET: return "RBracket";
                    case COMMA: return "Comma";
                    case ASSIGNMENT: return "Assignment";
                    case STRING: return "String";
                    case NUMERIC: return "Numeric";
                    case EOL: return "EndOfLine";
                }
                return "Unknown";
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Token& token) {
                os << "(" << token.TypeToString() << " " << token.content << " " << token.pos << ")";
                return os;
            }
        private:
            unsigned pos;
            Type type;
            std::string content;
        };

        /**
            \class Node
         */
        class Node {
        public:
            enum Type {
                LITERAL,
                COMMAND,
                ARGUMENTS,
                ASSIGNMENT,
                STRING,
                NUMERIC,
                PREVIOUS
            };
        public:
            Node(const std::string& name, Type type) : name(name), type(type) { }
        public:
            std::string GetName() const { return name; }
        public:
            bool IsLiteral() const { return type == Node::LITERAL; }
            bool IsCommand() const { return type == Node::COMMAND; }
            bool IsArguments() const { return type == Node::ARGUMENTS; }
            bool IsAssignment() const { return type == Node::ASSIGNMENT; }
            bool IsPrevious() const { return type == Node::PREVIOUS; }
            bool IsString() const { return type == Node::STRING; }
            bool IsNumeric() const { return type == Node::NUMERIC; }
        public:
            virtual std::string ToString() const {
                return name;
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Node& node) {
                os << node.ToString();
                return os;
            }
        protected:
            std::string name;
            Type type;
        };

        // Forward declaration
        class LiteralNode;
        class ArgumentsNode;
        class StringNode;
        class NumericNode;
        class PreviousNode;

        /**
            \class CommandNode
         */
        class CommandNode : public Node {
        public:
            CommandNode(const std::shared_ptr<LiteralNode>& identifier, const std::shared_ptr<ArgumentsNode>& arguments)
                    : Node("Command", Node::COMMAND), identifier(identifier), arguments(arguments)
            { }
        public:
            std::shared_ptr<LiteralNode> GetIdentifier() { return identifier; }
            std::shared_ptr<ArgumentsNode> GetArguments() { return arguments; }
        public:
            virtual std::string ToString() const;
        private:
            std::shared_ptr<LiteralNode> identifier;
            std::shared_ptr<ArgumentsNode> arguments;
        };

        /**
            \class AssignmentNode
         */
        class AssignmentNode : public Node {
        public:
            AssignmentNode(const std::shared_ptr<LiteralNode>& identifier, const std::shared_ptr<Node>& expression)
                    : Node("Assignment", Node::ASSIGNMENT), identifier(identifier), expression(expression)
            { }
        public:
            std::shared_ptr<LiteralNode> GetIdentifier() { return identifier; }
            std::shared_ptr<Node> GetExpression() { return expression; }
        public:
            virtual std::string ToString() const;
        private:
            std::shared_ptr<LiteralNode> identifier;
            std::shared_ptr<Node> expression;
        };

        /**
            \class ArgumentsNode


         */
        class ArgumentsNode : public Node {
        public:
            ArgumentsNode() : Node("Arguments", Node::ARGUMENTS) { }
        public:
            void Insert(const std::shared_ptr<Node>& node) {
                arguments.insert(arguments.begin(), node);
            }
        public:
            virtual std::string ToString() const;
        public:
            size_t Size() const { return arguments.size(); }
        public:
            std::vector< std::shared_ptr<Node> >::iterator begin() { return arguments.begin(); }
            std::vector< std::shared_ptr<Node> >::iterator end() { return arguments.end(); }

            std::vector< std::shared_ptr<Node> >::const_iterator begin() const { return arguments.begin(); }
            std::vector< std::shared_ptr<Node> >::const_iterator end() const { return arguments.end(); }
        private:
            std::vector< std::shared_ptr<Node> > arguments;
        };

        class LiteralNode : public Node {
        public:
            LiteralNode(const std::string& text) : Node("Literal", Node::LITERAL), text(text) { }
        public:
            std::string GetText() const { return text; }
        public:
            virtual std::string ToString() const {
                return text;
            }
        private:
            std::string text;
        };

        class StringNode : public Node {
        public:
            StringNode(const std::string& text) : Node("String", Node::STRING), text(text) { }
        public:
            std::string GetText() const { return text; }
        public:
            virtual std::string ToString() const {
                std::stringstream ss;
                ss << "\"" << text << "\"";
                return ss.str();
            }
        private:
            std::string text;
        };

        class NumericNode : public Node {
        public:
            NumericNode(const std::string& text) : Node("Numeric", Node::NUMERIC), text(text) { }
        public:
            std::string GetText() const { return text; }
        public:
            virtual std::string ToString() const {
                return text;
            }
        private:
            std::string text;
        };

        class PreviousNode : public Node {
        public:
            PreviousNode() : Node("Previous", Node::PREVIOUS) { }
        public:
            virtual std::string ToString() const { return "%"; }
        };



        /**
            \class Parser

            Parser

            The EBNF grammar reads

                EXPRESSION
                    = ASSIGNMENT
                    | COMMAND
                    | PREVIOUS
                    ;

                ASSIGNMENT
                    = LITERAL := RHS_EXPRESSION
                    ;

                RHS_EXPRESSION
                    = LITERAL
                    | COMMAND
                    ;

                COMMAND
                    = LITERAL ( ARGUMENTS )
                    ;

                ARGUMENTS
                    = Arguments , Arg
                    | Arg
                    ;

                Argument
                    = Command
                    | Literal
                    | PREVIOUS
                    | STRING
                    | NUMERIC
                    | EMPTY
                    ;

         */
        class Parser {
        public:
            enum Mode {
                DEFAULT = 1,

                ARGUMENT = 100,
                STRING = 101,
                NUMERIC = 102,
            };
        public:
            bool IsNumeric(const std::string& d) const {
                char c = d[0];
                return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
                       c == '5' || c == '6' || c == '7' || c == '8' || c == '9';
            }

            /**
                \brief Lexalize the input

                This method decomposites the input in its ingredients.

                \return Vector with all the input
             */
            void Lexalize(const std::string& code) {
                bool inString = false;
                bool inNumeric = false;
                std::string current;

                for (int i=0; i<code.length(); i++) {
                    std::string c = std::string(1,code[i]);

                    // If string, then append everything
                    if (inString) {
                        if (c == "\"") {
                            tokens.push_back(Token(Token::STRING, i-current.length(), current));
                            current = "";
                            inString = false;
                            continue;
                        }

                        current.append(c);
                        continue;
                    }


                    // If a comment starts, stop parsing
                    if (c == "#") break;

                    // Ignore white spaces
                    if (c == " ") continue;

                    if (inNumeric) {
                        if (!IsNumeric(c)) {
                            tokens.push_back(Token(Token::NUMERIC, i-current.length(), current));
                            current = "";
                            inNumeric = false;
                        }
                    }

                    if (IsNumeric(c)) {
                        inNumeric = true;
                        current.append(c);
                        continue;
                    }

                    if (c == "=") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::ASSIGNMENT, i, c));
                        current = "";
                        continue;
                    }

                    if (c == "(") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::LBRACKET, i, c));
                        current = "";
                        continue;
                    }

                    if (c == ")") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::RBRACKET, i, c));
                        current = "";
                        continue;
                    }

                    if (c == "%") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::PREVIOUS, i, c));
                        current = "";
                        continue;
                    }

                    if (c == ",") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::COMMA, i, c));
                        current = "";
                        continue;
                    }

                    if (c == "\"") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        inString = true;
                        current = "";
                        continue;
                    }

                    // potentially check if it is a letter

                    current.append(c);
                }

                if (current.length() > 0)
                    tokens.push_back(Token(Token::LITERAL, code.length()-current.length(),current));
            }
        private:
            void GetNext() {
                if (currentPos < tokens.size()-1) {
                    current = tokens[++currentPos];
                    if (currentPos < tokens.size()-1)
                        lookAhead = tokens[currentPos+1];
                    else lookAhead = Token(Token::EOL, 0, "");
                }
            }

            std::shared_ptr<Node> ParseArgument() {
                if (current.IsLiteral()) {
                    if (!lookAhead.IsLeftBracket()) {
                        auto res = std::make_shared<LiteralNode>(current.GetContent());
                        GetNext();
                        return res;
                    }

                    return ParseCommand();
                }

                if (current.IsString()) {
                    auto result = std::make_shared<StringNode>(current.GetContent());
                    GetNext();
                    return result;
                }

                if (current.IsNumeric()) {
                    auto result = std::make_shared<NumericNode>(current.GetContent());
                    GetNext();
                    return result;
                }

                if (current.IsPrevious()) {
                    GetNext();
                    return std::make_shared<PreviousNode>();
                }

                return nullptr;
            }

            /**
                \brief Parse arguments

                Arguments are defined by
                    Arguments :== Arguments , Argument
                              |   Argument
                              ;
             */
            std::shared_ptr<ArgumentsNode> ParseArguments() {
                auto arg = ParseArgument();
                if (arg == nullptr) return nullptr;

                if (current.IsComma()) {
                    GetNext();
                    auto args = ParseArguments();
                    args->Insert(std::move(arg));
                    return std::move(args);
                }

                auto args = std::make_shared<ArgumentsNode>();
                args->Insert(std::move(arg));

                return args;
            }

            std::shared_ptr<CommandNode> ParseCommand() {
                if (!current.IsLiteral()) return nullptr;
                auto identifier = std::make_shared<LiteralNode>(current.GetContent());
                GetNext();

                if (!current.IsLeftBracket()) return nullptr;

                GetNext();

                // Parse the arguments
                auto arguments = ParseArguments();
                if (arguments == nullptr) return nullptr;

                if (!current.IsRightBracket()) return nullptr;

                GetNext();

                return std::make_shared<CommandNode>(std::move(identifier), std::move(arguments));
            }

            std::shared_ptr<Node> ParseRHSExpression() {
                if (current.IsPrevious()) {
                    return std::make_shared<PreviousNode>();
                }

                if (current.IsLiteral()) {
                    Token c = current;

                    if (lookAhead.IsLeftBracket()) {
                        return ParseCommand();
                    }

                    // No command and no assignment => print variable
                    return std::make_shared<LiteralNode>(c.GetContent());
                }

                return nullptr;
            }

            std::shared_ptr<AssignmentNode> ParseAssignment() {
                std::shared_ptr<LiteralNode> identifier;
                std::shared_ptr<Node> expression;

                if (!current.IsLiteral()) return nullptr;
                identifier = std::make_shared<LiteralNode>(current.GetContent());
                GetNext();

                if (!current.IsAssignment()) return nullptr;
                GetNext();

                expression = ParseRHSExpression();
                if (expression == nullptr) {
                    return nullptr;
                }

                return std::make_shared<AssignmentNode>(identifier, expression);
            }

            /**
                \brief Parse an expression

                An expression is either an assignment, a command call
                or the previous token %.
             */
            std::shared_ptr<Node> ParseExpression() {
                if (current.IsPrevious()) {
                    return std::make_shared<PreviousNode>();
                }

                if (current.IsLiteral()) {
                    Token c = current;

                    if (lookAhead.IsLeftBracket()) {
                        return ParseCommand();
                    } else if (lookAhead.IsAssignment()) {
                        return ParseAssignment();
                    }

                    // No command and no assignment => print variable
                    return std::move(std::make_shared<LiteralNode>(c.GetContent()));
                }

                return nullptr;
            }
        public:
            std::shared_ptr<Node> Parse(const std::string& code) {
                this->text = code;
                tokens.clear();
                currentPos = 0;

                // Lexalize the input
                Lexalize(code);

                if (tokens.size() < 1) {
                    return nullptr;
                }

                current = tokens[0];
                if (tokens.size() > 1)
                    lookAhead = tokens[1];
                else lookAhead = Token(Token::EOL, 0, "");

                std::shared_ptr<Node> document;

                document = ParseExpression();

                return std::move(document);
            }
        private:
            std::vector<Token> tokens;
            std::string text;

            unsigned currentPos;
            Token current;
            Token lookAhead;
        };


        /***************************************************************************************************************
            IMPLEMENTATION
         **************************************************************************************************************/

        std::string CommandNode::ToString() const {
            std::stringstream ss;
            ss << "(" << name << " " << identifier->ToString() << " " << arguments->ToString() << ")";
            return ss.str();
        }

        std::string ArgumentsNode::ToString() const {
            std::stringstream ss;
            for (int i=0; i<arguments.size(); i++) {
                ss << arguments[i]->ToString();
                if (i != arguments.size()-1) ss << ", ";
            }
            return ss.str();
        }

        std::string AssignmentNode::ToString() const {
            std::stringstream ss;
            ss << identifier->ToString() << " := " << expression->ToString();
            return ss.str();
        }

    }
}