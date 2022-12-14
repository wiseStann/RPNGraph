#include "parse.h"
#include "io.h"
#include <string.h>

parser *new_parser(token **tokens, int tokens_size) {
  parser *_parser = (parser *)malloc(sizeof(struct Parser));
  _parser->brackets = new_op_stack();
  _parser->tokens = tokens;
  _parser->tokens_size = tokens_size;
  _parser->curr_pos = 0;
  _parser->state = 1;
  return _parser;
}

int is_valid_operator(int operator) {
  return operator== '-' || operator== '+' || operator== '/' || operator== '*' ||
  operator== '(' ||
  operator== ')';
}

int expect_token(parser *_parser, token_kind_e expected) {
  token *curr = get_curr_token(_parser);
  int result = 0;
  if (curr != NULL)
    result = curr->kind == expected;
  return result;
}

token *get_curr_token(parser *_parser) {
  token *curr = NULL;
  if (_parser->curr_pos < _parser->tokens_size)
    curr = _parser->tokens[_parser->curr_pos];
  return curr;
}

int get_text_length(parser *_parser) {
  token *last_token = _parser->tokens[_parser->tokens_size - 1];
  return last_token->text_pos + strlen(last_token->str_token);
}

void eat_token(parser *_parser) { _parser->curr_pos++; }

int parse_input(token **tokens, int tokens_size) {
  parser *_parser = new_parser(tokens, tokens_size);
  int result = parse_expression(_parser);
  op_stack_free(_parser->brackets);
  free(_parser);
  return result;
}

int parse_expression(parser *_parser) {
  for (; _parser->curr_pos < _parser->tokens_size;) {
    parse_single_expression(_parser);
    if (!_parser->state)
      break;
  }
  return _parser->state;
}

void parse_single_expression(parser *_parser) {
  token *curr_token = get_curr_token(_parser);
  if (curr_token->kind == TOKEN_FUNCTION_NAME ||
      curr_token->kind == TOKEN_VARIABLE_NAME) {
    parse_function(_parser);
  } else if (curr_token->kind == TOKEN_NUMBER) {
    parse_number(_parser);
  } else if (curr_token->kind == TOKEN_OPEN_BRACKET) {
    parse_parent_expression(_parser);
  } else {
    parse_operator(_parser);
  }
}

void parse_parent_expression(parser *_parser) {
  token *bracket = get_curr_token(_parser);
  op_stack_push(_parser->brackets, bracket);
  eat_token(_parser); // skip open bracket
  token *current = NULL;

  while ((current = get_curr_token(_parser)) != NULL &&
         current->kind != TOKEN_CLOSE_BRACKET) {
    parse_single_expression(_parser);
    if (!_parser->state)
      return;
  }
  if (current == NULL) {
    _THROW_ERROR(get_text_length(_parser),
                 "Expected close bracket at the end of parenth expression\n");
    _parser->state = 0;
  } else {
    eat_token(_parser); // skip close bracket
    op_node *open_bracket = op_stack_pop(_parser->brackets);
    if (open_bracket == NULL) {
      _THROW_ERROR(current->text_pos,
                   "Can't find matching open bracket for ')'\n");
      _parser->state = 0;
    }
    free(open_bracket);
  }
}

void parse_operator(parser *_parser) {
  token *current = get_curr_token(_parser);
  if (!is_valid_operator(current->operator.text)) {
    _THROW_ERROR(current->text_pos, "Invalid operator token '%c'\n",
                 current->operator.text);
    _parser->state = 0;
  } else if (current->kind == TOKEN_UN_PLUS) {
    _THROW_ERROR(current->text_pos, "Unary plus operator is not allowed\n");
    _parser->state = 0;
  } else if (current->kind == TOKEN_OPEN_BRACKET ||
             current->kind == TOKEN_CLOSE_BRACKET) {
    _THROW_ERROR(current->text_pos, "Incorrect place for bracket\n");
    _parser->state = 0;
  } else {
    eat_token(_parser); // skip operator token
  }
}

void parse_function(parser *_parser) {
  token *current = get_curr_token(_parser);
  if (current->func_id == 0 && strcmp(current->str_token, "x") != 0) {
    _THROW_ERROR(current->text_pos, "Invalid variable or function name '%s'\n",
                 current->str_token);
    _parser->state = 0;
  } else if (strcmp(current->str_token, "x") == 0) {
    eat_token(_parser); // skip x
  } else {
    eat_token(_parser); // skip function name
    token *bracket = get_curr_token(_parser);
    if (!expect_token(_parser, TOKEN_OPEN_BRACKET)) {
      int text_position;
      if (bracket == NULL)
        text_position = get_text_length(_parser);
      else
        text_position = bracket->text_pos;
      _THROW_ERROR(text_position, "Expected '(' token after function name\n");
      _parser->state = 0;
    } else {
      eat_token(_parser); // skip open bracket
      op_stack_push(_parser->brackets, bracket);
      while ((current = get_curr_token(_parser)) != NULL &&
             current->kind != TOKEN_CLOSE_BRACKET) {
        parse_single_expression(_parser); // parse entire function expression
        if (!_parser->state)
          return;
      }
      if (current == NULL) {
        _THROW_ERROR(get_text_length(_parser),
                     "Expected ')' token after function argument\n");
        _parser->state = 0;
      } else {
        eat_token(_parser); // skip close bracket
        op_node *open_bracket = op_stack_pop(_parser->brackets);
        if (open_bracket == NULL) {
          _THROW_ERROR(bracket->text_pos,
                       "Can't find matching open bracket for ')'\n");
          _parser->state = 0;
        }
        free(open_bracket);
      }
    }
  }
}

void parse_number(parser *_parser) {
  eat_token(_parser); // skip number
}
