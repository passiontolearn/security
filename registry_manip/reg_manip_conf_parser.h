#define STR_EVALUATE(x)   #x
#define STRINGIFY(x)      STR_EVALUATE(x)
#define CONFIG_MAX 1024
#define DEBUG 0

/* Thanks:
* 1. For the beautiful & elegant Linked List example: https://www.sanfoundry.com/c-program-create-linked-list-display-elements/
* 2. Code based on the following parser: https://github.com/welljsjs/Config-Parser-C/blob/master/parser.h
     - Improved parsing   key=value    works as well as
                          key = value  (with spaces)
     - Keys and Values list is returned in correct the order (!)
       (original parser returned them in reverse order...)
     - *Buffer for capturing the key/value is much larger and you only need to update it in one place :)
     - *Fixed a potential Memory leak when it reached EOF
* 3. For the "STRINGIFY(x)" example above:  https://stackoverflow.com/a/2740118
*/
typedef struct config_option config_option;
typedef config_option* config_option_t;
struct config_option {
    config_option_t next;
    char key[CONFIG_MAX];
    char value[CONFIG_MAX];
};

void skipComments(FILE* fp)
{
    // Based on:    http://www2.cs.siu.edu/~wainer/585S00/CUBE/ppmRead.c
    char buf[1024];
    while (1) {
       fscanf(fp, "%1s", buf);
       if (buf[0] != '#') {
            ungetc(buf[0], fp);
            return;
        }
        else {
            fgets(buf, 1024, fp); /* skip comment line */
        }
    }
}

config_option_t read_config_file(char* path) {
    FILE* fp;
    if ((fp = fopen(path, "r+")) == NULL) {
        fprintf(stderr, "fopen() : failed to open or read the conf file '%s'\n", path);
        return NULL;
    }

    config_option_t listhead = NULL, current = 0;
    int first_match = 0;

    while( 1 ) {
        skipComments( fp );  // Skip comments in the config file
        config_option_t conf_opt = NULL;
        if ((conf_opt = calloc(1, sizeof(config_option))) == NULL)
            continue;

        // Match  key = value (ignoring whitespace)
        if ( fscanf(fp, " %"STRINGIFY(CONFIG_MAX)"[^=]=%"STRINGIFY(CONFIG_MAX)"s",
                &conf_opt->key[0], &conf_opt->value[0]) != 2)  {

            if (feof(fp)) {
                free(conf_opt);
                break;
            }
            fprintf(stderr, "fscanf() error while parsing a line in the conf file");
            free(conf_opt);
            continue;
        }
        else {  // We got a valid key = value  pair! :)
            if (first_match != 0) {
                current->next = conf_opt;
                current = conf_opt;
            }
            else {
                first_match = 1;
                listhead = current = conf_opt;
            }
        }
        if (DEBUG) printf("Key: %s\nValue: %s\n", conf_opt->key, conf_opt->value);
    }
    return listhead;
}

