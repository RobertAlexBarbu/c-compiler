#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

void err(const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  fprintf(stderr, "error: ");
  vfprintf(stderr, fmt, va);
  fputc('\n', stderr);
  va_end(va);
  exit(-1);
}

#define SAFEALLOC(var, Type)                        \
  if ((var = (Type *)malloc(sizeof(Type))) == NULL) \
    err("not enough memory");

enum
{
  ID,
  END,
  CT_INT,
  CT_REAL,
  CT_CHAR,
  CT_STRING,
  DIV,
  MUL,
  ADD,
  SUB,
  DOT,
  AND,
  OR,
  EQUAL,
  ASSIGN,
  NOT,
  NOTEQ,
  LESS,
  LESSEQ,
  GREATER,
  GREATEREQ,
  COMMA,
  SEMICOLON,
  LPAR,
  RPAR,
  LBRACKET,
  RBRACKET,
  LACC,
  RACC,
  BREAK,
  CHAR,
  DOUBLE,
  ELSE,
  FOR,
  IF,
  INT,
  RETURN,
  STRUCT,
  VOID,
  WHILE
};

typedef struct _Token
{
  int code;
  union
  {
    char *text;
    long int i;
    double r;
  };
  int line;
  struct _Token *next;
} Token;

Token *lastToken = NULL;
Token *tokens = NULL;
int line = 0;

Token *addTk(int code)
{
  Token *tk;
  SAFEALLOC(tk, Token);
  tk->code = code;
  tk->line = line;
  tk->next = NULL;
  if (lastToken)
  {
    lastToken->next = tk;
  }
  else
  {
    tokens = tk;
  }
  lastToken = tk;
  return tk;
}

void tkerr(const Token *tk, const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  fprintf(stderr, "error in line %d: ", tk->line);
  vfprintf(stderr, fmt, va);
  fputc('\n', stderr);
  va_end(va);
  exit(-1);
}
int xa = 0;
char *createString(const char *pStartChr, char *pCrtCh, int nCh)
{
  char *aux;
  aux = (char *)malloc(nCh * sizeof(char));
  strncpy(aux, pStartChr, nCh);
  return aux;
}

// char *pCrtCh = "abcdef bbbbb22 123 55 01570 0x18A929 10.15 10e-2 10.21e+2 'a' '\n' \"hello\" \"hello\nworld\n\" / //hello\n abc /*aaa*/test /**/ test2 -+*.&& || ===aaaaaa= >=><==< {}[](),; break char double else for if int return struct void while";

char *pCrtCh = NULL;
int getNextToken()
{
  int state = 0, nCh;
  char ch;
  const char *pStartCh;
  Token *tk;
  while (1)
  {
    ch = *pCrtCh;
    switch (state)
    {
    case 0: // testare tranzitii posibile din starea 0
      if (isalpha(ch) || ch == '_')
      {
        pStartCh = pCrtCh; // memoreaza inceputul ID-ului
        pCrtCh++;          // consuma caracterul
        state = 1;         // trece la noua stare
      }
      else if (isdigit(ch) && ch >= '1' && ch <= '9')
      {
        pStartCh = pCrtCh;
        pCrtCh++;
        state = 3;
      }
      else if (isdigit(ch) && ch == '0')
      {
        pStartCh = pCrtCh;
        pCrtCh++;
        state = 5;
      }
      else if (ch == ' ' || ch == '\r' || ch == '\t' || ch == '\n') // consuma caracterul si ramane in starea 0
      {
        pCrtCh++;
      }
      else if (ch == '\n') // tratat separat pentru a actualiza linia curenta
      {
        line++;
        pCrtCh++;
      }
      else if (ch == '\'')
      {
        pStartCh = pCrtCh;
        pCrtCh++;
        state = 14;
      }
      else if (ch == '"')
      {
        pStartCh = pCrtCh;
        pCrtCh++;
        state = 18;
      }
      else if (ch == '/')
      {
        pCrtCh++;
        state = 29;
        // printf("DEBUG1 %c\n", ch);
        // pCrtCh++;
        // printf("DEBUG2 %c\n", ch);
        // if (ch == '/')
        // {
        //   printf("\ngoing to state 30 %c\n", ch);
        //   pCrtCh++;
        //   state = 30;
        // }
        // else
        // {
        //   printf("\ngoing to state 34 %c\n", ch);
        //   state = 34;
        // }
      }
      else if (ch == '+')
      {
        pCrtCh++;
        tk = addTk(ADD);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '-')
      {
        pCrtCh++;
        tk = addTk(SUB);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '*')
      {
        pCrtCh++;
        tk = addTk(MUL);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '.')
      {
        pCrtCh++;
        tk = addTk(DOT);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '&')
      {
        pCrtCh++;
        state = 35;
      }
      else if (ch == '|')
      {
        pCrtCh++;
        state = 36;
      }
      else if (ch == '=')
      {
        pCrtCh++;
        state = 37;
      }
      else if (ch == '<')
      {
        pCrtCh++;
        state = 40;
      }
      else if (ch == '>')
      {
        pCrtCh++;
        state = 43;
      }
      else if (ch == ',')
      {
        pCrtCh++;
        tk = addTk(COMMA);
        tk->line = line;
        return tk->code;
      }
      else if (ch == ';')
      {
        pCrtCh++;
        tk = addTk(SEMICOLON);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '(')
      {
        pCrtCh++;
        tk = addTk(LPAR);
        tk->line = line;
        return tk->code;
      }
      else if (ch == ')')
      {
        pCrtCh++;
        tk = addTk(RPAR);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '[')
      {
        pCrtCh++;
        tk = addTk(LBRACKET);
        tk->line = line;
        return tk->code;
      }
      else if (ch == ']')
      {
        pCrtCh++;
        tk = addTk(RBRACKET);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '{')
      {
        pCrtCh++;
        tk = addTk(LACC);
        tk->line = line;
        return tk->code;
      }
      else if (ch == '}')
      {
        pCrtCh++;
        tk = addTk(RACC);
        tk->line = line;
        return tk->code;
      }
      else if (ch == 0) // sfarsit de sir
      {
        addTk(END);
        return END;
      }
      else
        tkerr(addTk(END), "caracter invalid");
      break;
    case 1:
      if (isalnum(ch) || ch == '_')
        pCrtCh++;
      else
        state = 2;
      break;
    case 2:
      nCh = pCrtCh - pStartCh;
      if (nCh == 5 && !memcmp(pStartCh, "break", 5))
        tk = addTk(BREAK);
      else if (nCh == 4 && !memcmp(pStartCh, "char", 4))
        tk = addTk(CHAR);
      else if (nCh == 6 && !memcmp(pStartCh, "double", 6))
        tk = addTk(DOUBLE);
      else if (nCh == 4 && !memcmp(pStartCh, "else", 4))
        tk = addTk(ELSE);
      else if (nCh == 3 && !memcmp(pStartCh, "for", 3))
        tk = addTk(FOR);
      else if (nCh == 2 && !memcmp(pStartCh, "if", 2))
        tk = addTk(IF);
      else if (nCh == 3 && !memcmp(pStartCh, "int", 3))
        tk = addTk(INT);
      else if (nCh == 6 && !memcmp(pStartCh, "return", 6))
        tk = addTk(RETURN);
      else if (nCh == 6 && !memcmp(pStartCh, "struct", 6))
        tk = addTk(STRUCT);
      else if (nCh == 4 && !memcmp(pStartCh, "void", 4))
        tk = addTk(VOID);
      else if (nCh == 5 && !memcmp(pStartCh, "while", 5))
        tk = addTk(WHILE);
      else
      {
        tk = addTk(ID);
        tk->line = line;
        tk->text = createString(pStartCh, pCrtCh, nCh);
      }
      return tk->code;

    case 3:
      if (isdigit(ch))
        pCrtCh++;
      else if (ch == '.')
      {
        pCrtCh++;
        state = 8;
      }
      else if (ch == 'e' || ch == 'E')
      {
        pCrtCh++;
        state = 10;
      }
      else
        state = 4;
      break;
    case 4:
      nCh = pCrtCh - pStartCh;
      tk = addTk(CT_INT);
      tk->line = line;
      char *str = createString(pStartCh, pCrtCh, nCh);
      char *endptr = NULL;
      if (str[1] == 'x')
      {
        tk->i = strtol(str, &endptr, 16);
      }
      else if (str[0] == '0')
      {
        tk->i = strtol(str, &endptr, 8);
      }
      else
      {
        tk->i = atoi(str);
      }
      return tk->code;
    case 5:
      if (ch == '8' || ch == '9')
        state = 3;
      else if (ch == 'x')
      {
        state = 6;
      }
      else if (isdigit(ch) && ch != '8' && ch != '9')
        pCrtCh++;
      else if (ch == '.')
      {
        pCrtCh++;
        state = 8;
      }
      else if (ch == 'e' || ch == 'E')
      {
        pCrtCh++;
        state = 10;
      }
      else
        state = 4;
      break;
    case 6:
      pCrtCh++;
      if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
        state = 7;
      break;
    case 7:
      if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
        pCrtCh++;
      else
      {
        state = 4;
      }
      break;
    case 8:
      if (isdigit(ch))
      {
        pCrtCh++;
        state = 9;
      }
      break;
    case 9:
      if (isdigit(ch))
      {
        pCrtCh++;
      }
      else if (ch == 'e' || ch == 'E')
      {
        pCrtCh++;
        state = 10;
      }
      else
      {
        state = 13;
      }
      break;
    case 10:
      if (ch == '+' || ch == '-')
      {
        pCrtCh++;
        state = 11;
      }
      else if (isdigit(ch))
      {
        pCrtCh++;
        state = 12;
      }
      break;
    case 11:
      if (isdigit(ch))
      {
        pCrtCh++;
        state = 12;
      }
      break;
    case 12:
      if (isdigit(ch))
      {
        pCrtCh++;
      }
      else
      {
        state = 13;
      }
      break;
    case 13:
      nCh = pCrtCh - pStartCh;
      tk = addTk(CT_REAL);
      tk->line = line;
      char *str2 = createString(pStartCh, pCrtCh, nCh);
      tk->r = strtod(str2, NULL);
      return tk->code;
    case 14:
      if (ch != '\'' && ch != '\\')
      {
        pCrtCh++;
        state = 16;
      }
      else if (ch == '\\')
      {
        pCrtCh++;
        state = 15;
      }
      break;
    case 15:
      if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\' || ch == '0')
      {
        pCrtCh++;
        state = 16;
      }
      break;
    case 16:
      if (ch == '\'')
      {
        pCrtCh++;
        state = 17;
      }
      break;
    case 17:
      nCh = pCrtCh - pStartCh;
      tk = addTk(CT_CHAR);
      tk->line = line;
      tk->text = createString(pStartCh + 1, pCrtCh, nCh - 2);
      return tk->code;
    case 18:
      if (ch != '"' && ch != '\\')
      {
        pCrtCh++;
        state = 21;
      }
      else if (ch == '\\')
      {
        pCrtCh++;
        state = 19;
      }
      break;
    case 19:
      if (ch == 'a' || ch == 'b' || ch == 'f' || ch == 'n' || ch == 'r' || ch == 't' || ch == 'v' || ch == '\'' || ch == '?' || ch == '"' || ch == '\\' || ch == '0')
      {
        pCrtCh++;
        state = 20;
      }
      break;
    case 20:
      if (ch == '"')
      {
        pCrtCh++;
        state = 22;
      }
      else
      {
        state = 18;
      }
      break;
    case 21:
      if (ch == '"')
      {
        pCrtCh++;
        state = 22;
      }
      else
      {
        state = 18;
      }
      break;
    case 22:
      nCh = pCrtCh - pStartCh;
      tk = addTk(CT_STRING);
      tk->line = line;
      tk->text = createString(pStartCh + 1, pCrtCh, nCh - 2);
      return tk->code;
    case 29:
      if (ch == '/')
      {
        pCrtCh++;
        state = 30;
      }
      else if (ch == '*')
      {
        pCrtCh++;
        state = 31;
      }
      else
      {
        state = 34;
      }
      break;
    case 30:
      if (ch != '\n' && ch != '\r' && ch != '\0')
      {
        pCrtCh++;
      }
      else
      {
        state = 0;
      }
      break;
    case 31:
      if (ch == '*')
      {
        pCrtCh++;
        state = 32;
      }
      else if (ch != '*')
      {
        pCrtCh++;
        state = 33;
      }
      break;
    case 32:
      if (ch == '*')
      {
        pCrtCh++;
      }
      else if (ch == '/')
      {
        pCrtCh++;
        state = 0;
      }
      else if (ch != '*' && ch != '/')
      {
        pCrtCh++;
        state = 33;
      }
      break;
    case 33:
      if (ch != '*')
      {
        pCrtCh++;
      }
      else if (ch == '*')
      {
        pCrtCh++;
        state = 32;
      }
      break;
    case 34:
      tk = addTk(DIV);
      tk->line = line;
      return tk->code;
    case 35:
      if (ch == '&')
      {
        pCrtCh++;
        tk = addTk(AND);
        tk->line = line;
        return tk->code;
      }
      break;
    case 36:
      if (ch == '|')
      {
        pCrtCh++;
        tk = addTk(OR);
        tk->line = line;
        return tk->code;
      }
      break;
    case 37:
      printf("aaa\n%c\n", ch);
      if (ch != '=')
      {

        state = 38;
      }
      else if (ch == '=')
      {
        pCrtCh++;
        state = 39;
      }
      break;
    case 38:
      tk = addTk(ASSIGN);
      tk->line = line;
      return tk->code;
    case 39:
      tk = addTk(EQUAL);
      tk->line = line;
      return tk->code;
    case 40:
      if (ch != '=')
      {
        state = 41;
      }
      else if (ch == '=')
      {
        pCrtCh++;
        state = 42;
      }
      break;
    case 41:
      tk = addTk(LESS);
      tk->line = line;
      return tk->code;
    case 42:
      tk = addTk(LESSEQ);
      tk->line = line;
      return tk->code;
    case 43:
      if (ch != '=')
      {
        state = 44;
      }
      else if (ch == '=')
      {
        pCrtCh++;
        state = 45;
      }
      break;
    case 44:
      tk = addTk(GREATER);
      tk->line = line;
      return tk->code;
    case 45:
      tk = addTk(GREATEREQ);
      tk->line = line;
      return tk->code;
    }
  }
}
const char *tokenToString(int token)
{
  switch (token)
  {
  case ID:
    return "ID";
  case END:
    return "END";
  case CT_INT:
    return "CT_INT";
  case CT_REAL:
    return "CT_REAL";
  case CT_CHAR:
    return "CT_CHAR";
  case CT_STRING:
    return "CT_STRING";
  case DIV:
    return "DIV";
  case MUL:
    return "MUL";
  case ADD:
    return "ADD";
  case SUB:
    return "SUB";
  case DOT:
    return "DOT";
  case AND:
    return "AND";
  case OR:
    return "OR";
  case EQUAL:
    return "EQUAL";
  case ASSIGN:
    return "ASSIGN";
  case NOT:
    return "NOT";
  case NOTEQ:
    return "NOTEQ";
  case LESS:
    return "LESS";
  case LESSEQ:
    return "LESSEQ";
  case GREATER:
    return "GREATER";
  case GREATEREQ:
    return "GREATEREQ";
  case COMMA:
    return "COMMA"; // Added case
  case SEMICOLON:
    return "SEMICOLON"; // Added case
  case LPAR:
    return "LPAR"; // Added case
  case RPAR:
    return "RPAR"; // Added case
  case LBRACKET:
    return "LBRACKET"; // Added case
  case RBRACKET:
    return "RBRACKET"; // Added case
  case LACC:
    return "LACC"; // Added case
  case RACC:
    return "RACC"; // Added case
  case BREAK:
    return "BREAK";
  case CHAR:
    return "CHAR";
  case DOUBLE:
    return "DOUBLE";
  case ELSE:
    return "ELSE";
  case FOR:
    return "FOR";
  case IF:
    return "IF";
  case INT:
    return "INT";
  case RETURN:
    return "RETURN";
  case STRUCT:
    return "STRUCT";
  case VOID:
    return "VOID";
  case WHILE:
    return "WHILE";
  default:
    return "UNKNOWN";
  }
}
void printToken(Token *tk)
{
  int code = tokens->code;
  if (code == ID || code == CT_CHAR || code == CT_STRING)
  {
    printf("Token: %s - %s\n", tokenToString(code), tokens->text);
  }
  else if (code == CT_INT)
  {
    printf("Token: %s - %ld\n", tokenToString(code), tokens->i);
  }
  else if (code == CT_REAL)
  {
    printf("Token: %s - %.2f\n", tokenToString(code), tokens->r);
  }
  else
  {
    printf("Token: %s\n", tokenToString(code));
  }
}
/**/
int main()
{
  FILE *file = fopen("./tests/9.c", "r");
  if (file == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  // Seek to the end of the file to determine its size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory to store the entire file content
  pCrtCh = (char *)malloc(size + 1);
  if (pCrtCh == NULL)
  {
    perror("Error allocating memory");
    fclose(file);
    return 1;
  }

  // Read the entire file content into the memory
  size_t bytesRead = fread(pCrtCh, 1, size, file);
  if (bytesRead != size)
  {
    perror("Error reading file");
    free(pCrtCh);
    fclose(file);
    return 1;
  }

  // Null-terminate the content to make it a valid string
  pCrtCh[size] = '\0';

  int tk = getNextToken();
  while (tk != 1)
  {
    printf("%d", tk);
    tk = getNextToken();
  }
  printf("Tokens test\n");
  while (tokens != NULL)
  {
    printToken(tokens);
    tokens = tokens->next;
  }
  return 0;
}
