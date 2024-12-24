#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>

#define MAX_TOKEN_LEN 256
#define MAX_VARS 1000
#define MAX_STR_LEN 1024
#define MAX_ERROR_LEN 2048

// Enhanced Token types with more Turkish keywords
typedef enum {
    TOKEN_EOF,
    // Output commands
    TOKEN_YAZDIR,    // print
    TOKEN_GÖSTER,    // display
    
    // Control flow
    TOKEN_EĞER,      // if
    TOKEN_DEĞİLSE,   // else
    TOKEN_DÖNGÜ,     // loop
    TOKEN_DURDURMAK, // break
    TOKEN_ATLAMAK,   // continue
    TOKEN_BİTİR,     // end
    TOKEN_İSE,       // then
    TOKEN_YOKSA,     // else if
    TOKEN_İÇİN,      // for
    TOKEN_YÜRÜT,     // execute
    
    // Logical operators
    TOKEN_VE,        // and
    TOKEN_VEYA,      // or
    TOKEN_DEĞİL,     // not
    
    // Data types
    TOKEN_SAYI,      // number
    TOKEN_METİN,     // string
    TOKEN_MANTIK,    // boolean
    TOKEN_DİZİ,      // array
    TOKEN_ONDALIK,   // decimal/float
    
    // Variables and functions
    TOKEN_DEĞİŞKEN,  // variable
    TOKEN_FONKSİYON, // function
    TOKEN_DÖNÜŞ,     // return
    
    // Operators
    TOKEN_ATAMA,     // =
    TOKEN_ARTI,      // +
    TOKEN_EKSİ,      // -
    TOKEN_ÇARP,      // *
    TOKEN_BÖL,       // /
    TOKEN_MOD,       // %
    TOKEN_EŞİT,      // ==
    TOKEN_EŞİT_DEĞİL, // !=
    TOKEN_BÜYÜK,     // >
    TOKEN_KÜÇÜK,     // <
    TOKEN_BÜYÜK_EŞİT, // >=
    TOKEN_KÜÇÜK_EŞİT, // <=
    
    // Punctuation
    TOKEN_NOKTA,     // .
    TOKEN_VİRGÜL,    // ,
    TOKEN_NOKTALAMA, // ;
    TOKEN_PARANTEZ_AÇ,  // (
    TOKEN_PARANTEZ_KAPAT, // )
    TOKEN_KÖŞE_PARANTEZ_AÇ,    // [
    TOKEN_KÖŞE_PARANTEZ_KAPAT, // ]
    TOKEN_KÜME_PARANTEZ_AÇ,    // {
    TOKEN_KÜME_PARANTEZ_KAPAT, // }
    TOKEN_TIRNAK,    // "
    
    // Special tokens
    TOKEN_YORUM,     // comment
    TOKEN_BOŞ,       // null
    TOKEN_HATA       // error
} TokenType;

// Enhanced Variable types
typedef enum {
    TYPE_INT,        // tam sayı
    TYPE_FLOAT,      // ondalık sayı
    TYPE_STRING,     // metin
    TYPE_BOOLEAN,    // mantıksal
    TYPE_ARRAY,      // dizi
    TYPE_NULL        // boş
} VarType;

// Error types in Turkish
typedef enum {
    HATA_SÖZDİZİMİ,      // Syntax error
    HATA_DEĞİŞKEN,       // Variable error
    HATA_TÜR,           // Type error
    HATA_DEĞER,         // Value error
    HATA_KAPSAM,        // Scope error
    HATA_BELLEK,        // Memory error
    HATA_DOSYA,         // File error
    HATA_ÇALIŞMA        // Runtime error
} HataType;

typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
    int line;       // Line number
    int column;     // Column number
} Token;

typedef struct {
    char name[MAX_TOKEN_LEN];
    VarType type;
    union {
        int int_val;
        double float_val;
        char str_val[MAX_STR_LEN];
        int bool_val;
        struct {
            void* data;
            int size;
            VarType elem_type;
        } array;
    } value;
    int is_constant;    // For constants (sabitler)
    int scope_level;    // For variable scoping
} Variable;

// Function type definition
typedef struct {
    char name[MAX_TOKEN_LEN];
    VarType return_type;
    Variable* parameters;
    int param_count;
    char* body;
    int body_size;
} Function;

// Global variables
char *input;
int pos = 0;
Variable variables[MAX_VARS];
int var_count = 0;
char error_buffer[MAX_ERROR_LEN];
int current_line = 1;
int current_column = 1;
int scope_level = 0;
Function* functions = NULL;
int function_count = 0;

// Turkish character handling
typedef struct {
    const char* utf8;
    const char* ascii;
} TurkishChar;

static const TurkishChar turkish_chars[] = {
    {"ç", "c"}, {"Ç", "C"},
    {"ğ", "g"}, {"Ğ", "G"},
    {"ı", "i"}, {"İ", "I"},
    {"ö", "o"}, {"Ö", "O"},
    {"ş", "s"}, {"Ş", "S"},
    {"ü", "u"}, {"Ü", "U"},
    {NULL, NULL}
};

// Enhanced error handling with Turkish messages
void error(const char* msg, HataType type) {
    char type_str[32];
    switch(type) {
        case HATA_SÖZDİZİMİ:
            strcpy(type_str, "Sözdizimi Hatası");
            break;
        case HATA_DEĞİŞKEN:
            strcpy(type_str, "Değişken Hatası");
            break;
        case HATA_TÜR:
            strcpy(type_str, "Tür Hatası");
            break;
        case HATA_DEĞER:
            strcpy(type_str, "Değer Hatası");
            break;
        case HATA_KAPSAM:
            strcpy(type_str, "Kapsam Hatası");
            break;
        case HATA_BELLEK:
            strcpy(type_str, "Bellek Hatası");
            break;
        case HATA_DOSYA:
            strcpy(type_str, "Dosya Hatası");
            break;
        case HATA_ÇALIŞMA:
            strcpy(type_str, "Çalışma Hatası");
            break;
    }
    
    snprintf(error_buffer, MAX_ERROR_LEN, 
             "HATA: %s\n"
             "Tür: %s\n"
             "Satır: %d\n"
             "Sütun: %d\n"
             "Bağlam: '%.30s...'\n",
             msg, type_str, current_line, current_column, &input[pos]);
    
    fprintf(stderr, "%s", error_buffer);
    exit(1);
}

// Enhanced string handling for Turkish
void normalize_turkish_string(char* dest, const char* src) {
    while (*src) {
        int matched = 0;
        for (const TurkishChar* tc = turkish_chars; tc->utf8; tc++) {
            size_t len = strlen(tc->utf8);
            if (strncmp(src, tc->utf8, len) == 0) {
                *dest++ = *tc->ascii;
                src += len;
                matched = 1;
                break;
            }
        }
        if (!matched) {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
}

// Function to check if a string is a Turkish keyword
int is_turkish_keyword(const char* str) {
    static const char* keywords[] = {
        "yazdır", "göster", "eğer", "değilse", "döngü", "durdurmak",
        "atlamak", "bitir", "ise", "yoksa", "için", "yürüt", "ve",
        "veya", "değil", "sayı", "metin", "mantık", "dizi", "ondalık",
        "değişken", "fonksiyon", "dönüş", "boş", NULL
    };
    
    char normalized[MAX_TOKEN_LEN];
    normalize_turkish_string(normalized, str);
    
    for (const char** kw = keywords; *kw; kw++) {
        char norm_kw[MAX_TOKEN_LEN];
        normalize_turkish_string(norm_kw, *kw);
        if (strcmp(normalized, norm_kw) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to handle Turkish string literals
void parse_turkish_string(char* buffer) {
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
                default: error("Geçersiz kaçış karakteri", HATA_SÖZDİZİMİ);
            }
        } else {
            // Handle UTF-8 Turkish characters
            int matched = 0;
            for (const TurkishChar* tc = turkish_chars; tc->utf8; tc++) {
                size_t len = strlen(tc->utf8);
                if (strncmp(&input[pos], tc->utf8, len) == 0) {
                    strncpy(&buffer[i], tc->utf8, len);
                    i += len;
                    pos += len - 1;
                    matched = 1;
                    break;
                }
            }
            if (!matched) {
                buffer[i++] = input[pos];
            }
        }
        pos++;
    }
    if (input[pos] != '"') error("Kapatılmamış metin dizisi", HATA_SÖZDİZİMİ);
    buffer[i] = '\0';
    pos++; // Skip closing quote
}

// Function to get the next token with Turkish support
Token get_next_token(void) {
    Token token = {TOKEN_EOF, "", current_line, current_column};
    
    while (input[pos] == ' ' || input[pos] == '\n' || input[pos] == '\t') {
        if (input[pos] == '\n') {
            current_line++;
            current_column = 1;
        } else {
            current_column++;
        }
        pos++;
    }

    if (input[pos] == '\0')
        return token;

    // Store start position for error reporting
    token.line = current_line;
    token.column = current_column;

    unsigned char c = input[pos];

    // Handle comments
    if (c == '/' && input[pos + 1] == '/') {
        token.type = TOKEN_YORUM;
        pos += 2;
        int i = 0;
        while (input[pos] != '\n' && input[pos] != '\0') {
            token.value[i++] = input[pos++];
        }
        token.value[i] = '\0';
        return token;
    }

  // Handle strings
    if (c == '"') {
        token.type = TOKEN_METİN;
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
    if (isalpha(c) || c == '_' || is_turkish_letter(c)) {
        int i = 0;
        while (isalnum(input[pos]) || input[pos] == '_' || is_turkish_letter(input[pos])) {
            int char_size = is_turkish_letter(input[pos]);
            if (char_size > 0) {
                // Copy UTF-8 Turkish character (2 bytes)
                token.value[i++] = input[pos++];
                token.value[i++] = input[pos++];
            } else {
                token.value[i++] = input[pos++];
            }
        }
        token.value[i] = '\0';

        // Turkish keywords with proper characters
        if (strcmp(token.value, "yazdır") == 0) token.type = TOKEN_YAZDIR;
        else if (strcmp(token.value, "eğer") == 0) token.type = TOKEN_EĞER;
        else if (strcmp(token.value, "değilse") == 0) token.type = TOKEN_DEĞİLSE;
        else if (strcmp(token.value, "döngü") == 0) token.type = TOKEN_DÖNGÜ;
        else if (strcmp(token.value, "bitir") == 0) token.type = TOKEN_BİTİR;
        else if (strcmp(token.value, "ve") == 0) token.type = TOKEN_VE;
        else if (strcmp(token.value, "veya") == 0) token.type = TOKEN_VEYA;
        else token.type = TOKEN_DEĞİŞKEN;

        return token;
    }

    // Handle operators and special characters
    token.value[1] = '\0';
    token.value[0] = c;
    pos++;

    switch (c) {
        case '=':
            if (input[pos] == '=') {
                token.type = TOKEN_EŞİT;
                token.value[1] = '=';
                token.value[2] = '\0';
                pos++;
            } else {
                token.type = TOKEN_ATAMA;
            }
            break;
        case '+': token.type = TOKEN_ARTI; break;
        case '-': token.type = TOKEN_EKSİ; break;
        case '*': token.type = TOKEN_ÇARP; break;
        case '/': token.type = TOKEN_BÖL; break;
        case '>': token.type = TOKEN_BÜYÜK; break;
        case '<': token.type = TOKEN_KÜÇÜK; break;
        case '.': token.type = TOKEN_NOKTA; break;
        case '(': token.type = TOKEN_PARANTEZ_AÇ; break;
        case ')': token.type = TOKEN_PARANTEZ_KAPAT; break;
        default:
            error("Geçersiz karakter");
    }

    return token;
}


// Modified main function to set locale
int main(int argc, char *argv[]) {
    // Set locale to support Turkish characters
    setlocale(LC_ALL, "tr_TR.UTF-8");
    
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
