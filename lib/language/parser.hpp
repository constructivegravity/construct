#pragma once

#include <iostream>
#include <sstream>
#include <memory>

namespace Construction {
    namespace Language {

        // Forward declaration
        class Parser;

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
                INDICES,
                STRING,
                NUMERIC,
                PLUS,
                MINUS,
                ASTERISK,
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
            bool IsIndices() const { return type == INDICES; }
            bool IsString() const { return type == STRING; }
            bool IsNumeric() const { return type == NUMERIC; }
            bool IsEndOfLine() const { return type == EOL; }
            bool IsPlus() const { return type == PLUS; }
            bool IsMinus() const { return type == MINUS; }
            bool IsAsterisk() const { return type == ASTERISK; }

            std::string TypeToString() const {
                switch (type) {
                    case LITERAL: return "Literal";
                    case PREVIOUS: return "Previous";
                    case LBRACKET: return "LBracket";
                    case RBRACKET: return "RBracket";
                    case COMMA: return "Comma";
                    case PLUS: return "Plus";
                    case MINUS: return "Minus";
                    case ASTERISK: return "Asterisk";
                    case ASSIGNMENT: return "Assignment";
                    case INDICES: return "Indices";
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
                INDICES,
                STRING,
                NUMERIC,
                PREVIOUS,
                NEGATION,
                BINARY
            };
        public:
            Node(const std::string& name, Type type) : name(name), type(type) { }
        public:
            std::string GetName() const { return name; }
        public:
            bool IsLiteral() const { return type == Node::LITERAL; }
            bool IsNegation() const { return type == Node::NEGATION; }
            bool IsBinary() const { return type == Node::BINARY; }
            bool IsCommand() const { return type == Node::COMMAND; }
            bool IsArguments() const { return type == Node::ARGUMENTS; }
            bool IsAssignment() const { return type == Node::ASSIGNMENT; }
            bool IsPrevious() const { return type == Node::PREVIOUS; }
            bool IsIndices() const { return type == Node::INDICES; }
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
        class BinaryNode;
        class IndicesNode;
        class StringNode;
        class NumericNode;
        class PreviousNode;
        class NegationNode;

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

        class BinaryNode : public Node {
        public:
            BinaryNode(const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs, char op) : Node("Binary", Node::BINARY), lhs(lhs), rhs(rhs), op(op) { }
        public:
            std::shared_ptr<Node> GetLeft() { return lhs; }
            std::shared_ptr<Node> GetRight() { return rhs; }

            char GetOperator() const { return op; }

            bool IsAddition() const { return op == '+'; }
            bool IsSubtraction() const { return op == '-'; }
            bool IsMultiplication() const { return op == '*'; }

            virtual std::string ToString() const {
                std::stringstream ss;
                ss << "(" << op << " " << lhs->ToString() << " " << rhs->ToString() << ")";
                return ss.str();
            }
        private:
            std::shared_ptr<Node> lhs;
            std::shared_ptr<Node> rhs;
            char op;
        };

        class NegationNode : public Node {
        public:
            NegationNode(const std::shared_ptr<Node>& node) : Node("Negation", Node::NEGATION), node(node) { }
        public:
            std::shared_ptr<Node> GetNode() { return node; }
        private:
            std::shared_ptr<Node> node;
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

        class IndicesNode : public Node {
        public:
            IndicesNode(const std::string& text) : Node("Indices", Node::INDICES), text(text) { }
        public:
            std::string GetText() const { return text; }
        public:
            virtual std::string ToString() const {
                std::stringstream ss;
                ss << "{" << text << "}";
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

        class BacktrackingRAII {
        public:
            BacktrackingRAII(Parser& parser);
            virtual ~BacktrackingRAII();
        public:
            void CancelBacktracking() {
                backtrack = false;
            }

            void DoBacktrack() {
                backtrack = true;
            }
        private:
            Parser& parser;
            int position;
            bool backtrack=true;
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
                    = LITERAL = RHS_EXPRESSION
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
                    | INDICES
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
                bool inIndices = false;
                bool inNumeric = false;
                std::string current;

                for (int i=0; i<code.length(); i++) {
                    std::string c = std::string(1,code[i]);

                    // If index, then append everything
                    if (inIndices) {
                        if (c == "}") {
                            tokens.push_back(Token(Token::INDICES, i-current.length(), current));
                            current = "";
                            inIndices = false;
                            continue;
                        }

                        current.append(c);
                        continue;
                    }

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
                        if (!IsNumeric(c) && c != ".") {
                            tokens.push_back(Token(Token::NUMERIC, i-current.length(), current));
                            current = "";
                            inNumeric = false;
                        }
                    }

                    if (c == "." && inNumeric) {
                        current.append(c);
                        continue;
                    }

                    if (IsNumeric(c) || (c == "-" && IsNumeric(std::string(1, code[i+1])))) {
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

                    if (c == "+") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::PLUS, i, c));
                        current = "";
                        continue;
                    }

                    if (c == "-") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::MINUS, i, c));
                        current = "";
                        continue;
                    }

                    if (c == "*") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        tokens.push_back(Token(Token::ASTERISK, i, c));
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

                    if (c == "{") {
                        if (current.length() > 0)
                            tokens.push_back(Token(Token::LITERAL, i-current.length(),current));
                        inIndices = true;
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
        public:
            int GetPosition() const { return currentPos; }
            void GoToPosition(int pos) {
                if (pos > tokens.size()-1) return;

                // Set the tokens
                current = tokens[pos];
                if (pos < tokens.size()-1) lookAhead = tokens[pos+1];
                else lookAhead = Token(Token::EOL, 0, "");

                currentPos = pos;
            }

            void GetNext() {
                if (currentPos < tokens.size()-1) {
                    current = tokens[++currentPos];
                    if (currentPos < tokens.size()-1)
                        lookAhead = tokens[currentPos+1];
                    else lookAhead = Token(Token::EOL, 0, "");
                }
            }
        public:
            std::shared_ptr<Node> ParseIndices() {
                BacktrackingRAII backtracking(*this);

                if (current.IsIndices()) {
                    backtracking.CancelBacktracking();

                    auto result = std::make_shared<IndicesNode>(current.GetContent());

                    // Move cursor
                    GetNext();

                    return std::move(result);
                }

                return nullptr;
            }

            std::shared_ptr<Node> ParseString() {
                BacktrackingRAII backtracking(*this);

                if (current.IsString()) {
                    backtracking.CancelBacktracking();

                    auto result = std::make_shared<StringNode>(current.GetContent());

                    // Move cursor
                    GetNext();

                    return std::move(result);
                }

                return nullptr;
            }

            std::shared_ptr<Node> ParseNumeric() {
                BacktrackingRAII backtracking(*this);

                if (current.IsNumeric()) {
                    backtracking.CancelBacktracking();

                    auto result = std::make_shared<NumericNode>(current.GetContent());

                    // Move cursor
                    GetNext();

                    return std::move(result);
                }

                // Move cursor

                return nullptr;
            }

            std::shared_ptr<LiteralNode> ParseLiteral() {
                BacktrackingRAII backtracking (*this);

                // If not a literal, return
                if (!current.IsLiteral()) return nullptr;

                // Go to the next position
                backtracking.CancelBacktracking();

                // Make result
                auto result = std::make_shared<LiteralNode>(current.GetContent());

                // Move cursor
                GetNext();

                return std::move(result);
            }

            /**
                \brief Parses a primary expression

                Parses a primary expression

                    primary ::=
                            | literal
                            | string
                            | indices
                            | previous
                            | numeric
                            ;
             */
            std::shared_ptr<Node> ParsePrimary() {
                BacktrackingRAII backtracking(*this);

                auto literal = ParseLiteral();
                if (literal) {
                    backtracking.CancelBacktracking();
                    return std::move(literal);
                }

                auto primary = ParseIndices();
                if (primary) {
                    backtracking.CancelBacktracking();
                    return std::move(primary);
                }

                primary = ParseString();
                if (primary) {
                    backtracking.CancelBacktracking();
                    return std::move(primary);
                }

                primary = ParseNumeric();
                if (primary) {
                    backtracking.CancelBacktracking();
                    return std::move(primary);
                }

                if (current.IsPrevious()) {
                    backtracking.CancelBacktracking();

                    auto result = std::make_shared<PreviousNode>();
                    GetNext();

                    return std::move(result);
                }

                return nullptr;
            }

            /**
                \brief Parses a list of arguments

                Parses a list of arguments.

                    arguments ::=
                            | rhs_expression ',' arguments
                            | rhs_expression
                            ;
             */
            std::shared_ptr<ArgumentsNode> ParseArguments() {
                BacktrackingRAII backtracking(*this);

                // Parse for the first argument
                auto arg = ParseRHSExpression();

                if (current.IsComma()) {
                    // Go to next token
                    GetNext();

                    // Parse for the arguments
                    auto args = ParseArguments();
                    if (!args) return nullptr;

                    // Insert the argument at the beginning
                    args->Insert(std::move(arg));

                    backtracking.CancelBacktracking();

                    return std::move(args);
                }

                auto args = std::make_shared<ArgumentsNode>();
                args->Insert(std::move(arg));

                backtracking.CancelBacktracking();

                return std::move(args);
            }

            /**
                \brief Parses a function call expression

                Parses a call expression. This can either be a function call
                or a reference to a variable

                    call_expression ::=
                            | literal '(' arguments ')'
                            | primary
                            ;
             */
            std::shared_ptr<Node> ParseCallExpression() {
                BacktrackingRAII backtracking(*this);

                // If the current token is a literal parse for a function call
                if (current.IsLiteral()) {
                    auto identifier = ParseLiteral();

                    if (!identifier) return nullptr;

                    if (!current.IsLeftBracket()) {
                        // Finally cancel backtracking
                        backtracking.CancelBacktracking();

                        return std::move(identifier);
                    }

                    // Move parser to next token
                    GetNext();

                    auto arguments = ParseArguments();

                    if (!arguments || !current.IsRightBracket()) return nullptr;

                    // Finally cancel backtracking
                    backtracking.CancelBacktracking();

                    // Go to the next token
                    GetNext();

                    return std::make_shared<CommandNode>(std::move(identifier), std::move(arguments));
                }

                // Parse for a primary expression
                auto primary = ParsePrimary();
                if (!primary) return nullptr;

                // Cancel backtracking
                backtracking.CancelBacktracking();

                return std::move(primary);
            }

            /**
                \brief Parses an expression in brackets

                Parses an expression in brackets

                    bracket_expression ::=
                            | '(' rhs_expression ')'
                            | '-' bracket_expression
                            | call_expression
                            ;
             */
            std::shared_ptr<Node> ParseBracketExpression() {
                BacktrackingRAII backtracking(*this);

                // If left bracket
                if (current.IsLeftBracket()) {
                    GetNext();

                    // Parse for an expression
                    auto expression = ParseRHSExpression();
                    if (!expression) return nullptr;

                    if (!current.IsRightBracket()) return nullptr;

                    // Stop backtracking
                    backtracking.CancelBacktracking();

                    return std::move(expression);
                } else if (current.IsMinus()) {
                    GetNext();

                    // Parse the bracket expression
                    auto expression = ParseBracketExpression();

                    // Stop backtracking
                    backtracking.CancelBacktracking();

                    return std::make_shared<NegationNode>(std::move(expression));
                } else {
                    auto expression = ParseCallExpression();

                    if (!expression) return nullptr;

                    // Stop backtracking
                    backtracking.CancelBacktracking();

                    return std::move(expression);
                }
            }

            /**
                \brief Parses a multiplicative expression

                Parses a multiplicative expression.

                    multiplicative_expression ::=
                                | bracket_expression '*' multiplicative_expression
                                | bracket_expression
                                ;
             */
            std::shared_ptr<Node> ParseMultiplicativeExpression() {
                BacktrackingRAII backtracking(*this);

                auto bracket = ParseBracketExpression();
                if (!bracket) return nullptr;

                backtracking.CancelBacktracking();

                if (current.IsAsterisk()) {
                    // Move to next token
                    GetNext();

                    // Parse the next term
                    auto rhs = ParseMultiplicativeExpression();
                    if (!rhs) {
                        backtracking.DoBacktrack();
                        return nullptr;
                    }

                    return std::make_shared<BinaryNode>(std::move(bracket), std::move(rhs), '*');
                } else {
                    return std::move(bracket);
                }
            }

            /**
                \brief Parses a rhs expression

                Parses a right hand side expression. This is either a sum of
                objects or a multiplicative expression.

                    rhs_expression ::=
                                | multiplicative_expression ('+' | '-') rhs_expression
                                | multiplicative_expression
                                ;
             */
            std::shared_ptr<Node> ParseRHSExpression() {
                BacktrackingRAII backtracking(*this);

                auto multiplicative = ParseMultiplicativeExpression();
                if (multiplicative == nullptr) return nullptr;

                // Cancel backtracking
                backtracking.CancelBacktracking();

                // If it is a position
                if (current.IsMinus() || current.IsPlus()) {
                    // Get the operation
                    char op = (current.IsPlus()) ? '+' : '-';

                    // Move to next token
                    GetNext();

                    // Parse the second part
                    auto other = ParseRHSExpression();

                    // If this was not a rhs expression, move back
                    if (other == nullptr) {
                        backtracking.DoBacktrack();
                        return nullptr;
                    }

                    // Build the binary operation node
                    return std::make_shared<BinaryNode>(std::move(multiplicative), std::move(other), op);
                } else {
                    return std::move(multiplicative);
                }
            }

            /**
                Parses an assignment

                    assignment ::= literal '=' rhs_expression
             */
            std::shared_ptr<AssignmentNode> ParseAssignment() {
                BacktrackingRAII backtracking(*this);

                auto identifier = ParseLiteral();
                if (identifier == nullptr) return nullptr;

                if (!current.IsAssignment()) return nullptr;
                GetNext();

                auto expression = ParseRHSExpression();
                if (expression == nullptr) return nullptr;

                // Cancel backtracking
                backtracking.CancelBacktracking();

                return std::make_shared<AssignmentNode>(std::move(identifier), std::move(expression));
            }

            /**
                \brief Parse an expression

                An expression is either an assignment or a rhs expression

                expression ::=
                        | assignment
                        | rhs_expression
                        ;
             */
            std::shared_ptr<Node> ParseExpression() {
                BacktrackingRAII backtrack(*this);

                // Parse an assignment first
                auto node = ParseAssignment();
                if (node != nullptr) {
                    backtrack.CancelBacktracking();
                    return std::move(node);
                }

                // Parse a rhs node otherwise
                auto rhs = ParseRHSExpression();
                if (rhs != nullptr) {
                    backtrack.CancelBacktracking();
                    return std::move(rhs);
                }

                // No expression found, return nullptr
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

        BacktrackingRAII::BacktrackingRAII(Parser& parser) : parser(parser) {
                position = parser.GetPosition();
        }

        BacktrackingRAII::~BacktrackingRAII() {
            if (backtrack) {
                parser.GoToPosition(position);
            }
        }

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
