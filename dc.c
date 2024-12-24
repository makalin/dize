#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 256
#define MAX_VARS 1000
#define MAX_STR_LEN 1024

// Token types
typedef enum {
    TOKEN_EOF,
    TOKEN_YAZDIR,    // print
    TOKEN_EGER,      // if
    TOKEN_DEGILSE,   // else
    TOKEN_DONGU,     // loop
    TOKEN_BITIR,     // end
    TOKEN_VE,        // and
    TOKEN_VEYA,      // or
    TOKEN_SAYI,      // number
    TOKEN_METIN,     // string
    TOKEN_DEGISKEN,  // variable
    TOKEN_ATAMA,     // =
    TOKEN_ARTI,      // +
    TOKEN_EKSI,      // -
    TOKEN_CARP,      // *
    TOKEN_BOL,       // /
    TOKEN_ESIT,      // ==
    TOKEN_BUYUK,     // >
    TOKEN_KUCUK,     // <
    TOKEN_NOKTA,     // .
    TOKEN_PARANTEZ_AC,  // (
    TOKEN_PARANTEZ_KAPAT, // )
    TOKEN_TIRNAK,    // "
} TokenType;

// Variable types
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOLEAN
} VarType;

// Token structure
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
} Token;

// Variable structure
typedef struct {
    char name[MAX_TOKEN_LEN];
    VarType type;
    union {
        int int_val;
        char str_val[MAX_STR_LEN];
        int bool_val;
    } value;
} Variable;

// Global variables
char *input;
int pos = 0;
Variable variables[MAX_VARS];
int var_count = 0;
char error_buffer[1024];

// Function declarations
Token get_next_token(void);
void error(const char *msg);
int find_variable(const char *name);
void execute_program(void);
int evaluate_condition(void);
void skip_whitespace(void);
void parse_string(char *buffer);

// Error handling with more detail
void error(const char *msg) {
    int line = 1;
    int col = 1;
    for (int i = 0; i < pos; i++) {
        if (input[i] == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }
    }
    
    snprintf(error_buffer, sizeof(error_buffer), 
             "Hata (Satır %d, Sütun %d): %s\n"
             "Bağlam: '%.20s...'\n",
             line, col, msg, &input[pos]);
    
    fprintf(stderr, "%s", error_buffer);
    exit(1);
}

// Enhanced lexer
void skip_whitespace(void) {
    while (input[pos] == ' ' || input[pos] == '\n' || input[pos] == '\t')
        pos++;
}

void parse_string(char *buffer) {
    int i = 0;
    pos++; // Skip opening quote
    while (input[pos] != '"' && input[pos] != '\0') {
        if (input[pos] == '\\') {
            pos++;
            switch (input[pos]) {
                case 'n': buffer[i++] = '\n'; break;
                case 't': buffer[i++] = '\t'; break;
                case '"': buffer[i++] = '"'; break;
                case '\\': buffer[i++] = '\\'; break;
                default: error("Geçersiz kaçış karakteri");
            }
        } else {
            buffer[i++] = input[pos];
        }
        pos++;
    }
    if (input[pos] != '"') error("Kapatılmamış metin dizisi");
    buffer[i] = '\0';
    pos++; // Skip closing quote
}

Token get_next_token(void) {
    Token token = {TOKEN_EOF, ""};
    skip_whitespace();

    if (input[pos] == '\0')
        return token;

    char c = input[pos];

    // Handle strings
    if (c == '"') {
        token.type = TOKEN_METIN;
        parse_string(token.value);
        return token;
    }

    // Handle numbers
    if (isdigit(c)) {
        int i = 0;
        while (isdigit(input[pos])) {
            token.value[i++] = input[pos++];
        }
        token.value[i] = '\0';
        token.type = TOKEN_SAYI;
        return token;
    }

    // Handle keywords and identifiers
    if (isalpha(c) || c == '_') {
        int i = 0;
        while (isalnum(input[pos]) || input[pos] == '_') {
            token.value[i++] = input[pos++];
        }
        token.value[i] = '\0';

        // Turkish keywords
        if (strcmp(token.value, "yazdir") == 0) token.type = TOKEN_YAZDIR;
        else if (strcmp(token.value, "eger") == 0) token.type = TOKEN_EGER;
        else if (strcmp(token.value, "degilse") == 0) token.type = TOKEN_DEGILSE;
        else if (strcmp(token.value, "dongu") == 0) token.type = TOKEN_DONGU;
        else if (strcmp(token.value, "bitir") == 0) token.type = TOKEN_BITIR;
        else if (strcmp(token.value, "ve") == 0) token.type = TOKEN_VE;
        else if (strcmp(token.value, "veya") == 0) token.type = TOKEN_VEYA;
        else token.type = TOKEN_DEGISKEN;

        return token;
    }

    // Handle operators and special characters
    token.value[1] = '\0';
    token.value[0] = c;
    pos++;

    switch (c) {
        case '=':
            if (input[pos] == '=') {
                token.type = TOKEN_ESIT;
                token.value[1] = '=';
                token.value[2] = '\0';
                pos++;
            } else {
                token.type = TOKEN_ATAMA;
            }
            break;
        case '+': token.type = TOKEN_ARTI; break;
        case '-': token.type = TOKEN_EKSI; break;
        case '*': token.type = TOKEN_CARP; break;
        case '/': token.type = TOKEN_BOL; break;
        case '>': token.type = TOKEN_BUYUK; break;
        case '<': token.type = TOKEN_KUCUK; break;
        case '.': token.type = TOKEN_NOKTA; break;
        case '(': token.type = TOKEN_PARANTEZ_AC; break;
        case ')': token.type = TOKEN_PARANTEZ_KAPAT; break;
        default:
            error("Geçersiz karakter");
    }

    return token;
}

// Enhanced variable management
int find_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, name) == 0)
            return i;
    }
    return -1;
}

// Evaluate conditions for if statements
int evaluate_condition(void) {
    Token left = get_next_token();
    Token op = get_next_token();
    Token right = get_next_token();
    
    int left_val, right_val;
    
    if (left.type == TOKEN_SAYI) {
        left_val = atoi(left.value);
    } else if (left.type == TOKEN_DEGISKEN) {
        int var_index = find_variable(left.value);
        if (var_index == -1) error("Tanımlanmamış değişken");
        if (variables[var_index].type != TYPE_INT) error("Tür uyuşmazlığı");
        left_val = variables[var_index].value.int_val;
    } else {
        error("Geçersiz karşılaştırma");
    }
    
    if (right.type == TOKEN_SAYI) {
        right_val = atoi(right.value);
    } else if (right.type == TOKEN_DEGISKEN) {
        int var_index = find_variable(right.value);
        if (var_index == -1) error("Tanımlanmamış değişken");
        if (variables[var_index].type != TYPE_INT) error("Tür uyuşmazlığı");
        right_val = variables[var_index].value.int_val;
    } else {
        error("Geçersiz karşılaştırma");
    }
    
    switch (op.type) {
        case TOKEN_ESIT: return left_val == right_val;
        case TOKEN_BUYUK: return left_val > right_val;
        case TOKEN_KUCUK: return left_val < right_val;
        default: error("Geçersiz karşılaştırma operatörü");
    }
    return 0;
}

// Enhanced program execution
void execute_program(void) {
    Token token;
    while ((token = get_next_token()).type != TOKEN_EOF) {
        switch (token.type) {
            case TOKEN_YAZDIR: {
                Token next = get_next_token();
                if (next.type == TOKEN_METIN) {
                    printf("%s\n", next.value);
                } else if (next.type == TOKEN_SAYI) {
                    printf("%s\n", next.value);
                } else if (next.type == TOKEN_DEGISKEN) {
                    int var_index = find_variable(next.value);
                    if (var_index != -1) {
                        switch (variables[var_index].type) {
                            case TYPE_INT:
                                printf("%d\n", variables[var_index].value.int_val);
                                break;
                            case TYPE_STRING:
                                printf("%s\n", variables[var_index].value.str_val);
                                break;
                            case TYPE_BOOLEAN:
                                printf("%s\n", variables[var_index].value.bool_val ? "dogru" : "yanlis");
                                break;
                        }
                    } else {
                        error("Tanımlanmamış değişken");
                    }
                }
                break;
            }
            case TOKEN_EGER: {
                if (evaluate_condition()) {
                    // Execute the if block
                    execute_program();
                } else {
                    // Skip to matching BITIR or DEGILSE
                    int nested = 1;
                    while (nested > 0) {
                        token = get_next_token();
                        if (token.type == TOKEN_EOF) error("Kapatılmamış EGER bloğu");
                        if (token.type == TOKEN_EGER) nested++;
                        if (token.type == TOKEN_BITIR) nested--;
                        if (token.type == TOKEN_DEGILSE && nested == 1) {
                            execute_program();
                            break;
                        }
                    }
                }
                break;
            }
            case TOKEN_DONGU: {
                int start_pos = pos;
                while (evaluate_condition()) {
                    execute_program();
                    pos = start_pos;
                }
                // Skip to matching BITIR
                int nested = 1;
                while (nested > 0) {
                    token = get_next_token();
                    if (token.type == TOKEN_EOF) error("Kapatılmamış DONGU bloğu");
                    if (token.type == TOKEN_DONGU) nested++;
                    if (token.type == TOKEN_BITIR) nested--;
                }
                break;
            }
            case TOKEN_DEGISKEN: {
                char var_name[MAX_TOKEN_LEN];
                strcpy(var_name, token.value);
                
                Token next = get_next_token();
                if (next.type != TOKEN_ATAMA)
                    error("Atama operatörü bekleniyor");

                next = get_next_token();
                int var_index = find_variable(var_name);
                
                if (var_index == -1) {
                    if (var_count >= MAX_VARS)
                        error("Çok fazla değişken");
                    var_index = var_count++;
                    strcpy(variables[var_index].name, var_name);
                }

                switch (next.type) {
                    case TOKEN_SAYI:
                        variables[var_index].type = TYPE_INT;
                        variables[var_index].value.int_val = atoi(next.value);
                        break;
                    case TOKEN_METIN:
                        variables[var_index].type = TYPE_STRING;
                        strcpy(variables[var_index].value.str_val, next.value);
                        break;
                    default:
                        error("Geçersiz değer atama");
                }
                break;
            }
            case TOKEN_BITIR:
                return;
            default:
                error("Beklenmeyen token");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Kullanım: %s <kaynak_dosya>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Dosya açılamadı");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    input = malloc(size + 1);
    if (!input) {
        perror("Bellek hatası");
        fclose(file);
        return 1;
    }

    fread(input, 1, size, file);
    input[size] = '\0';
    fclose(file);

    execute_program();
    free(input);
    return 0;
}
