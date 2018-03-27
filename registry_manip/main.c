#include <windows.h>
#include <stdio.h>
#include "reg_manip_conf_parser.h"

int read_conf_file(config_option_t* reg_opt) {
    if ( (*reg_opt = read_config_file("registry_manip.conf")) == NULL) {
        fprintf(stderr, "read_config_file() : Error reading/parsing file");
        return -1;
    }
    return 0;
}

void print_reg_hash(config_option_t reg_opt) {
    while(1) {
        printf("Key: %s\nValue: %s\n", reg_opt->key, reg_opt->value);
        if (reg_opt->next != NULL) {
            reg_opt = reg_opt->next;
        } else {
            break;
        }
    }
}

void split_str(char str[], char sep[2], char **first_part, char **second_part) {
   *first_part = strtok(str, sep);
    if (first_part) {
        printf("first_part=%s\n", *first_part);
    }

    *second_part = strtok(NULL,"\0");
    if (second_part) {
        printf("second_part=%s\n", *second_part);
    }
}

HKEY get_rook_key(char* key) {
    if (strcmp(key,"HKCR") == 0 || strcmp(key,"HKEY_CLASSES_ROOT") == 0 ) {
        return HKEY_CLASSES_ROOT;
    }
    else if (strcmp(key,"HKCU") == 0 || strcmp(key,"HKEY_CURRENT_USER") == 0 ) {
        return HKEY_CURRENT_USER;
    }
    else if (strcmp(key,"HKLM") == 0 || strcmp(key,"HKEY_LOCAL_MACHINE") == 0 ) {
        return HKEY_LOCAL_MACHINE;
    }
    else if (strcmp(key,"HKU") == 0 || strcmp(key,"HKEY_USERS") == 0 )  {
        return HKEY_USERS;
    }
    else if (strcmp(key,"HKCC") == 0 || strcmp(key,"HKEY_CURRENT_CONFIG") == 0 ) {
        return HKEY_CURRENT_CONFIG;
    }
    else {
            return NULL;  //  NULL means we got an "Unknown root hive key" error.
    }
}

int write_to_reg(config_option_t reg) {
    while(1) {
        char *reg_str = reg->key;
        char sep[2] = "\\";
        char *first_part_hkey, *second_part_hkey;

        // WARNING: the first param sent will also be modified by split_str() !
        split_str(reg_str, sep, &first_part_hkey, &second_part_hkey);

      // TODO:
      // *Improve error handling by skipping invalid values, empty/NULL strings etc'
        if (reg_str && first_part_hkey && second_part_hkey) {
            HKEY root_key = get_rook_key(first_part_hkey);
            TCHAR *szData = TEXT(reg->value);      // Thanks to: http://www.cplusplus.com/forum/beginner/3794/#msg16333
            LONG lRetval = RegSetValueEx(
                root_key,
                TEXT(second_part_hkey),
                0,
                REG_SZ,
                (LPBYTE)szData,
                sizeof(szData)
            );
            if (lRetval != ERROR_SUCCESS) {
                 fprintf(stderr, "Registry set error..., are you running as Administrator?");
            }
        }
        if (reg->next != NULL) {
            reg = reg->next;
        } else {
            break;
        }
    }
    return 0;
}

int main() {
    config_option_t reg_hash = NULL;
    int result = read_conf_file(&reg_hash);
    if (result != 0) return result;

    print_reg_hash(reg_hash);
    result = write_to_reg(reg_hash);
    if (result != 0) return result;

    return 0;
}
