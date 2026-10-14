#pragma once
#ifndef JSON_HPP
#define JSON_HPP

#ifdef JSON_EXPORTS
#define JSON_API __declspec(dllexport)
#else
#define JSON_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <format>
#include <functional>
#include <iostream>
#include <exception>
#include <stdexcept>
#include "colors.hpp"
using namespace std;

namespace json {
    
    namespace DataTypes {
        class DataBase {
        public:
            virtual string to_string() const = 0;
        };

        class String : public DataBase {
        public:
            string value = "";

            String(const string& val) : value(val) {}

            string to_string() const override {
                return value;
            }
        };
        class Number : public DataBase {
        private:
            double dValue = 0.0;
            int iValue = 0;

            bool isInteger = true;
        public:
            Number(int val) : iValue(val), isInteger(true) {}
            Number(double val) : dValue(val), isInteger(false) {}
            Number(float val) : dValue(val), isInteger(false) {}

            string to_string() const override {
                if (isInteger) {
                    return std::to_string(iValue);
                }
                else {
                    return std::to_string(dValue);
                }
            }

            string getType() const {
                return isInteger ? "int" : "double";
            }

            Number& operator=(int val) {
                iValue = val;
                isInteger = true;
                return *this;
            }
            Number& operator=(double val){
                dValue = val;
                isInteger = false;
                return *this;
            }
        
            operator int() const {
                if (isInteger) {
                    return iValue;
                }
                else {
                    return static_cast<int>(dValue);
                }
            }
            operator double() const {
                if (isInteger) {
                    return static_cast<double>(iValue);
                }
                else {
                    return dValue;
                }
            }
};
        class Boolean : public DataBase {
        public:
            bool value = false;
        
            Boolean(bool val) : value(val) {}

            string to_string() const override {
                return value ? "true" : "false";
            }

        };
        class Array : public DataBase {
        public:
            vector<DataBase*> values = {};
            
            Array() {}

            string to_string() const override {
                string result = "[";
                for (size_t i = 0; i < values.size(); ++i) {
                    result += values[i]->to_string();
                    if (i < values.size() - 1) {
                        result += ", ";
                    }
                }
                result += "]";
                return result;
            }
        
            DataBase* operator[](int index) const {
                if (index < 0 || index >= static_cast<int>(values.size())) {
                    throw std::out_of_range("Index out of range");
                }
                return values[index];
            }
        };
        class Object : public DataBase {
        public:
            map<string, DataBase*> values = {};

            Object() {}

            string to_string() const override {
                string result = "{ ";
                size_t count = 0;
                for (const auto& pair : values) {
                    result += pair.first + ": " + pair.second->to_string();
                    if (count < values.size() - 1) {
                        result += ", ";
                    }
                    ++count;
                }
                result += " }";
                return result;
            }
        
            DataBase* operator[](const string& key) const {
                for (const auto& pair : values) {
                    if (pair.first == key) {
                        return pair.second;
                    }
                }
                return nullptr;
            }
        };
        class Null : public DataBase {
        public:
            Null() {}

            string to_string() const override {
                return "null";
            }
        };
    }

    using DataPtr = DataTypes::DataBase*;

    // The Exception of parsing json strings
    class JsonParsingException : public std::runtime_error {
    private:
        string message;

    public:
        JsonParsingException(const string& msg) : std::runtime_error(msg), message(msg) {}

        const char* what() const noexcept override {
            const char* cachedMessage = new char[message.size() + 1];
            std::copy(message.begin(), message.end(), cachedMessage);

            return cachedMessage;
        }
    };

    class Parser {
    private:
        string jsonString;

    public:
        static vector<string> tokenize(const string& str) {
            vector<string> tokens;
            string currentToken;
            for (int i = 0; i < str.length(); ++i) {
                char c = str[i];
                if (isspace(c)) {
                    if(!currentToken.empty()) {
                        tokens.push_back(currentToken);
                        currentToken.clear();
                    }
                    continue;
                }
                else if (c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',') {
                    if (!currentToken.empty()) {
                        tokens.push_back(currentToken);
                        currentToken.clear();
                    }
                    tokens.push_back(string(1, c));
                }
                else if (c == '"') {
                    if (!currentToken.empty()) {
                        tokens.push_back(currentToken);
                        currentToken.clear();
                    }

                    currentToken += c;
                    i++;
                    for(; i < str.size() - 1 && str[i] != '"'; i++) {
                        if(str[i] == '\\' && i + 1 < str.size() - 1) {
                            currentToken += str[i];
                            i++;
                        }
                        currentToken += str[i];
                    }
                    currentToken += '"';
                    tokens.push_back(currentToken);
                    currentToken.clear();
                }
                else {
                    currentToken += c;
                }
            }
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
            }
            return tokens;
        }
        
    public:
        Parser(const string& jsonString) : jsonString(jsonString) {}
        Parser() : jsonString("") {}

        DataTypes::DataBase* parse() {
            vector<string> tokens = tokenize(jsonString);

            if (tokens.empty()) { return new DataTypes::Null(); }

            // check
            {
                // check brackets
                stack<char> bracketStack;

                for (const string& token : tokens) {
                    if (token == "{" || token == "[") {
                        bracketStack.push(token[0]);
                    }
                    else if (token == "}" || token == "]") {
                        if (bracketStack.empty()) {
                            throw JsonParsingException("Mismatched brackets in JSON string (bracketStack.empty())");
                        }
                        char top = bracketStack.top();
                        if ([&]() -> bool { // check the brackets is matched
                            if (token == "}") return top != '{';
                            if (token == "]") return top != '[';
                            return false;
                        }()) {
                            throw JsonParsingException(std::format(RED "Mismatched brackets in JSON string, need '{}'" RESET, [&]() -> char {
                                if (top == '{') return '}';
                                if (top == '[') return ']';
                                return '?';
                            }()));
                        }
                        bracketStack.pop();
                    }
                } // end each token in tokens

                while (!bracketStack.empty()) {
                    char top_backup = bracketStack.top();
                    throw JsonParsingException(std::format(RED "Mismatched brackets in JSON string, need '{}'" RESET, [&]() -> char {
                        char top = top_backup;
                        if (top == '{') return '}';
                        if (top == '[') return ']';
                        return '?';
                    }()));
                }
            } // end check
            
            DataPtr result = nullptr;
            stack<DataPtr> history;
            if (tokens[0] == "{") {
                result = new DataTypes::Object();
                history.push(result);
            }
            if (tokens[0] == "[") {
                result = new DataTypes::Array();
                history.push(result);
            }

            tokens.erase(tokens.begin());

            // this is the logic of parsing the object and into the datas into it

            // what's the attribute name now
            string currentAttributeName = "";

            for (int i = 0; i < tokens.size(); i++) {
                // the logic of insert data into current object

                // current token
                #define token (tokens[i])

                // check is this token a legal attribute name, and set it into the variable
                if (token[0] != '"' || token.back() != '"') {
                    // if not
                    throw JsonParsingException(std::format(RED "Illegal attribute name: {}" RESET, token));
                } else {
                    // it's legal
                    currentAttributeName = token.substr(1, token.length() - 2);
                }

                // check the next token is a colon
                if (tokens[i + 1] != ":") {
                    throw JsonParsingException(
                        std::format(RED "Expected ':' after attribute name '{}', but found '{}'" RESET, currentAttributeName, tokens[i + 1])
                    );
                }

                i += 2; // skip the attribute name and the colon
                if (i >= tokens.size()) {
                    throw JsonParsingException(std::format(RED "Expected value after attribute name '{}', but found end of tokens" RESET, currentAttributeName));
                }

                // parse the tokens following as the value of the attribute
                DataPtr value = nullptr;
                if (token == "{") {
                    value = new DataTypes::Object();
                    history.push(value);
                }
                if (token == "[") {
                    value = new DataTypes::Array();
                    history.push(value);
                }
                
                // the constant values, it will be continued
                if (token == "null") {
                    value = new DataTypes::Null();
                    continue;
                }
                if (token == "true") {
                    value = new DataTypes::Boolean(true);
                    continue;
                }
                if (token == "false") {
                    value = new DataTypes::Boolean(false);
                    continue;
                }
                if (token[0] == '"') {
                    if (token.back() != '"') {
                        throw JsonParsingException(std::format(RED "Illegal string value: {}, it needs a '\"' to close." RESET, token));
                    }
                    value = new DataTypes::String("");

                    // for each character in token, and process the escape character
                    for (int j = 1;j < token.size() - 2;j++) {
                        // the special: '\' escape character
                        if (token[j] == '\\') {
                            // it will be a escape character

                            if (j + 1 > token.size() - 1) {
                                throw JsonParsingException(std::format(
                                    "In the string literal: \"{}\", needs an escaping character at the tail",
                                    token
                                ));
                            }
                            
                            // what's this escape character, and it will be pushed to the string of value
                            char thisEscapeCharacter = 0;
                            char c = token[++j];
                            switch (c) {
                                case '"':  thisEscapeCharacter = '"';  break;
                                case '\\': thisEscapeCharacter = '\\'; break;
                                case '/':  thisEscapeCharacter = '/';  break;
                                case 'b':  thisEscapeCharacter = '\b'; break;
                                case 'f':  thisEscapeCharacter = '\f'; break;
                                case 'n':  thisEscapeCharacter = '\n'; break;
                                case 'r':  thisEscapeCharacter = '\r'; break;
                                case 't':  thisEscapeCharacter = '\t'; break;
                                default:   thisEscapeCharacter = c;    break;
                            }

                            // the pointer for string of this value
                            DataTypes::String* value_s = static_cast<DataTypes::String*>(value);

                            // push the escape character into the string of this value
                            value_s->value.push_back(thisEscapeCharacter);

                            continue;
                        } // end if it's a escape character
                        
                        // it's gerenal
                        else {
                            // the pointer for string of this value
                            DataTypes::String* value_s = static_cast<DataTypes::String*>(value);
                            
                            value_s->value.push_back(token[j]);
                        
                        }
                    }
                }
                
                // check is the token a number, return 2 bool value
                // the first is is token a number,
                // the second is is token a decimal
                auto checkIsTokenANumber = [&]() -> bool* {

                    bool* result;

                    // is there a dot before this charcter?
                    bool isHasADotBefore = false;

                    for (int j = (token[0] == '-'); j < token.size() - 1; j++) {
                        const char& c = token[j];

                        // it's an illegal charcter
                        if ((c < '0' || c > '9') && c != '.') {
                            result = new bool[2] { false, false };
                            return result;
                        }

                        if (c == '.') {
                            if (isHasADotBefore) {
                                result = new bool[2] { false, false };
                            return result;
                            }
                            isHasADotBefore = true;
                        }
                    }
                
                    result = new bool[2] { true, isHasADotBefore };
                    return result;
                };
                
                if (checkIsTokenANumber()[0]) {
                    value = new DataTypes::Number(0);
                    std::function<void()> parseThisNumber = checkIsTokenANumber()[1] ?
                        // how to parse a decimal that it has dot
                        std::function<void()>([&]() -> void {
                            double resultFloat = 0.0;
                            int resultInt = 0;
                            std::string tokenCache = token;

                            while (!(tokenCache.empty() || tokenCache[0] == '-' || tokenCache[0] == '.')) {
                                resultInt *= 10;
                                resultInt += tokenCache[0] - '0';
                                tokenCache.erase(0, 1);
                            }

                            if (!tokenCache.empty() && tokenCache[0] == '-') {
                                resultInt = 0 - resultInt;
                            }

                            if (!tokenCache.empty() && tokenCache[0] == '.') {
                                // 
                            }

                        }) :
                        // how to parse an integer
                        std::function<void()>([&]() -> void {
                            int result = 0;
                            std::string tokenCache = token;

                            while (!(tokenCache.empty() || tokenCache[0] == '-')) {
                                result *= 10;
                                result += tokenCache[0] - '0';
                                tokenCache.erase(0, 1);
                            }

                            if (!tokenCache.empty() && tokenCache[0] == '-') {
                                result = 0 - result;
                            }

                            // what this number's address
                            DataTypes::Number* valueN = static_cast<DataTypes::Number*>(value);

                            (*valueN) = result;

                            return;

                        });
                }

                // TODO: finish the parsing of the object and array, and the nested objects and arrays
            }

            return nullptr;
        }


    }; // end class Parser


}

#endif // !JSON_HPP