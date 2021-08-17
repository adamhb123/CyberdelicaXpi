#define MAX_VARS 64
#define MAX_VAR_NAME 32
#define MAX_VAR_VALUE 32

#include <stdio.h>
#include <string.h>


typedef struct Variable {
    char name[MAX_VAR_NAME];
    char value[MAX_VAR_VALUE];

} Variable;

typedef enum SuperPhase{
    VariableParse,
    XpiJSON
} SuperPhase;
typedef enum SubPhase{
    Declaration,
    Assignment,
} SubPhase;

int parse_xpijson(Variable* variable_array, const char* filename, const char* output){
    FILE* fp = fopen(filename, "r");
    char c;
    int var_count = 0;
    int var_name_index = 0;
    int var_val_index = 0;
    SuperPhase superphase = VariableParse;
    SubPhase subphase = Declaration;
    while((c = fgetc(fp)) != EOF){
        // Phase-shifting
        if(c != '\n'){
            if(c == '=') {
                //  Equals means we shift to variable Assignment
                subphase = Assignment;
                var_val_index = 0;
                continue;
            }
            else if(c == '{') {
                // Open bracket indicates we're switching to XpiJSON
                // mode
                superphase = XpiJSON;
                continue;
            }
        } else {
            if(subphase == Assignment) {
                // Newline means we're declaring a new var (or are done)
                var_count++;
                var_name_index = 0;
                subphase = Declaration;
            } else {
                // If we're newlining while still declaring, there is a
                // syntactical problem
                perror("Incorrect syntax");
                return 1;
            }
        }
        // Phase-ops
        if(superphase == VariableParse){
            if(subphase == Declaration){
                variable_array[var_count].name[var_name_index] = c;
                var_name_index++;
            } else if(subphase == Assignment){
                variable_array[var_count].value[var_val_index] = c;
                var_val_index++;
            }
        }
    }
}

int main(int argc, char* argv){
    Variable variable_array[MAX_VARS];
    parse_xpijson(variable_array, "manifest.xpijson", "./build/manifest.json");
    
    return 0;
}
